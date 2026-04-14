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

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Utils/CAWF/ICAWFMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#include "FemModule.h"
#include "ElementMatrix.h"
#include "ElementMatrixHexQuad.h"
#include "BodyForce.h"
#include "Traction.h"
#include "Dirichlet.h"
#include "Neumann.h"

/*---------------------------------------------------------------------------*/
/**
 * @brief Initializes the FemModuleElasticity at the start of the simulation.
 *
 * This method initializes degrees of freedom (DoFs) on nodes.
 */
/*---------------------------------------------------------------------------*/
void FemModuleElasticity::init()
{

  Alien::setTraceMng(traceMng());
  Alien::setVerbosityLevel(Alien::Verbosity::Debug);
  m_parallel_mng = subDomain()->parallelMng();


  m_dynamic_mesh_mng = nullptr;
  if(options()->dynamicMeshMng.isPresent())
  {
    m_dynamic_mesh_mng = options()->dynamicMeshMng();
    m_dynamic_mesh_mng->init();
  }

  m_cawf_mng = nullptr ;
  if(options()->cawfMng.isPresent())
    m_cawf_mng = options()->cawfMng() ;
  if(m_cawf_mng)
  {
    m_cawf_mng->init() ;
    m_cawf_mng->setTimeIterStateOp(this) ;
    m_cawf_mng->initMesh() ;
  }
  else
  {
    m_max_iter = options()->maxIter() ;
  }

  /*
  m_linear_solver = options()->linearSolver();
  m_linear_solver->init() ;

  if (not m_linear_solver->hasParallelSupport() and m_parallel_mng->commSize() > 1)
  {
    fatal() << "Current solver has not a parallel support for solving linear system : skip it";
  }*/

  m_geometry_mng = options()->geometryMng() ;
  m_geometry_mng -> addItemGroupProperty(mesh() -> allCells(),
                                   IGeometryProperty::PCenter,
                                   IGeometryProperty::PVariable) ;

  m_geometry_mng -> addItemGroupProperty(mesh() -> allCells(),
                                   IGeometryProperty::PMeasure,
                                   IGeometryProperty::PVariable) ;

  m_geometry_mng -> addItemGroupProperty(mesh() -> allFaces(),
                                   IGeometryProperty::PNormal,
                                   IGeometryProperty::PVariable) ;

  m_geometry_mng -> addItemGroupProperty(mesh() -> allFaces(),
                                   IGeometryProperty::PMeasure,
                                   IGeometryProperty::PVariable) ;

  m_geometry_mng -> addItemGroupProperty(mesh() -> allFaces(),
                                   IGeometryProperty::PCenter,
                                   IGeometryProperty::PVariable) ;

  m_geometry_mng -> setPolicyTolerance(true) ;
  m_geometry_mng -> update(&m_geometry_policy) ;

  m_event_index = 0 ;

  if(options()->top.isPresent())
    m_top_boundary_name = options()->top() ;
  if(options()->bottom.isPresent())
    m_bottom_boundary_name = options()->bottom() ;
  if(options()->front.isPresent())
    m_front_boundary_name = options()->front() ;
  if(options()->back.isPresent())
    m_back_boundary_name = options()->back() ;
  if(options()->left.isPresent())
    m_left_boundary_name = options()->left() ;
  if(options()->right.isPresent())
    m_right_boundary_name = options()->right() ;
  if(options()->border.isPresent())
    m_border_boundary_name = options()->border() ;

  _updateTopBoundary(mesh()->allCells());

  _computeBoundaryFaceNormalType(mesh()->allCells());

  {
    FaceGroup boundary = mesh()->allCells().outerFaceGroup();

    const IGeometryMng::Real3Variable & face_normal =
        m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PNormal);

    const IGeometryMng::RealVariable & face_measure =
      m_geometry_mng->getRealVariableProperty(mesh()->allFaces(),IGeometryProperty::PMeasure);
    m_face_is_nz.fill(0) ;
    ENUMERATE_FACE(iface,boundary)
    {
      Real3 normal = face_normal[*iface]/face_measure[*iface] ;
      Integer sgn = iface->isSubDomainBoundaryOutside() ? 1 : -1 ;
      if(normal.z> 0.75)
        m_face_is_nz[iface] = sgn ;
      if(normal.z< -0.75)
        m_face_is_nz[iface] = -sgn ;
    }
  }


  m_new_layer_id = 0 ;
  ENUMERATE_CELL(icell,allCells())
  {
    m_layer_id[*icell] = m_new_layer_id ;
  }

  m_time = 0. ;
  m_dt = 1. ;

  m_event_period = options()->eventPeriod() ;
  m_next_event_time = m_time + m_event_period ;
  if(m_cawf_mng)
  {
    m_cawf_mng->start() ;
    m_dt = m_cawf_mng->initialTimeStep() ;
  }
  m_global_deltat = m_dt ;

  startInit();
}

