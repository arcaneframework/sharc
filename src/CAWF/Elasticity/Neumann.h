// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* Neumann.h                                                 (C) 2022-2025 */
/*                                                                           */
/* Contains functions to compute and assemble Neumann contribution to RHS  */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/**
 * @brief Applies Neumann to LHS matrix and RHS vector of the linear system.
 *
 * This function applies Neumann boundary conditions to both the LHS matrix
 * and RHS vector of the linear system.
 *
 * @param rhs_values The variable representing the RHS vector to be updated.
 * @param node_dof The connectivity view mapping nodes to their corresponding
 */
/*---------------------------------------------------------------------------*/

inline void FemModuleElasticity::
_applyNeumann(VariableDoFReal& rhs_values, const IndexedNodeDoFConnectivityView& node_dof)
{
  // check if Hypre|PETSc solver is used and delegate to GPU for Neumann assembly
  auto use_gpu = options()->linearSystem.serviceName() == "HypreLinearSystem" ||
                 options()->linearSystem.serviceName() == "PETScLinearSystem" ||
                 m_matrix_format == "Alien-BSR-Acc";
  if (use_gpu) {
    _assembleNeumannsGpu(rhs_values);
    return;
  }

  info() << "[ArcaneFem-Info] Started module _assembleLinearOperatorCpu()";

  BC::IArcaneFemBC* bc = options()->boundaryConditions();
  if (bc) {
    if(m_hex_quad_mesh)
    {
      for (BC::INeumannBoundaryCondition* bs : bc->neumannBoundaryConditions())
        ArcaneFemFunctions::BoundaryConditions3D::applyNeumannToRhsHexa8(bs, node_dof, m_node_coord, rhs_values);
    }
    else
    {
      for (BC::INeumannBoundaryCondition* bs : bc->neumannBoundaryConditions())
        ArcaneFemFunctions::BoundaryConditions3D::applyNeumannToRhs(bs, node_dof, m_node_coord, rhs_values);
    }

  }
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Applies Neumann to LHS matrix and RHS vector of the linear system on Gpu.
 *
 * This function applies Neumann boundary conditions to both the LHS matrix
 * and RHS vector of the linear system using GPU acceleration.
 *
 * @param rhs_values The variable representing the RHS vector to be updated.
 * @param node_dof The connectivity view mapping nodes to their corresponding
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::_assembleNeumannsGpu(VariableDoFReal& rhs_values)
{
  info() << "[ArcaneFem-Info] Started module  _assembleLinearOperatorGpu()";

  auto queue = subDomain()->acceleratorMng()->defaultQueue();
  auto mesh_ptr = mesh();

  BC::IArcaneFemBC* bc = options()->boundaryConditions();

  if (bc) {
    for (BC::INeumannBoundaryCondition* bs : bc->neumannBoundaryConditions())
      FemUtils::Gpu::BoundaryConditions3D::applyNeumannToRhs(bs, m_dofs_on_nodes, m_node_coord, rhs_values, mesh_ptr, queue);
  }
}
