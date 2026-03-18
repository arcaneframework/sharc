// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* FemModule.cc                                                (C) 2022-2025 */
/*                                                                           */
/* FEM code to test vectorial FE for Elasticity problem.                     */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/core/IParallelMng.h>

#include "arcane/accelerator/core/IAcceleratorMng.h"
#include "arcane/accelerator/Reduce.h"
#include "arcane/accelerator/Accelerator.h"
#include "arcane/accelerator/RunCommandEnumerate.h"
#include "arcane/accelerator/NumArrayViews.h"
#include "arcane/accelerator/SpanViews.h"

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Utils/CAWF/ICAWFMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#include "FemModule.h"
#include "ElementMatrixSYCL.h"
#include "ElementMatrixHexQuadSYCL.h"
//#include "BodyForce.h"
//#include "Traction.h"
//#include "Dirichlet.h"

#include <alien/arcane_tools/IIndexManager.h>
#include <alien/arcane_tools/indexManager/BasicIndexManager.h>
#include <alien/arcane_tools/indexManager/SimpleAbstractFamily.h>
#include <alien/arcane_tools/distribution/DistributionFabric.h>
#include <alien/arcane_tools/indexSet/IndexSetFabric.h>
#include <alien/arcane_tools/data/Space.h>

#include <alien/handlers/scalar/sycl/VectorAccessorT.h>
#include <alien/handlers/scalar/sycl/MatrixProfiler.h>
#include <alien/handlers/scalar/sycl/ProfiledMatrixBuilderT.h>
#include <alien/handlers/scalar/sycl/CombineProfiledMatrixBuilderT.h>

#include <alien/handlers/scalar/sycl/CombineProfiledMatrixBuilderImplT.h>
#include <alien/handlers/scalar/sycl/VectorAccessorImplT.h>
#include <alien/handlers/scalar/sycl/ProfiledMatrixBuilderImplT.h>


#include <alien/ref/AlienRefSemantic.h>


#include <alien/arcane_tools/accelerator/ArcaneParallelEngine.h>

#include <alien/arcane_tools/accelerator/ArcaneParallelEngineImplT.h>

#ifdef ALIEN_USE_SYCL
#include <alien/kernels/sycl/SYCLPrecomp.h>

#include "alien/kernels/sycl/data/SYCLEnv.h"
#include "alien/kernels/sycl/data/SYCLEnvInternal.h"

#include <alien/kernels/sycl/data/SYCLBEllPackMatrix.h>
#include <alien/kernels/sycl/data/SYCLVector.h>
#include <alien/kernels/sycl/algebra/SYCLLinearAlgebra.h>

#include "alien/kernels/sycl/data/SYCLVectorInternal.h"
#include <alien/kernels/sycl/data/SYCLBEllPackInternal.h>
#include <alien/kernels/sycl/algebra/SYCLInternalLinearAlgebra.h>

#include "alien/kernels/sycl/data/SYCLSendRecvOp.h"
#include "alien/kernels/sycl/data/SYCLLUSendRecvOp.h"
#include <alien/kernels/sycl/algebra/SYCLKernelInternal.h>
#endif

namespace ax = Arcane::Accelerator;