void FemModuleElasticity::
startInit()
{
  info() << "[ArcaneFem-Info] Started module  startInit()";
  Real elapsedTime = platform::getRealTime();

  m_dof_per_node = defaultMesh()->dimension();
  m_matrix_format = options()->matrixFormat();
  m_assemble_linear_system = options()->assembleLinearSystem();
  m_solve_linear_system = options()->solveLinearSystem();
  m_cross_validation = options()->hasSolutionComparisonFile();
  m_petsc_flags = options()->petscFlags();
  m_hex_quad_mesh = options()->hexQuadMesh();

  _getMaterialParameters(mesh()->allCells());

  const UniqueArray<String> f_string = options()->f();
  info() << "[ArcaneFem-Info] Applying Bodyforce " << f_string;
  for (Int32 i = 0; i < f_string.size(); ++i)
  {
    m_F[i] = 0.0;
    if (f_string[i] != "NULL") {
      m_applyBodyForce = true;
      m_F[i] = std::stod(f_string[i].localstr());
    }
  }
  if(m_applyBodyForce)
  {
    const IGeometryMng::Real3Variable & cell_center =
        m_geometry_mng->getReal3VariableProperty(mesh()->allCells(),IGeometryProperty::PCenter);
    ENUMERATE_CELL(icell,mesh()->allCells())
    {
      m_cell_pressure[icell] = m_F[2]*(m_top_z -cell_center[icell].z);
    }
  }

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"initialize", elapsedTime);
}

void FemModuleElasticity::
_updateTopBoundary(CellGroup const& new_event)
{
  FaceGroup top_boundary = mesh()->faceFamily()->findGroup(options()->top()) ;
  if(top_boundary.empty())
  {
    info()<<"UPDATE TOP BOUNDARY FACE GROUP";
      const IGeometryMng::RealVariable & face_measure =
          m_geometry_mng->getRealVariableProperty(mesh()->allFaces(),IGeometryProperty::PMeasure);

      const IGeometryMng::Real3Variable & face_normal =
          m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PNormal);

      const IGeometryMng::Real3Variable & face_center =
          m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PCenter);

      UniqueArray<Integer> top_face_lids;
      ENUMERATE_CELL(icell,new_event)
      {
        ENUMERATE_FACE(iface,icell->faces())
        {
          if(iface->isSubDomainBoundary())
          {
            Real3 normal = face_normal[*iface]/face_measure[*iface] ;
            if(std::abs(normal.z)> 0.75)
              top_face_lids.add(iface->localId());
          }
        }
      }
      mesh()->faceFamily()->createGroup(options()->top(),top_face_lids,true) ;
  }

  FaceGroup const& top_face_boundary = mesh()->faceFamily()->findGroup(options()->top());
  {
    const IGeometryMng::Real3Variable & face_center =
        m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PCenter);
    Real top_z = 0. ;
    ENUMERATE_FACE(iface,top_boundary)
    {
      top_z += face_center[*iface].z ;
      info()<<"TOP FACE : "<<iface->uniqueId()<<" BOUNDARY CELL : "<<iface->boundaryCell().uniqueId()<<" H = "<<face_center[*iface].z ;
    }
    top_z /= top_boundary.size() ;
    m_top_z = top_z;
    info()<<"TOP BOUDARY AVG HEIGHT : "<<m_top_z;
  }
  info()<<"TOP BOUNDARY SIZE["<<m_event_index<<"] : "<<top_face_boundary.size() ;
  m_face_is_top.fill(0) ;
  ENUMERATE_FACE(iface,top_face_boundary)
  {
    m_face_is_top[iface] = 1 ;
  }
}


