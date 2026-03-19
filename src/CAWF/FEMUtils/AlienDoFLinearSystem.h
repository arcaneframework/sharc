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
#pragma once

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

#include <alien/arcane_tools/accessors/ItemVectorAccessor.h>
#include <alien/core/block/VBlock.h>

#include <alien/arcane_tools/IIndexManager.h>
#include <alien/arcane_tools/indexManager/BasicIndexManager.h>
#include <alien/arcane_tools/indexManager/SimpleAbstractFamily.h>
#include <alien/arcane_tools/distribution/DistributionFabric.h>
#include <alien/arcane_tools/indexSet/IndexSetFabric.h>
#include <alien/arcane_tools/data/Space.h>

#include <alien/kernels/simple_csr/algebra/SimpleCSRLinearAlgebra.h>
#include <alien/kernels/simple_csr/algebra/SimpleCSRInternalLinearAlgebra.h>

#include <alien/ref/AlienRefSemantic.h>
#include <alien/ref/AlienImportExport.h>

#include <alien/kernels/redistributor/Redistributor.h>
#include <alien/ref/data/scalar/RedistributedVector.h>
#include <alien/ref/data/scalar/RedistributedMatrix.h>
#include <alien/ref/import_export/MatrixMarketSystemWriter.h>

#include <alien/expression/solver/SolverStater.h>
#include <alien/AlienLegacyConfig.h>

#include <AlienDoFLinearSystemFactory_axl.h>

#ifdef ALIEN_USE_PETSC
#include <alien/kernels/petsc/io/AsciiDumper.h>
#include <alien/kernels/petsc/algebra/PETScLinearAlgebra.h>
#endif

#ifdef ALIEN_USE_HYPRE
#include <alien/kernels/hypre/HypreBackEnd.h>
#include <alien/kernels/hypre/data_structure/HypreMatrix.h>
#include <alien/kernels/hypre/data_structure/HypreVector.h>
#include <alien/kernels/hypre/algebra/HypreLinearAlgebra.h>
#endif

#include "CAWF/FEMUtils/internal/CsrDoFLinearSystemImpl.h"
#include "CAWF/FEMUtils/AlienBSRFormat.h"

namespace Arcane::FemUtils
{

using namespace Arcane;
using namespace Alien;

class AlienDoFLinearSystemImpl
//: public CsrDoFLinearSystemImpl
: public DoFLinearSystemImplBase
{
 public:

  AlienDoFLinearSystemImpl(IItemFamily* dof_family, const String& solver_name) ;

  ~AlienDoFLinearSystemImpl() override ;

 public:

  AlienBSRFormat* bsr(ITraceMng* trace_mng, RunQueue& queue, FemDoFsOnNodes& dofs_on_nodes)
  {
    if(not m_alien_bsr_format.get())
      m_alien_bsr_format.reset( new AlienBSRFormat{trace_mng,queue,dofs_on_nodes,options->useAccelerator()}) ;
    return m_alien_bsr_format.get();
  }

  Int32 indexValue(DoFLocalId row_lid, DoFLocalId column_lid)
  {
    /*
    auto begin = m_csr_view.rows()[row_lid];
    auto end = row_lid == m_csr_view.nbRow() - 1 ? m_csr_view.nbColumn() : m_csr_view.row(row_lid + 1);
    for (auto i = begin; i < end; ++i)
      if (m_csr_view.columns()[i] == column_lid)
        return i;
    */
    return -1;
  }

 public:

  void matrixAddValue(DoFLocalId row, DoFLocalId column, Real value) override
  {
    //m_csr_view.values()[indexValue(row, column)] += value;
    (*m_builder)(m_allUIndex[row], m_allUIndex[column]) += value;
  }

  void matrixSetValue(DoFLocalId row, DoFLocalId column, Real value) override
  {
    //m_csr_view.values()[indexValue(row, column)] = value;
    (*m_builder)(m_allUIndex[row], m_allUIndex[column]) = value;

  }

  CSRFormatView& getCSRValues() override
  {
    //return m_csr_view;
    ARCANE_THROW(NotImplementedException, "");
  };

  void setCSRValues(const CSRFormatView& csr_view) override
  {
    //m_csr_view = csr_view;
    ARCANE_THROW(NotImplementedException, "");
  }

  bool hasSetCSRValues() const override { return false; }

  void eliminateRow(DoFLocalId row, Real value) override
  {
    ARCANE_THROW(NotImplementedException, "");
  }

  void eliminateRowColumn(DoFLocalId row, Real value) override
  {
    ARCANE_THROW(NotImplementedException, "");
  }

  void applyMatrixTransformation() override;
  void applyRHSTransformation() override;

  void clearValues() override
  {
    info() << "[CsrImpl]: Clear values";
    DoFLinearSystemImplBase::clearValues();
    //m_csr_view = {};
    m_alien_bsr_format.reset() ;
    m_use_accelerator = options->useAccelerator() ;
  }

  void startSystemAssembly() {
    m_allUIndex = m_alien_bsr_format->getAllUIndex() ;
    m_alien_bsr_format->startSystemFillingStep() ;
    m_builder = m_alien_bsr_format->getMatrixBuilder() ;
  }

  void endSystemAssembly() {
    m_alien_bsr_format->endSystemFillingStep() ;
#ifdef PRINT_DEBUG
    auto& matrixA = m_alien_bsr_format->getMatrixA();
#ifdef ALIEN_USE_SYCL
    if(m_use_accelerator)
      _printMatrixAcc(matrixA) ;
    else
#endif
      _printMatrixCpu(matrixA) ;
#endif
  }

  void solve() override;

  void setSolverCommandLineArguments(const CommandLineArguments& args) override
  {
    info() << "[Alien-Info] initialize command lines arguments";
    auto argv = *args.commandLineArgv();
    auto o = info() << "[Alien-Info] ./" << argv[0];


    for (int i = 1; i < *args.commandLineArgc(); i++)
      o << ' ' << argv[i];
  }

  void setSolver(Alien::ILinearSolver* s) { m_solver_backend = s; }

  CaseOptionsAlienDoFLinearSystemFactory* options = nullptr;

 private:
  void _applyForcedValuesToLhsCpu();
  void _setVectorsCpu(Alien::Vector& vectorX, Alien::Vector& vectorB) ;
  void _getSolutionCpu(Alien::Vector& vectorX) ;
  void _printMatrixCpu(Alien::Matrix& matrix) ;
#ifdef ALIEN_USE_SYCL
  void _applyForcedValuesToLhsAcc();
  void _setVectorsAcc(Alien::Vector& vectorX, Alien::Vector& vectorB) ;
  void _getSolutionAcc(Alien::Vector& vectorX) ;
  void _printMatrixAcc(Alien::Matrix& matrix) ;
#endif
  bool                             m_use_accelerator  = false ;
  Alien::ILinearSolver*            m_solver_backend   = nullptr;
  Alien::ProfiledMatrixBuilder*    m_builder          = nullptr;
  std::unique_ptr<AlienBSRFormat>  m_alien_bsr_format;
  ConstArrayView<Arccore::Integer> m_allUIndex;

};

} // namespace Arcane::FemUtils