template <class FunctionT>
void FemModuleElasticity::
_assembleBilinearOperatorGpu(const FunctionT& compute_element_matrix)
{
  const Int32 dim = mesh()->dimension();
  UnstructuredMeshConnectivityView connectivity_view(mesh());
  auto cell_node_cv = connectivity_view.cellNode();
  auto node_cell_cv = connectivity_view.nodeCell();

  ItemGenericInfoListView nodes_infos(mesh()->nodeFamily());

  auto node_dof(m_dofs_on_nodes.nodeDoFConnectivityView());

  auto& matrixA = m_alien_bsr_format->getMatrixA();
  auto& vectorX = m_alien_bsr_format->getVectorX();
  auto& vectorB = m_alien_bsr_format->getVectorB();

  Arccore::SmallSpan<const Int32> accAllUIndex = m_alien_bsr_format->getAllUIndex() ;

  Arccore::UniqueArray<Arccore::Integer> node_lids(platform::getDefaultDataAllocator()) ;
  node_lids = mesh()->ownNodes().view().localIds() ;
  Arccore::UniqueArray<Arccore::Integer> all_node_lids(platform::getDefaultDataAllocator()) ;
  all_node_lids = mesh()->allNodes().view().localIds() ;
  Arccore::UniqueArray<Arccore::Integer> cell_lids(platform::getDefaultDataAllocator()) ;
  cell_lids = mesh()->allCells().view().localIds() ;
  Arccore::UniqueArray<Arccore::Integer> own_cell_lids(platform::getDefaultDataAllocator()) ;
  own_cell_lids = mesh()->ownCells().view().localIds() ;
  {
    Alien::SYCL::ProfiledMatrixBuilder builder(matrixA, Alien::ProfiledMatrixOptions::eResetValues);
    //builder.setParallelAssembleStencil(1,m_dof_dof_connection_offset.view(),m_dof_dof_connection_index.view()) ;

    Alien::ParallelEngine engine(*(acceleratorMng()->defaultQueue())) ;
    engine.submit([&](ControlGroupHandler& handler)
                  {
                    auto& command = handler.command() ;

                    auto in_allUIndex      = ax::viewIn(command,accAllUIndex) ;
                    auto in_node_lids      = ax::viewIn(command,node_lids) ;

                    auto in_node_coord     = ax::viewIn(command, m_node_coord);
                    auto in_lambda         = ax::viewIn(command, m_cell_lambda);
                    auto in_mu             = ax::viewIn(command, m_cell_mu);

                    auto matrix_acc = builder.view(handler) ;
                    auto local_size = mesh()->ownNodes().size() ;

                    handler.parallel_for(engine.maxNumThreads(),
                                         [=](Alien::ParallelEngine::Item<1>::type item)
                                         {
                                            auto id = item.get_id(0) ;
                                            for (auto index = id; index < local_size; index += item.get_range()[0])
                                            {
                                              auto node0_lid = in_node_lids[index] ;
                                              auto node0 = NodeLocalId(node0_lid) ;
                                              for (auto cell : node_cell_cv.cells(node0))
                                              {
                                                auto K_e = compute_element_matrix(cell,
                                                                                  cell_node_cv,
                                                                                  in_node_coord,
                                                                                  in_lambda,
                                                                                  in_mu);
                                                Int32 n1_index = 0;
                                                for (auto node1 : cell_node_cv.nodes(cell))
                                                {
                                                  if(node1==node0)
                                                  {
                                                    Int32 n2_index = 0;
                                                    for (auto node2 : cell_node_cv.nodes(cell))
                                                    {
                                                      for (Int32 i = 0; i < dim; ++i)
                                                      {
                                                        DoFLocalId dof1 = node_dof.dofId(node1, i);
                                                        Integer row_i = in_allUIndex[dof1];
                                                        for (Int32 j = 0; j < dim; ++j)
                                                        {
                                                          DoFLocalId dof2 = node_dof.dofId(node2, j);
                                                          Integer col_j = in_allUIndex[dof2];
                                                          auto eij = matrix_acc.entryIndex(row_i, col_j) ;
                                                          Real value = K_e(dim * n1_index + i, dim * n2_index + j);
                                                          matrix_acc[eij] += value;
                                                        }
                                                      }
                                                      ++n2_index;
                                                    }
                                                  }
                                                }
                                              }
                                            } ;
                                          }) ;
                  }) ;
  }
}

void FemModuleElasticity::
_assembleBilinearOperatorAlienAcc()
{
  UnstructuredMeshConnectivityView m_connectivity_view(mesh());
  if (mesh()->dimension() == 2)
  {
    if (m_hex_quad_mesh)
    {
      _assembleBilinearOperatorGpu([=] ARCCORE_HOST_DEVICE (CellLocalId cell,
                                                            const IndexedCellNodeConnectivityView& cn_cv,
                                                            const Accelerator::VariableNodeReal3InView& node_coord,
                                                            const Accelerator::VariableCellRealInView& lambda,
                                                            const Accelerator::VariableCellRealInView& mu)
                                    {
                                        return computeElementMatrixQuad4Gpu(cell,cn_cv,node_coord,lambda,mu);
                                    });
    }
    else
    {
      _assembleBilinearOperatorGpu([=] ARCCORE_HOST_DEVICE (CellLocalId cell,
                                                            const IndexedCellNodeConnectivityView& cn_cv,
                                                            const Accelerator::VariableNodeReal3InView& node_coord,
                                                            const Accelerator::VariableCellRealInView& lambda,
                                                            const Accelerator::VariableCellRealInView& mu)
                                   {
                                      return computeElementMatrixTria3SYCL(cell,cn_cv,node_coord,lambda,mu);
                                   });
    }
  }
  if (mesh()->dimension() == 3)
  {
    if (m_hex_quad_mesh)
    {
      _assembleBilinearOperatorGpu([=] ARCCORE_HOST_DEVICE (CellLocalId cell,
                                                            const IndexedCellNodeConnectivityView& cn_cv,
                                                            const Accelerator::VariableNodeReal3InView& node_coord,
                                                            const Accelerator::VariableCellRealInView& lambda,
                                                            const Accelerator::VariableCellRealInView& mu)
                                   {
                                      return computeElementMatrixHexa8Gpu(cell,cn_cv,node_coord,lambda,mu);
                                   });
    }
    else
    {
      _assembleBilinearOperatorGpu([=] ARCCORE_HOST_DEVICE (CellLocalId cell,
                                                            const IndexedCellNodeConnectivityView& cn_cv,
                                                            const Accelerator::VariableNodeReal3InView& node_coord,
                                                            const Accelerator::VariableCellRealInView& lambda,
                                                            const Accelerator::VariableCellRealInView& mu)
                                   {
                                      return computeElementMatrixTetra4SYCL(cell,cn_cv,node_coord,lambda,mu);
                                   });
    }
  }
}

void FemModuleElasticity::
_defineMatrixProfileAcc()
{
  auto func = [this](Alien::SYCL::MatrixProfiler& profiler,ConstArrayView<Integer> allUIndex)
              {
                return this->_defineMatrixProfile(profiler,allUIndex) ;
              } ;
  m_alien_bsr_format->computeProfile<Alien::SYCL::MatrixProfiler>(func);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