void FemModuleElasticity::
_computeBoundaryFaceNormalType(CellGroup const& new_event)
{
  const IGeometryMng::Real3Variable & face_normal =
      m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PNormal);

  ENUMERATE_FACE(iface,new_event.faceGroup())
  {
    m_face_normal_type[iface] = -1;
    if(iface->isSubDomainBoundary())
    {
      Real3 fnormal = face_normal[iface];
      Real max_normal_comp = 0. ;
      Integer index = -1 ;
      for(Integer i=0;i<3;++i)
      {
        if(std::abs(fnormal[i]) > max_normal_comp)
        {
          index = i;
          max_normal_comp = std::abs(fnormal[i]);
        }
      }
      m_face_normal_type[iface] = index ;
    }
  }
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Performs the main computation for the FemModuleElasticity.
 *
 * This method:
 *   1. Stops the time loop after 1 iteration since the equation is steady state.
 *   2. Resets, configures, and initializes the linear system.
 *   3. Sets PETSc flags if user has provided them.
 *   4. Executes the stationary solve.
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
computeDivU()
{
  info() << "[ArcaneFem-Info] Started module  computeDivU()";
  Real elapsedTime = platform::getRealTime();

  // Stop code after computations
  //if (m_global_iteration() > 0)
  //  subDomain()->timeLoopMng()->stopComputeLoop(true);

  m_dofs_on_nodes.initialize(defaultMesh(), m_dof_per_node);

  m_linear_system.reset();
  m_linear_system.setLinearSystemFactory(options()->linearSystem());
  m_linear_system.initialize(subDomain(), acceleratorMng()->defaultRunner(), m_dofs_on_nodes.dofFamily(), "Solver");
  m_linear_system.clearValues();

  if (m_petsc_flags != NULL){
    CommandLineArguments args = ArcaneFemFunctions::GeneralFunctions::getPetscFlagsFromCommandline(m_petsc_flags);
    m_linear_system.setSolverCommandLineArguments(args);
  }

  if (m_matrix_format == "BSR" || m_matrix_format == "AF-BSR")
    _initBsr();
  else if(m_matrix_format == "Alien-BSR" || m_matrix_format == "Alien-BSR-Acc")
    _initAlienBsr() ;

  Int64 nb_node = mesh()->ownNodes().size();
  Int64 total_nb_node = mesh()->parallelMng()->reduce(Parallel::ReduceSum, nb_node);

  Int64 nb_face = mesh()->outerFaces().size();
  Int64 total_nb_boundary_elt = mesh()->parallelMng()->reduce(Parallel::ReduceSum, nb_face);

  Int64 nb_cell = mesh()->ownCells().size();
  Int64 total_nb_elt = mesh()->parallelMng()->reduce(Parallel::ReduceSum, nb_cell);

  info() << "[ArcaneFem-Info] mesh dimension " << defaultMesh()->dimension();
  info() << "[ArcaneFem-Info] mesh boundary elements " << total_nb_boundary_elt;
  info() << "[ArcaneFem-Info] mesh cells " << total_nb_elt;
  info() << "[ArcaneFem-Info] mesh nodes " << total_nb_node;

  _computePrePro();
  _doStationarySolve();
  _computePostPro();

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"compute", elapsedTime);
}


