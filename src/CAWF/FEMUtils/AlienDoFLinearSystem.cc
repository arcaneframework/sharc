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

#include <alien/handlers/scalar/CSRModifierViewT.h>
#include <alien/kernels/simple_csr/algebra/SimpleCSRLinearAlgebra.h>
#include <alien/kernels/simple_csr/algebra/SimpleCSRInternalLinearAlgebra.h>

#include <alien/ref/AlienRefSemantic.h>
#include <alien/ref/AlienImportExport.h>

#include <alien/expression/solver/ILinearSolver.h>
#include <alien/expression/solver/SolverStater.h>
#include <alien/AlienLegacyConfig.h>

#include <AlienDoFLinearSystemFactory_axl.h>


#include "CAWF/FEMUtils/AlienBSRFormat.h"
#include "CAWF/FEMUtils/AlienDoFLinearSystem.h"

namespace Arcane::FemUtils
{

using namespace Arcane;
using namespace Alien;

AlienDoFLinearSystemImpl::AlienDoFLinearSystemImpl(IItemFamily* dof_family, const String& solver_name)
//: CsrDoFLinearSystemImpl(dof_family, solver_name)
: DoFLinearSystemImplBase(dof_family, solver_name)
//, m_dof_matrix_numbering(VariableBuildInfo(dof_family, solver_name + "MatrixNumbering"))
{
  info() << "[Alien-Info] Creating AlienDoFLinearSystemImpl()";
}

AlienDoFLinearSystemImpl::~AlienDoFLinearSystemImpl()
{
  info() << "[Alien-Info] Calling AlienDoFLinearSystemImpl destructor";
}

void  AlienDoFLinearSystemImpl::
_setVectorsCpu(Alien::Vector& vectorX, Alien::Vector& vectorB)
{
  IItemFamily* dof_family = dofFamily();
  VariableDoFReal& rhs_variable = this->rhsVariable();
  VariableDoFReal& dof_variable = this->solutionVariable();
  auto areaU = dof_family->allItems();
  {
    Alien::VectorWriter writer(vectorX);

    ENUMERATE_DOF (idof, areaU.own())
    {
      const Integer iIndex = m_allUIndex[idof->localId()];
      // info() << "local: " << idof->localId() << " global: " << idof.index();
      // info() << "local " << idof->localId()<< " iIndex " << iIndex << " res val: " << result_data[idof->localId()];
      writer[iIndex] = dof_variable[*idof];
    }
  }

  {
    Alien::VectorWriter writer(vectorB);

    ENUMERATE_DOF (idof, areaU.own()) {
      const Integer iIndex = m_allUIndex[idof->localId()];
      writer[iIndex] = rhs_variable[*idof];
    }
  }
}


void AlienDoFLinearSystemImpl::
_getSolutionCpu(Alien::Vector& vectorX)
{
  VariableDoFReal& dof_variable = this->solutionVariable();
  Alien::VectorReader reader(vectorX);
  ENUMERATE_DOF(idof,  dofFamily()->allItems().own())
  {
    const Integer iIndex = m_allUIndex[idof->localId()];
    dof_variable[idof] = reader[iIndex];
  }
}



void AlienDoFLinearSystemImpl::
_printMatrixCpu(Alien::Matrix& matrix)
{
  auto const& true_A = matrix.impl()->get<Alien::BackEnd::tag::simplecsr>() ;

  Alien::CSRConstViewT<SimpleCSRMatrix<Real>> view(true_A);
  auto nrows  = view.nrows();
  auto kcol   = view.kcol() ;
  auto cols   = view.cols() ;
  auto values = view.data() ;
  for(int irow=0;irow<nrows;++irow)
  {
    std::cout<<"ROW["<<irow<<"] : ";
    for(int k=kcol[irow];k<kcol[irow+1];++k)
    {
      std::cout<<"("<<cols[k]<<","<<values[k]<<") ";
    }
    std::cout<<std::endl;
  }
}

void AlienDoFLinearSystemImpl::
solve()
{
  info() << "[Alien-Info] Calling Alien solver";

  IItemFamily* dof_family = dofFamily();
  IParallelMng* pm = dof_family->parallelMng();
  Runner runner = this->runner();

  Real a1 = platform::getRealTime();

  /*
  CSRFormatView csr_view = this->getCSRValues();

  auto areaU = dof_family->allItems();

  Alien::ArcaneTools::BasicIndexManager index_manager(pm);
  index_manager.setTraceMng(traceMng());

  auto indexSetU = index_manager.buildScalarIndexSet("U", areaU);
  index_manager.prepare();
  UniqueArray<Arccore::Integer> allUIndex = index_manager.getIndexes(indexSetU);

  Alien::ArcaneTools::Space space(&index_manager, "TestSpace");
  auto mdist = Alien::ArcaneTools::createMatrixDistribution(space);
  auto vdist = Alien::ArcaneTools::createVectorDistribution(space);

  Alien::Vector vectorB(vdist);
  Alien::Vector vectorX(vdist);
  Alien::Matrix matrixA(mdist);
  // local matrix for exact measure without side effect
  // (however, you can reuse a matrix with several
  // builder)


  pm->barrier();

  {
    Alien::MatrixProfiler profiler(matrixA);
    ///////////////////////////////////////////////////////////////////////////
    //
    // DEFINE PROFILE
    //
    ENUMERATE_DOF(idof, dof_family->allItems()) {
      if (!idof->isOwn()) {
        continue;
      }

      int i = idof.index();
      // info() << "owned index: " << i << " local id: " << idof.localId();
      for (CsrRowColumnIndex csr_index : csr_view.rowRange(i)) {
        Int32 column_index = csr_view.column(csr_index);
        // info() << "column index: " << column_index << " allUIndex: " << allUIndex[idof.localId()] << " csr index: " << csr_index;
        profiler.addMatrixEntry(allUIndex[idof.localId()], allUIndex[column_index]);
      }
    }
  }
  {
    Alien::ProfiledMatrixBuilder builder(
    matrixA, Alien::ProfiledMatrixOptions::eResetValues);

    ENUMERATE_DOF(idof, dof_family->allItems()) {
      if (!idof->isOwn()) {
        continue;
      }

      int i = idof.index();
      for (CsrRowColumnIndex csr_index : csr_view.rowRange(i)) {
        Int32 column_index = csr_view.column(csr_index);
        // info() << "row: " << i << " col: " << column_index << " val: " << csr_view.value(csr_index);
        builder(allUIndex[idof.localId()], allUIndex[column_index]) += csr_view.value(csr_index);
      }
    }
    builder.finalize();
  }
  */

  Real a2 = platform::getRealTime();
  info() << "[Alien-Timer] Time to create matrix = " << (a2 - a1);

  auto& matrixA = m_alien_bsr_format->getMatrixA();
  auto& vectorX = m_alien_bsr_format->getVectorX();
  auto& vectorB = m_alien_bsr_format->getVectorB();

#ifdef ALIEN_USE_SYCL
  if(m_use_accelerator)
    _setVectorsAcc(vectorX,vectorB) ;
  else
#endif
    _setVectorsCpu(vectorX,vectorB) ;

  Real a3 = platform::getRealTime();
  info() << "[Alien-Timer] Time to create vectors = " << (a3 - a2);

#ifdef PRINT_DEBUG
#ifdef ALIEN_USE_SYCL
  if(m_use_accelerator)
    _printMatrixAcc(matrixA) ;
  else
#endif
    _printMatrixCpu(matrixA) ;
#endif


  m_solver_backend->solve(matrixA, vectorB, vectorX);

  Real a4 = platform::getRealTime();
  info() << "[Alien-Timer] Time to solve = " << (a4 - a3);

  Alien::SolverStatus status = m_solver_backend->getStatus();

  if (status.succeeded)
  {
    info() << "[Alien-Info] " << "Converged in " << status.iteration_count + 1 << " iterations";
#ifdef ALIEN_USE_SYCL
    if(options->useAccelerator())
    {
      _getSolutionAcc(vectorX) ;
    }
    else
#endif
    {
      _getSolutionCpu(vectorX) ;
    }
  }
  else
    info()<<"SOLVER FAILED";
  m_solver_backend->getSolverStat().print(Universe().traceMng(), status, "Linear Solver : ");
}


void AlienDoFLinearSystemImpl::
_applyForcedValuesToLhsCpu()
{
  IItemFamily* dof_family = dofFamily();
  VariableDoFReal& rhs_variable = this->rhsVariable();
  VariableDoFReal& dof_variable = this->solutionVariable();

  VariableDoFBool& is_forced     = this->getForcedInfo() ;
  VariableDoFReal& forced_value  = this->getForcedValue() ;

  auto& matrixA = m_alien_bsr_format->getMatrixA();
  {
    Alien::ProfiledMatrixBuilder builder(matrixA, Alien::ProfiledMatrixOptions::eKeepValues);
    ENUMERATE_DOF(idof, dof_family->allItems().own())
    {
      if(is_forced[*idof])
      {
        const Integer iIndex = m_allUIndex[idof->localId()];
        builder(iIndex,iIndex) = forced_value[*idof] ;
      }
    }
  }
}

void AlienDoFLinearSystemImpl::
applyMatrixTransformation()
{
  // Matrix transformation
  //_fillRowColumnEliminationInfos();
  //_applyRowOrRowColumnEliminationOnMatrix();
  if(m_use_accelerator)
    _applyForcedValuesToLhsAcc();
  else
    _applyForcedValuesToLhsCpu();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void AlienDoFLinearSystemImpl::
applyRHSTransformation()
{
  // RHS transformation
  //_applyRowColumnEliminationToRHS(false);
  //_applyRowOrRowColumnEliminationOnRHS();
}




class AlienDoFLinearSystemFactoryService
: public ArcaneAlienDoFLinearSystemFactoryObject
{
 public:

  explicit AlienDoFLinearSystemFactoryService(const ServiceBuildInfo& sbi)
  : ArcaneAlienDoFLinearSystemFactoryObject(sbi)
  {
    info() << "[Alien-Info] Create AlienDoF";
  };
  IDoFLinearSystemImpl*
  createInstance(ISubDomain* sd, IItemFamily* dof_family, const String& solver_name) override
  {
    auto* x = new AlienDoFLinearSystemImpl(dof_family, solver_name);
    x->options = options();

    Alien::ILinearSolver* solver_backend = options()->linearSolver.instance();
    x->setSolver(solver_backend);
    solver_backend->init();

    return x;
  }
};

ARCANE_REGISTER_SERVICE_ALIENDOFLINEARSYSTEMFACTORY(AlienLinearSystem,
                                                    AlienDoFLinearSystemFactoryService);

} // namespace Arcane::FemUtils
