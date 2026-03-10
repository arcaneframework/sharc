// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* FemModule.h                                                (C) 2022-2025  */
/*                                                                           */
/* FemModuleElasticity class definition.                                     */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#ifndef FEMMODULES_H
#define FEMMODULES_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/utils/CommandLineArguments.h>
#include <arcane/utils/ParameterList.h>
#include <arcane/utils/ApplicationInfo.h>

#include <arcane/ITimeLoopMng.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/ItemGroup.h>
#include <arcane/ICaseMng.h>
#include <arcane/accelerator/core/IAcceleratorMng.h>
#include <arcane/accelerator/core/RunQueue.h>
#include <arcane/core/ItemTypes.h>
#include <arccore/base/ArccoreGlobal.h>
#include "arccore/base/NotImplementedException.h"

#include "CAWF/PreCICE/FEMUtils/IArcaneFemBC.h"
#include "CAWF/PreCICE/FEMUtils/IDoFLinearSystemFactory.h"
#include "CAWF/PreCICE/DynamicMeshMng/IDynamicMeshMng.h"

#include "Fem_axl.h"

#include "CAWF/PreCICE/FEMUtils/FemUtils.h"
#include "CAWF/PreCICE/FEMUtils/DoFLinearSystem.h"
#include "CAWF/PreCICE/FEMUtils/FemDoFsOnNodes.h"
#include "CAWF/PreCICE/FEMUtils/BSRFormat.h"

#include "CAWF/PreCICE/FEMUtils/ArcaneFemFunctions.h"
#include "CAWF/PreCICE/FEMUtils/ArcaneFemFunctionsGpu.h"


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
using namespace Arcane::FemUtils;

/*---------------------------------------------------------------------------*/
/**
 * @brief A module for finite element method.
 *
 * This class handles the initialization and computation for finite element
 * method (FEM) simulations, providing methods to  set  up and solve linear
 * systems, assemble FEM operators, and perform result checks.
 */
/*---------------------------------------------------------------------------*/

class FemModuleElasticity
: public ArcaneFemObject
, public ArcGeoSim::ICAWFMng::ITimeIterStateOp
{
 public:

  explicit FemModuleElasticity(const ModuleBuildInfo& mbi)
  : ArcaneFemObject(mbi)
  , m_dofs_on_nodes(mbi.subDomain()->traceMng())
  , m_bsr_format(mbi.subDomain()->traceMng(), *(mbi.subDomain()->acceleratorMng()->defaultQueue()), m_dofs_on_nodes)
  {
    ICaseMng* cm = mbi.subDomain()->caseMng();
    cm->setTreatWarningAsError(true);
    cm->setAllowUnkownRootElelement(false);
  }

  void saveOldState();
  void reloadOldState();

  void startInit() override; //! Method called at the beginning of the simulation
  void compute() override; //! Method called at each iteration
  VersionInfo versionInfo() const override { return VersionInfo(1, 0, 0); }

  void _doStationarySolve();
  void _assembleBilinearOperator();
  void _assembleDirichletsGpu();

  void _computePrePro();
  void _computePostPro();

 private:

  DoFLinearSystem m_linear_system;
  FemDoFsOnNodes m_dofs_on_nodes;
  BSRFormat m_bsr_format;

  Real E;
  Real nu;
  //Real mu;
  //Real lambda;

  //Real3 f;
  Real3 t;
  bool m_applyBodyForce = false;

  Int8 m_dof_per_node;

  String m_petsc_flags;
  String m_matrix_format = "DOK";

  bool m_assemble_linear_system = true;
  bool m_solve_linear_system = true;
  bool m_cross_validation = false;
  bool m_hex_quad_mesh = false;

  Integer                        m_max_iter         = 0 ;

  ShArc::IDynamicMeshMng*        m_dynamic_mesh_mng = nullptr;
  ArcGeoSim::ICAWFMng*           m_cawf_mng         = nullptr ;
  IGeometryMng *                 m_geometry_mng     = nullptr;
  NoOptimizationGeometryPolicy   m_geometry_policy ;

  void _getMaterialParameters();
  void _solve();
  void _assembleLinearOperator();
  void _validateResults();
  void _updateVariables();
  void _initBsr();

  inline void _applyBodyForce(VariableDoFReal& rhs_values, const IndexedNodeDoFConnectivityView& node_dof);
  inline void _applyTraction(VariableDoFReal& rhs_values, const IndexedNodeDoFConnectivityView& node_dof);
  inline void _applyDirichlet(VariableDoFReal& rhs_values, const IndexedNodeDoFConnectivityView& node_dof);

  RealMatrix<6, 6> _computeElementMatrixTria3(Cell cell);
  RealMatrix<12, 12> _computeElementMatrixTetra4(Cell cell);
  RealMatrix<8, 8> _computeElementMatrixQuad4(Cell cell);
  RealMatrix<24, 24> _computeElementMatrixHexa8(Cell cell);
  Real3 _computeElementGradientHexa8(Cell cell);
  Real _computeElementDiv(Cell cell);

  template <int N>
  void _assembleBilinearOperatorCpu(const std::function<RealMatrix<N, N>(const Cell&)>& compute_element_matrix);
};

#endif