void
FemModuleElasticity::
test()
{

  info()<<"FEM Elasticity MODULE COMPUTATION";

  ///////////////////////////////////////////////////////////////////////////
  //
  // RESOLUTION
  //
  if(m_cawf_mng)
  {
    if(m_cawf_mng->isCouplingOngoing())
    {
      info()<<"ITERATION ["<<m_iter<<"] TIME = "<<m_time<<" DT="<<m_dt ;


      bool time_iter_is_valid = false ;
      int sub_iter = 0 ;
      while(not time_iter_is_valid)
      {
          info()<<"TIME STEP NUB ITERATION : "<<sub_iter;
          // precice.readVectorData
          m_cawf_mng->startTimeStep() ;

#ifdef USE_PRECICE_V3
          if(not m_cawf_mng->isMeshUpdate())
          {
            m_cawf_mng->updateMesh() ;
          }
#endif
          computeDivU();

          //precice.writeVectorData
          m_cawf_mng->endTimeStep() ;

          // precice.advance coupling
          Real precice_dt = m_cawf_mng->newTimeStep(m_dt) ;

          // precice.readIterationCheck point reloadOldState
          // time step non converged
          if( m_cawf_mng->validateCurrentTimeStep())
          {
              //m_dt = new_dt ;
            m_dt = math::min(m_dt,precice_dt );
            ++m_iter ;
            m_time += m_dt ;
            m_global_deltat = m_dt ;
            time_iter_is_valid = true ;
            info() << " VALIDATESTEP " ;
            if(m_dynamic_mesh_mng)
            {
              if((m_event_period>0) && (m_time >= m_next_event_time))
              {
                m_dynamic_mesh_mng->updateNewEvent() ;
                m_geometry_mng -> update(&m_geometry_policy) ;

                CellGroup new_event = mesh()->cellFamily()->findGroup("NewEventLayer") ;

                info()<<" NEW EVENT : "<<m_event_index<<" "<<new_event.size();
                if(not new_event.empty())
                {
                  info()<<" NEW EVENT LAYER : "<<m_new_layer_id;
                  ++ m_new_layer_id ;

#ifdef USE_PRECICE_V3
                  m_cawf_mng->invalidateMesh(true) ;
#endif
                  ENUMERATE_CELL(icell,new_event)
                  {
                    m_cell_div_u[icell] = 0. ;
                    m_layer_id[icell] = m_new_layer_id ;
                  }

                  _getMaterialParameters(new_event);

                  if(m_applyBodyForce)
                  {
                    const IGeometryMng::Real3Variable & cell_center =
                        m_geometry_mng->getReal3VariableProperty(mesh()->allCells(),IGeometryProperty::PCenter);
                    ENUMERATE_CELL(icell,new_event)
                    {
                      m_cell_pressure[icell] = m_F[2]*(m_top_z-cell_center[icell].z);
                    }
                  }

                  _updateTopBoundary(new_event) ;
                  _computeBoundaryFaceNormalType(new_event);


                }
                ++ m_event_index ;
                m_next_event_time += m_event_period ;
              }
            }
          }
          else
          {
            ++sub_iter;
            m_global_deltat = 0. ;
            info() << " ITERATE COUPLING " ;
            info() << " Global Time " << m_global_time() << " Iter " << m_global_iteration();
          }

        } //while( m_cawf_mng->isCouplingOngoing());
    }
    else
    {
      info()<<"FINAL TIME : "<<m_time ;
      m_cawf_mng->finalize() ;
      info()<<"STOP PERFECT";
      subDomain()->timeLoopMng()->stopComputeLoop(true);
    }
  }
  else
  {
    info()<<"ITERATION ["<<m_iter<<"] TIME = "<<m_time<<" DT="<<m_dt ;

    {
      computeDivU() ;
      ++m_iter ;
      m_time += m_dt ;
      m_global_deltat = m_dt ;
      if(m_dynamic_mesh_mng)
      {
        if((m_event_period>0) && (m_time >= m_next_event_time))
        {
          m_dynamic_mesh_mng->updateNewEvent() ;
          m_geometry_mng -> update(&m_geometry_policy) ;

          CellGroup new_event = mesh()->cellFamily()->findGroup("NewEventLayer") ;

          info()<<" NEW EVENT : "<<m_event_index<<" "<<new_event.size();
          if(not new_event.empty())
          {
            info()<<" NEW EVENT LAYER : "<<m_new_layer_id;
            ++ m_new_layer_id ;
            ENUMERATE_CELL(icell,new_event)
            {
              m_layer_id[*icell] = m_new_layer_id ;
            }

            _getMaterialParameters(new_event);

            if(m_applyBodyForce)
            {
              const IGeometryMng::Real3Variable & cell_center =
                  m_geometry_mng->getReal3VariableProperty(mesh()->allCells(),IGeometryProperty::PCenter);
              ENUMERATE_CELL(icell,new_event)
              {
                m_cell_pressure[icell] = m_F[2]*(m_top_z-cell_center[icell].z);
              }
            }

            _updateTopBoundary(new_event) ;
            _computeBoundaryFaceNormalType(new_event);

          }
          ++ m_event_index ;
          m_next_event_time += m_event_period ;
        }
      }
    }
    if(m_iter>=m_max_iter)
    {
      info()<<"FINAL TIME : "<<m_time ;
      info()<<"STOP PERFECT";
      subDomain()->timeLoopMng()->stopComputeLoop(true);
    }
  }
}

