// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* PETScDoFLinearSystem.cc                                     (C) 2022-2025 */
/*                                                                           */
/* Linear system: Matrix A + Vector x + Vector b for Ax=b.                   */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "DoFLinearSystem.h"

#include <mpi.h>
#include <arccore/trace/TraceAccessor.h>

#include "arcane/accelerator/core/IAcceleratorMng.h"
#include "arcane/accelerator/Reduce.h"
#include "arcane/accelerator/Accelerator.h"
#include "arcane/accelerator/RunCommandEnumerate.h"
#include "arcane/accelerator/NumArrayViews.h"
#include "arcane/accelerator/SpanViews.h"

#include <arcane/accelerator/RunCommandLoop.h>
#include <arcane/utils/FatalErrorException.h>
#include <arcane/utils/PlatformUtils.h>
#include <arcane/utils/ArcaneGlobal.h>
#include <arcane/utils/MemoryUtils.h>
#include <arcane/utils/MemoryView.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/NumArray.h>
#include <arcane/utils/CommandLineArguments.h>

#include <arcane/core/ServiceFactory.h>
#include <arcane/core/VariableTypes.h>
#include <arcane/core/BasicService.h>
#include <arcane/core/IParallelMng.h>
#include <arcane/core/IItemFamily.h>
#include <arcane/core/ItemPrinter.h>
#include <arcane/core/Timer.h>

#include <arcane/accelerator/VariableViews.h>
#include <arcane/accelerator/core/Runner.h>
#include <arcane/accelerator/core/Memory.h>

#include "IDoFLinearSystemFactory.h"
#include "internal/CsrDoFLinearSystemImpl.h"

#include <alien/arcane_tools/accessors/ItemVectorAccessor.h>
#include <alien/core/block/VBlock.h>

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


#include <alien/expression/solver/ILinearSolver.h>

#include <alien/ref/AlienRefSemantic.h>

#include <alien/AlienLegacyConfig.h>


#include "CAWF/FEMUtils/AlienBSRFormat.h"
#include "CAWF/FEMUtils/AlienDoFLinearSystem.h"

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

namespace Arcane::FemUtils
{

using namespace Arcane;
using namespace Alien;

namespace ax = Arcane::Accelerator;

void  AlienDoFLinearSystemImpl::
_setVectorsAcc(Alien::Vector& vectorX, Alien::Vector& vectorB)
{
  IItemFamily* dof_family = dofFamily();
  VariableDoFReal& rhs_variable = this->rhsVariable();
  VariableDoFReal& dof_variable = this->solutionVariable();
  auto rhs_data = rhs_variable.asArray();
  auto result_data = dof_variable.asArray();
  Arccore::UniqueArray<Arccore::Integer> dof_lids(platform::getDefaultDataAllocator()) ;
  dof_lids = dof_family->allItems().own().view().localIds() ;
  Arccore::SmallSpan<const Int32> accAllUIndex = m_alien_bsr_format->getAllUIndex() ;

  {
    auto vx_acc = Alien::SYCL::VectorAccessorT<Real>(vectorX);
    auto vb_acc = Alien::SYCL::VectorAccessorT<Real>(vectorB);

    Alien::ParallelEngine engine(m_alien_bsr_format->queue()) ;

    engine.submit([&](ControlGroupHandler& handler)
                  {
                    auto& command         = handler.command() ;
                    auto in_dof_lids      = ax::viewIn(command,dof_lids);
                    auto in_allUIndex     = ax::viewIn(command,accAllUIndex) ;
                    auto in_result        = ax::viewIn(command,dof_variable);
                    auto in_rhs           = ax::viewIn(command,rhs_variable);

                    auto out_vx = vx_acc.view(handler) ;
                    auto out_vb = vb_acc.view(handler) ;

                    auto local_size = dof_lids.size() ;
                    handler.parallel_for(engine.maxNumThreads(),
                                         [=](Alien::ParallelEngine::Item<1>::type item)
                                         {
                                            auto id = item.get_id(0) ;
                                            for (auto index = id; index < local_size; index += item.get_range()[0])
                                            {
                                              auto vi   = DoFLocalId(in_dof_lids[index]) ;
                                              auto lid     = in_dof_lids[index] ;
                                              auto iIndex = in_allUIndex[lid];
                                              if(iIndex!=-1)
                                              {
                                                out_vx[iIndex] = in_result[vi] ;
                                                out_vb[iIndex] = in_rhs[vi] ;
                                              }
                                            } ;
                                         }) ;

                  }) ;
  }
}

void  AlienDoFLinearSystemImpl::
_getSolutionAcc(Alien::Vector& vectorX)
{
  IItemFamily* dof_family = dofFamily();
  VariableDoFReal& rhs_variable = this->rhsVariable();
  VariableDoFReal& dof_variable = this->solutionVariable();
  Arccore::UniqueArray<Arccore::Integer> dof_lids(platform::getDefaultDataAllocator()) ;
  dof_lids = dof_family->allItems().own().view().localIds() ;

  Arccore::SmallSpan<const Int32> accAllUIndex = m_alien_bsr_format->getAllUIndex() ;
  {
    auto vx_acc = Alien::SYCL::VectorAccessorT<Real>(vectorX);

    Alien::ParallelEngine engine(m_alien_bsr_format->queue()) ;

    engine.submit([&](ControlGroupHandler& handler)
                  {
                    auto& command         = handler.command() ;
                    auto in_dof_lids      = ax::viewIn(command,dof_lids);
                    auto in_allUIndex     = ax::viewIn(command,accAllUIndex) ;
                    auto out_result        = ax::viewOut(command,dof_variable);

                    auto in_vx = vx_acc.view(handler) ;

                    auto local_size = dof_lids.size() ;
                    handler.parallel_for(engine.maxNumThreads(),
                                         [=](Alien::ParallelEngine::Item<1>::type item)
                                         {
                                            auto id = item.get_id(0) ;
                                            for (auto index = id; index < local_size; index += item.get_range()[0])
                                            {
                                              auto vi   = DoFLocalId(in_dof_lids[index]) ;
                                              auto lid     = in_dof_lids[index] ;
                                              auto iIndex = in_allUIndex[lid];
                                              if(iIndex!=-1)
                                              {
                                                out_result[vi] = in_vx[iIndex];
                                              }
                                            } ;
                                         }) ;

                  }) ;
  }
}


} // namespace Arcane::FemUtils