void
FemModuleElasticity::saveOldState()
{
  info() << " App1 saveOldState() " ;
  ENUMERATE_CELL(icell,allCells())
  {
    //m_cell_pressure_N[icell] = m_cell_pressure[icell] ;
  }
}

void FemModuleElasticity::reloadOldState()
{
  info() << " App1  ReloadOldState " ;
  ENUMERATE_CELL(icell,allCells())
  {
    //m_cell_pressure[icell] = m_cell_pressure_N[icell] ;
  }
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Initializes BSR matrix.
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::_initBsr()
{
  info() << "[ArcaneFem-Info] Started module  _initBsr()";
  Real elapsedTime = platform::getRealTime();

  bool use_csr_in_linearsystem =
  options()->linearSystem.serviceName() == "HypreLinearSystem" ||
  options()->linearSystem.serviceName() == "AlienLinearSystem" ||
  options()->linearSystem.serviceName() == "PETScLinearSystem";

  if (m_matrix_format == "BSR")
    m_bsr_format.initialize(defaultMesh(), m_dof_per_node, use_csr_in_linearsystem, 0);
  else
    m_bsr_format.initialize(defaultMesh(), m_dof_per_node, use_csr_in_linearsystem, 1);

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"initialize-bsr-matrix", elapsedTime);
}

void FemModuleElasticity::_initAlienBsr()
{
  info() << "[ArcaneFem-Info] Started module  _initAlienCsr()";
  Real elapsedTime = platform::getRealTime();
  m_alien_bsr_format = m_linear_system.alienBsr(traceMng(),
                           *(acceleratorMng()->defaultQueue()),
                           m_dofs_on_nodes) ;
  m_alien_bsr_format->initialize(defaultMesh(),m_dof_per_node) ;

  //m_alien_bsr_format.initialize(defaultMesh(), m_dof_per_node, use_csr_in_linearsystem, 0);

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"initialize-alien-csr-matrix", elapsedTime);
}


void FemModuleElasticity::
_computePrePro()
{
  const IGeometryMng::RealVariable & cell_measure =
    m_geometry_mng->getRealVariableProperty(mesh()->allCells(),IGeometryProperty::PMeasure);

  // Compute BodyForce : Grad(Pressure)
  if (mesh()->dimension() == 3)
  {
    if (m_hex_quad_mesh)
    {
      ENUMERATE_NODE(inode,allNodes())
      {
        Real pressure = 0. ;
        Real volume = 0. ;
        ENUMERATE_CELL(icell,inode->cells())
        {
          Real cell_vol = cell_measure[icell]/icell->nbNode() ;
          volume += cell_vol ;
          pressure += cell_vol*m_cell_pressure[icell] ;
        }
        m_node_pressure[inode] = pressure/volume ;
      }
      ENUMERATE_CELL(icell,allCells())
      {
        m_cell_body_force[icell] = - ArcaneFemFunctions::FeOperation3D::computeGradientHexa8(*icell,m_node_coord,m_node_pressure) ;
      }
    }
  }
}

void FemModuleElasticity::
_computePostPro()
{
  if (mesh()->dimension() == 3)
  {
    if (m_hex_quad_mesh)
    {
      ENUMERATE_NODE(inode,allNodes())
      {
        m_Ux[inode] = m_U[inode][0] ;
        m_Uy[inode] = m_U[inode][1] ;
        m_Uz[inode] = m_U[inode][2] ;
      }

      ENUMERATE_CELL(icell,allCells())
      {
        Real3 gradUx = ArcaneFemFunctions::FeOperation3D::computeGradientHexa8(*icell,m_node_coord,m_Ux) ;
        Real3 gradUy = ArcaneFemFunctions::FeOperation3D::computeGradientHexa8(*icell,m_node_coord,m_Uy) ;
        Real3 gradUz = ArcaneFemFunctions::FeOperation3D::computeGradientHexa8(*icell,m_node_coord,m_Uz) ;
        m_cell_div_u[icell] = gradUx.x + gradUy.y + gradUz.z ;
      }
    }
  }
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Performs a stationary solve for the FEM system.
 *
 * This method follows a sequence of steps to solve FEM system:
 *
 *   1. _getMaterialParameters()     Retrieves material parameters via
 *   2. _assembleBilinearOperator()  Assembles the FEM  matrix 𝐀
 *   3. _assembleLinearOperator()    Assembles the FEM RHS vector 𝐛
 *   4. _solve()                     Solves for solution vector 𝐮 = 𝐀⁻¹𝐛
 *   5. _updateVariables()           Updates FEM variables 𝐮 = 𝐱
 *   6. _validateResults()           Regression test
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
_doStationarySolve()
{
  if(m_assemble_linear_system)
  {
    if(m_alien_bsr_format)
    {
#ifdef ALIEN_USE_SYCL
      if(m_matrix_format == "Alien-BSR-Acc")
        _defineMatrixProfileAcc() ;
      else
#endif
      {
        auto func = [this](Alien::MatrixProfiler& profiler,ConstArrayView<Integer> allUIndex)
                   {
                    return this->_defineMatrixProfile(profiler,allUIndex) ;
                   } ;
        m_alien_bsr_format->computeProfile<Alien::MatrixProfiler>(func);
      }
    }

    m_linear_system.startSystemAssembly() ;
    _assembleBilinearOperator();
    _assembleLinearOperator();
    m_linear_system.endSystemAssembly() ;
  }
  if(m_solve_linear_system)
  {
    _solve();
    _updateVariables();
  }
  if(m_cross_validation){
    _validateResults();
  }
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Retrieves and sets the material parameters for the simulation.
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
_getMaterialParameters(CellGroup const& domain)
{
  info() << "[ArcaneFem-Info] Started module  _getMaterialParameters()";
  Real elapsedTime = platform::getRealTime();

  E = options()->E(); // Youngs modulus
  nu = options()->nu(); // Poission ratio ν

  Real mu = (E / (2 * (1 + nu))); // lame parameter μ
  Real lambda = E * nu / ((1 + nu) * (1 - 2 * nu)); // lame parameter λ

  ENUMERATE_CELL(icell,domain)
  {
    m_cell_mu[icell] = mu ;
    m_cell_lambda[icell] = lambda;
  }

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"get-material-params", elapsedTime);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/**
 * @brief Assemble the FEM linear operator.
 *
 * This method follows a sequence of steps to assemble RHS of FEM linear system:
 *
 *   1. assembles the bodyforce contribution (source term) ∫∫∫ (𝐟.𝐯) on Ω
 *   2. assembles the traction contribution (Neumann term) ∫∫ (𝐭.𝐯)  on ∂Ω
 *   3. apply Dirichlet contributions to LHS and RHS
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
_assembleLinearOperator()
{
  info() << "[ArcaneFem-Info] Started module  _assembleLinearOperator()";
  Real elapsedTime = platform::getRealTime();

  VariableDoFReal& rhs_values(m_linear_system.rhsVariable()); // Temporary variable to keep values for the RHS
  rhs_values.fill(0.0);

  auto node_dof(m_dofs_on_nodes.nodeDoFConnectivityView());

  _applyBodyForce(rhs_values, node_dof);
  _applyTraction(rhs_values, node_dof);
  _applyDirichlet(rhs_values, node_dof);
  _applyNeumann(rhs_values, node_dof);

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"rhs-vector-assembly", elapsedTime);
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Calls the right function for LHS assembly given as mesh type.
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
_assembleBilinearOperator()
{
  info() << "[ArcaneFem-Info] Started module  _assembleBilinearOperator()";
  Real elapsedTime = platform::getRealTime();

  if (m_matrix_format == "BSR")
  {
    UnstructuredMeshConnectivityView m_connectivity_view(mesh());
    auto cn_cv = m_connectivity_view.cellNode();
    auto command = makeCommand(acceleratorMng()->defaultQueue());
    auto in_node_coord = Accelerator::viewIn(command, m_node_coord);
    auto in_lambda_copy = Accelerator::viewIn(command, m_cell_lambda);
    auto in_mu_copy = Accelerator::viewIn(command, m_cell_mu);

    m_bsr_format.computeSparsity();
    if (mesh()->dimension() == 2)
      m_bsr_format.assembleBilinearAtomic([=] ARCCORE_HOST_DEVICE(CellLocalId cell_lid) { return computeElementMatrixTria3Gpu(cell_lid, cn_cv, in_node_coord, in_lambda_copy, in_mu_copy); });
    if (mesh()->dimension() == 3)
      m_bsr_format.assembleBilinearAtomic([=] ARCCORE_HOST_DEVICE(CellLocalId cell_lid) { return computeElementMatrixTetra4Gpu(cell_lid, cn_cv, in_node_coord, in_lambda_copy, in_mu_copy); });
    m_bsr_format.toLinearSystem(m_linear_system);
  }
  else if (m_matrix_format == "AF-BSR")
  {
    UnstructuredMeshConnectivityView m_connectivity_view(mesh());
    auto cn_cv = m_connectivity_view.cellNode();
    auto command = makeCommand(acceleratorMng()->defaultQueue());
    auto in_node_coord = Accelerator::viewIn(command, m_node_coord);
    auto lambda_copy = Accelerator::viewIn(command,m_cell_lambda);
    auto mu_copy = Accelerator::viewIn(command, m_cell_mu);

    m_bsr_format.computeSparsity();
    if (mesh()->dimension() == 2)
      m_bsr_format.assembleBilinearAtomicFree([=] ARCCORE_HOST_DEVICE(CellLocalId cell_lid, Int32 node_lid) { return computeElementVectorTria3Gpu(cell_lid, cn_cv, in_node_coord, lambda_copy, mu_copy, node_lid); });
    if (mesh()->dimension() == 3)
      m_bsr_format.assembleBilinearAtomicFree([=] ARCCORE_HOST_DEVICE(CellLocalId cell_lid, Int32 node_lid) { return computeElementVectorTetra4Gpu(cell_lid, cn_cv, in_node_coord, lambda_copy, mu_copy, node_lid); });
    m_bsr_format.toLinearSystem(m_linear_system);
  }
  else if (m_matrix_format == "Alien-BSR")
  {
    UnstructuredMeshConnectivityView m_connectivity_view(mesh());
    if (mesh()->dimension() == 2)
    {
      if (m_hex_quad_mesh) {
        _assembleBilinearOperatorCpu<8>([this](const Cell& cell) { return _computeElementMatrixQuad4(cell); });
      }
      else {
        _assembleBilinearOperatorCpu<6>([this](const Cell& cell) { return _computeElementMatrixTria3(cell); });
      }
    }
    if (mesh()->dimension() == 3)
    {
      if (m_hex_quad_mesh) {
        _assembleBilinearOperatorCpu<24>([this](const Cell& cell) { return _computeElementMatrixHexa8(cell); });
      }
      else {
        _assembleBilinearOperatorCpu<12>([this](const Cell& cell) { return _computeElementMatrixTetra4(cell); });
      }
    }
  }
#ifdef ALIEN_USE_SYCL
  else if (m_matrix_format == "Alien-BSR-Acc")
  {
    _assembleBilinearOperatorAlienAcc() ;
  }
#endif
  else if (m_matrix_format == "DOK")
  {
    if (mesh()->dimension() == 2) {
      if (m_hex_quad_mesh) {
        _assembleBilinearOperatorCpu<8>([this](const Cell& cell) { return _computeElementMatrixQuad4(cell); });
      }
      else {
        _assembleBilinearOperatorCpu<6>([this](const Cell& cell) { return _computeElementMatrixTria3(cell); });
      }
    }
    if (mesh()->dimension() == 3) {
      if (m_hex_quad_mesh) {
        _assembleBilinearOperatorCpu<24>([this](const Cell& cell) { return _computeElementMatrixHexa8(cell); });
      }
      else {
        _assembleBilinearOperatorCpu<12>([this](const Cell& cell) { return _computeElementMatrixTetra4(cell); });
      }
    }
  }
  else {
    ARCANE_FATAL("Unsupported matrix type, only DOK| BSR|AF-BSR is supported.");
  }

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"lhs-matrix-assembly", elapsedTime);
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Assembles the FEM bilinear operator on CPU.
 *
 * This method assembles the FEM stiffness matrix by iterating over each cell,
 * computing the element stiffness matrix using the provided function, and
 * populating the global stiffness matrix accordingly.
 *
 * @tparam N Total DOF size (nodes_per_element × dimensions).
 * @param compute_element_matrix function computing cell's element stiffness matrix.
 */
/*---------------------------------------------------------------------------*/

template <int N>
void FemModuleElasticity::
_assembleBilinearOperatorCpu(const std::function<RealMatrix<N, N>(const Cell&)>& compute_element_matrix)
{
  const Int32 dim = mesh()->dimension();
  auto node_dof(m_dofs_on_nodes.nodeDoFConnectivityView());

  ENUMERATE_ (Cell, icell, allCells())
  {
    Cell cell = *icell;
    auto K_e = compute_element_matrix(cell);

    Int32 n1_index = 0;
    for (Node node1 : cell.nodes())
    {
      if (node1.isOwn())
      {
        Int32 n2_index = 0;
        for (Node node2 : cell.nodes()) {
          for (Int32 i = 0; i < dim; ++i) {
            DoFLocalId dof1 = node_dof.dofId(node1, i);
            for (Int32 j = 0; j < dim; ++j) {
              DoFLocalId dof2 = node_dof.dofId(node2, j);
              Real value = K_e(dim * n1_index + i, dim * n2_index + j);
              m_linear_system.matrixAddValue(dof1, dof2, value);
            }
          }
          ++n2_index;
        }
      }
      ++n1_index;
    }
  }
}


/*---------------------------------------------------------------------------*/
/**
 * @brief Solves the linear system.
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
_solve()
{
  info() << "[ArcaneFem-Info] Started module  _solve()";
  Real elapsedTime = platform::getRealTime();

  m_linear_system.applyLinearSystemTransformationAndSolve();

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"solve-linear-system", elapsedTime);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
_validateResults()
{
  info() << "[ArcaneFem-Info] Started module  _validateResults()";
  Real elapsedTime = platform::getRealTime();

  if (allNodes().size() < 200) {
    int p = std::cout.precision();
    std::cout.precision(17);
    ENUMERATE_ (Node, inode, allNodes()) {
      Node node = *inode;
      std::cout << "( N_id, u1, u2, u3 ) = ( "
                << node.uniqueId() << ", " << m_U[node].x << ", " << m_U[node].y << ", " << m_U[node].z
                << ")\n";
    }
    std::cout.precision(p);
  }

  String filename = options()->solutionComparisonFile();
  const double epsilon = options()->resultEpsilon();
  const double min_value_to_test = 1.0e-10;

  Arcane::FemUtils::checkNodeResultFile(traceMng(), filename, m_U, epsilon, min_value_to_test);

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"result-validation", elapsedTime);
}

/*---------------------------------------------------------------------------*/
/**
 * @brief Update the FEM variables.
 *
 * This method performs the following actions:
 *   1. Fetches values of solution from solved linear system to FEM variables,
 *      i.e., it copies RHS DOF to u.
 *   2. Performs synchronize of FEM variables across subdomains.
 */
/*---------------------------------------------------------------------------*/

void FemModuleElasticity::
_updateVariables()
{
  info() << "[ArcaneFem-Info] Started module  _updateVariables()";
  Real elapsedTime = platform::getRealTime();

  {
    VariableDoFReal& dof_u(m_linear_system.solutionVariable());
    auto node_dof(m_dofs_on_nodes.nodeDoFConnectivityView());
    if (mesh()->dimension() == 3)
      ENUMERATE_ (Node, inode, ownNodes()) {
        Node node = *inode;
        Real u1_val = dof_u[node_dof.dofId(node, 0)];
        Real u2_val = dof_u[node_dof.dofId(node, 1)];
        Real u3_val = dof_u[node_dof.dofId(node, 2)];
        m_U[node] = Real3(u1_val, u2_val, u3_val);
      }
    else
      ENUMERATE_ (Node, inode, ownNodes()) {
        Node node = *inode;
        Real u1_val = dof_u[node_dof.dofId(node, 0)];
        Real u2_val = dof_u[node_dof.dofId(node, 1)];
        m_U[node] = Real3(u1_val, u2_val, 0.);
      }
  }

  m_U.synchronize();

  elapsedTime = platform::getRealTime() - elapsedTime;
  ArcaneFemFunctions::GeneralFunctions::printArcaneFemTime(traceMng(),"update-variables", elapsedTime);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_MODULE_FEM(FemModuleElasticity);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
