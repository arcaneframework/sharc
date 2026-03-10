#include <arcane/ArcaneVersion.h>
#include <arcane/ITimeLoopMng.h>
#include <arcane/mesh/ItemFamily.h>
#include <arcane/Timer.h>
#include <arcane/utils/IMemoryInfo.h>
#include <arcane/utils/OStringStream.h>
#include <arcane/utils/PlatformUtils.h>
#include <mpi.h>
#include <time.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Utils/CAWF/ICAWFMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#ifdef USE_ALIEN_V2
#include <alien/arcane_tools/accessors/ItemVectorAccessor.h>
#include <alien/core/block/VBlock.h>

#include <alien/arcane_tools/IIndexManager.h>
#include <alien/arcane_tools/indexManager/BasicIndexManager.h>
#include <alien/arcane_tools/indexManager/SimpleAbstractFamily.h>
#include <alien/arcane_tools/distribution/DistributionFabric.h>
#include <alien/arcane_tools/indexSet/IndexSetFabric.h>
#include <alien/arcane_tools/data/Space.h>

#include <alien/kernels/simple_csr/algebra/SimpleCSRLinearAlgebra.h>

#include <alien/ref/AlienRefSemantic.h>

#include <alien/kernels/redistributor/Redistributor.h>
#include <alien/ref/data/scalar/RedistributedVector.h>
#include <alien/ref/data/scalar/RedistributedMatrix.h>

#include <alien/expression/solver/SolverStater.h>

#include <alien/expression/solver/ILinearSolver.h>

#include <alien/expression/normalization/NormalizeOpt.h>
#endif

#include "CompactionModule.h"

#include <arcane/ItemPairGroup.h>
#include <arcane/IMesh.h>


using namespace Arcane;
using namespace Alien;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void CompactionModule::init()
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
  }
  else
  {
    m_max_iter = options()->maxIter() ;
  }

  m_linear_solver = options()->linearSolver();
  m_linear_solver->init() ;

  if (not m_linear_solver->hasParallelSupport() and m_parallel_mng->commSize() > 1)
  {
    fatal() << "Current solver has not a parallel support for solving linear system : skip it";
  }

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

  FaceGroup boundary = mesh()->allCells().outerFaceGroup();
  FaceGroup top_face_boundary = mesh()->faceFamily()->findGroup("TopBoundary");
  if(top_face_boundary.empty())
  {
    const IGeometryMng::Real3Variable & face_center =
        m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PCenter);
    UniqueArray<Integer> top_face_lids;
    top_face_lids.reserve(boundary.size());
    ENUMERATE_FACE(iface,boundary)
    {
      info()<<"BOUNDARY FACE["<<iface->localId()<<"]"<<face_center[*iface].z;
      if(face_center[*iface].z == 0.)
        top_face_lids.add(iface->localId());
    }
    mesh()->faceFamily()->createGroup("TopBoundary",top_face_lids,true) ;
    top_face_boundary = mesh()->faceFamily()->findGroup("TopBoundary");
  }
  {
    const IGeometryMng::Real3Variable & face_center =
        m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PCenter);
    Real top_z = 0. ;
    ENUMERATE_FACE(iface,top_face_boundary)
    {
      top_z += face_center[*iface].z ;
    }
    top_z = top_z/top_face_boundary.size() ;
    m_top_z = top_z;
    info()<<"TOP BOUNDARY AVG HEIGHT : "<<m_top_z;
  }

  info()<<"TOP BOUNDARY SIZE["<<m_event_index<<"] : "<<top_face_boundary.size();
  m_face_is_top.fill(0) ;
  ENUMERATE_FACE(iface,top_face_boundary)
  {
    m_face_is_top[iface] = 1 ;
  }
  {
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
  Real rho0    = options()->surfaceDensity() ;
  Real drhodp0 = options()->surfaceCompressibility() ;
  ENUMERATE_CELL(icell,allCells())
  {
    m_rho[*icell]    = rho0 ;
    m_drhodp[*icell] = drhodp0 ;
    m_layer_id[*icell] = m_new_layer_id ;
  }

  /*
  m_areaU = allCells();
  m_cell_cell_connection     = CellCellGroup(m_areaU.own(),m_areaU,m_stencil_kind);
  m_all_cell_cell_connection = CellCellGroup(m_areaU,m_areaU,m_stencil_kind);

  m_index_manager.reset(new Alien::ArcaneTools::BasicIndexManager(m_parallel_mng));

  //Alien::BasicIndexManager index_manager(m_parallel_mng);
  m_index_manager->setTraceMng(traceMng());

  m_indexSetU = m_index_manager->buildScalarIndexSet("U", m_areaU);
  m_index_manager->prepare();

  m_allUIndex = m_index_manager->getIndexes(m_indexSetU);
  info()<<"END Compaction MODULE INITIALIZATION";

  ///////////////////////////////////////////////////////////////////////////
  //
  // CREATE Space FROM IndexManger
  // CREATE MATRIX ASSOCIATED TO Space
  // CREATE VECTORS ASSOCIATED TO Space
  //
  m_space = Alien::ArcaneTools::Space(m_index_manager.get(), "TestSpace");
  m_mdist = Alien::ArcaneTools::createMatrixDistribution(m_space);
  m_vdist = Alien::ArcaneTools::createVectorDistribution(m_space);

  m_globalSize = m_vdist.globalSize();
  m_localSize = m_vdist.localSize();
  info()<<"GLOBAL SIZE : "<<m_vdist.globalSize();


  m_vectorB  = Alien::Vector(m_vdist);
  m_vectorBB = Alien::Vector(m_vdist);
  m_vectorX  = Alien::Vector(m_vdist);

  m_matrixA = Alien::Matrix(m_mdist); // local matrix for exact measure without side effect
  ///////////////////////////////////////////////////////////////////////////
  //
  // MATRIX BUILDING
  //
  {
    Alien::MatrixProfiler profiler(m_matrixA);
    ///////////////////////////////////////////////////////////////////////////
    //
    // DEFINE PROFILE
    //
    ENUMERATE_ITEMPAIR(Cell,Cell,icell,m_cell_cell_connection)
    {
      const Cell & cell = *icell;
      const Integer iIndex = m_allUIndex[cell.localId()];
      profiler.addMatrixEntry(iIndex, m_allUIndex[cell.localId()]);
      ENUMERATE_SUB_ITEM(Cell,isubcell,icell)
      {
        const Cell& subcell = *isubcell;
        profiler.addMatrixEntry(iIndex,m_allUIndex[subcell.localId()]);
      }
    }
  }*/

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

}

/*---------------------------------------------------------------------------*/

void
CompactionModule::saveOldState()
{
  info() << " App1 saveOldState() " ;
  ENUMERATE_CELL(icell,allCells())
  {
    m_c_N[icell] = m_c[icell] ;
  }

}
void CompactionModule::reloadOldState()
{
  info() << " App1  ReloadOldState " ;
  ENUMERATE_CELL(icell,allCells())
  {
    m_c[icell] = m_c_N[icell] ;
  }
}

void
CompactionModule::
computePressure()
{
  const IGeometryMng::RealVariable & cell_measure =
    m_geometry_mng->getRealVariableProperty(mesh()->allCells(),IGeometryProperty::PMeasure);

  const IGeometryMng::Real3Variable & cell_center =
      m_geometry_mng->getReal3VariableProperty(mesh()->allCells(),IGeometryProperty::PCenter);

  const IGeometryMng::RealVariable & face_measure =
    m_geometry_mng->getRealVariableProperty(mesh()->allFaces(),IGeometryProperty::PMeasure);

  const IGeometryMng::Real3Variable & face_center =
      m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PCenter);

  const IGeometryMng::Real3Variable & face_normal =
      m_geometry_mng->getReal3VariableProperty(mesh()->allFaces(),IGeometryProperty::PNormal);

  auto areaU = allCells();

  auto index_manager = Alien::ArcaneTools::BasicIndexManager(m_parallel_mng);

  //Alien::BasicIndexManager index_manager(m_parallel_mng);
  index_manager.setTraceMng(traceMng());

  auto indexSetU = index_manager.buildScalarIndexSet("U", areaU);
  index_manager.prepare();

  auto const& allUIndex = index_manager.getIndexes(indexSetU);
  info()<<"END Compaction MODULE INITIALIZATION";

  ///////////////////////////////////////////////////////////////////////////
  //
  // CREATE Space FROM IndexManger
  // CREATE MATRIX ASSOCIATED TO Space
  // CREATE VECTORS ASSOCIATED TO Space
  //
  auto space = Alien::ArcaneTools::Space(&index_manager, "PressureModelSpace");
  auto mdist = Alien::ArcaneTools::createMatrixDistribution(space);
  auto vdist = Alien::ArcaneTools::createVectorDistribution(space);

  auto globalSize = vdist.globalSize();
  auto localSize  = vdist.localSize();
  info()<<"GLOBAL SIZE : "<<vdist.globalSize();


  auto vectorB  = Alien::Vector(vdist);
  auto vectorBB = Alien::Vector(vdist);
  auto vectorX  = Alien::Vector(vdist);

  auto matrixA = Alien::Matrix(mdist); // local matrix for exact measure without side effect
  ///////////////////////////////////////////////////////////////////////////
  //
  // MATRIX BUILDING
  //
  {
    Alien::MatrixProfiler profiler(matrixA);
    ///////////////////////////////////////////////////////////////////////////
    //
    // DEFINE PROFILE
    //
    ENUMERATE_CELL(icell,areaU.own())
    {
      const Cell & cell = *icell;
      const Integer iIndex = allUIndex[cell.localId()];
      profiler.addMatrixEntry(iIndex, iIndex);
      ENUMERATE_FACE(iface,icell->faces())
      {
        if(not iface->isSubDomainBoundary())
        {
          Cell jCell ;
          if(iface->backCell()==cell)
            jCell = iface->frontCell() ;
          else
            jCell = iface->backCell() ;
          profiler.addMatrixEntry(iIndex,allUIndex[jCell.localId()]);
        }
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////
  //
  // VECTOR BUILDING AND FILLING
  //
  info() << "Building & initializing vector b";
  info() << "Space size = " << globalSize << ", local size= " << localSize;
  {
      // Builder du vecteur
      Alien::VectorWriter vX(vectorX);
      Alien::VectorWriter vB(vectorB);
      ENUMERATE_CELL(icell,areaU.own())
      {
        const Integer iIndex = allUIndex[icell->localId()];
        //vX[iIndex] = m_u[icell] ;
        vX[iIndex] = 0. ;
        vB[iIndex] = 0. ;
      }
  }

  ///////////////////////////////////////////////////////////////////////////
  //
  // MATRIX FILLING
  //
  //   S_j  Pj/2 n_ij S_ij = - rho g Vi ez
  {
    auto builder = Alien::ProfiledMatrixBuilder(matrixA,Alien::ProfiledMatrixOptions::eResetValues);

    auto vB = Alien::VectorWriter{vectorB};
    ENUMERATE_CELL(icell, areaU.own())
    {
        const Cell & cell = *icell;

        Integer i = allUIndex[cell.localId()];

        vB[i] += - m_gravity * m_rho[cell] * cell_measure[cell] ;

        ENUMERATE_FACE(iface, icell->faces())
        {
          Face const& face = *iface ;
          if(iface->isSubDomainBoundary())
          {
            Integer sgn = iface->isSubDomainBoundaryOutside() ? 1 : -1 ;
            double off_diag = 0.5 * sgn * face_normal[face].z ;
            if((m_face_is_top[face])/*||(m_face_is_nz[*iface]==1)*/)
            {
              //vB[i] -= off_diag ;
              double diag = 1000. ;
              builder(i,i) = diag ;
              //vB[i] += diag * m_gravity * m_rho[cell] * (face_center[face].z - cell_center[cell].z) ;
              vB[i] = diag * m_gravity * m_rho[cell] * (m_top_z - cell_center[cell].z) ;

              //info()<<"TOP FACE DIAG "<<i<<" "<<i<<" "<<diag;
              //info()<<"TOP FACE RHS  "<<i<<" "<<off_diag;
            }
            else
            {
              //if(m_face_is_nz[*iface]== -1)
                builder(i,i) += off_diag;
              /*
              else
              {
                double diag = 1000. ;
                builder(i,i) = diag ;
                //vB[i] += diag * m_gravity * m_rho[cell] * (face_center[face].z - cell_center[cell].z) ;
                vB[i] = diag * m_gravity * m_rho[cell] * (m_top_z - cell_center[cell].z) ;
              }*/
              //info()<<"BOUNDARY FACE DIAG "<<i<<" "<<i<<" "<<off_diag;
            }
          }
          else
          {
            //if(m_face_is_nz[*iface]==1)
            {
              Integer sgn = 1 ;
              Cell jCell ;
              if(iface->backCell()==cell)
                jCell = iface->frontCell() ;
              else
              {
                jCell = iface->backCell() ;
                sgn = -1 ;
              }
              Integer j = allUIndex[jCell.localId()];
              double off_diag = 0.5 * sgn * face_normal[face].z ;
              builder(i,j) += off_diag ;
              //info()<<"INTERNAL OFF DIAG "<<i<<" "<<j<<" "<<off_diag;
            }
          }
        }
    }
    builder.finalize();
  }


  Alien::NormalizeOpt op ;
  op.setAlgo(Alien::NormalizeOpt::StdLU) ;
  op.setOpt(Alien::NormalizeOpt::SumFirstEq,true) ;
  op.normalize(matrixA,vectorB) ;

  m_linear_solver->solve(matrixA,vectorB,vectorX) ;

  Alien::SolverStatus status = m_linear_solver->getStatus();
  if(status.succeeded)
  {

    Alien::VectorReader reader(vectorX);
    ENUMERATE_CELL(icell,areaU.own())
    {
      const Integer iIndex = allUIndex[icell->localId()];
      m_x[icell] = reader[iIndex] ;
      m_u[icell] = reader[iIndex] ;
      m_ul[icell] = reader[iIndex] ;
      //info()<<"SOL["<<icell->localId()<<"]"<<m_u[icell];
    }
    m_x.synchronize() ;
    m_u.synchronize() ;
    m_ul.synchronize() ;
  }
  m_linear_solver->getSolverStat().print(Universe().traceMng(), status, "Linear Solver : ") ;

}
void
CompactionModule::
test()
{

  info()<<"Compaction MODULE COMPUTATION";

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
          /*
          ///////////////////////////////////////////////////////////////////////////
          //
          // VECTOR BUILDING AND FILLING
          //
          info() << "Building & initializing vector b";
          info() << "Space size = " << m_globalSize << ", local size= " << m_localSize;
          {
              // Builder du vecteur
              Alien::VectorWriter vX(m_vectorX);
              Alien::VectorWriter vB(m_vectorB);
              ENUMERATE_CELL(icell,m_areaU.own())
              {
                const Integer iIndex = m_allUIndex[icell->localId()];
                //vX[iIndex] = m_u[icell] ;
                vX[iIndex] = 0. ;
                vB[iIndex] = 0. ;
              }
          }
          // precice.readVectorData
          m_cawf_mng->startTimeStep() ;
#ifdef USE_PRECICE_V3
          if((m_layer_event_period>0) && (m_layers.size() > 0) && (m_time > m_next_layer_event_time))
          {
            ++m_new_layer_id ;
            if(m_new_layer_id<m_layers.size())
            {
              info()<<"ACTIVATE NEW LAYER : "<<m_new_layer_id<<" "<<m_layers[m_new_layer_id].size();
              m_activated_layers_uids.reserve(m_activated_layers_uids.size()+m_layers[m_new_layer_id].size()) ;
              ENUMERATE_CELL(icell,m_layers[m_new_layer_id])
              {
                m_activated_layers_uids.add(icell->uniqueId()) ;
              }
              m_cawf_mng->setAppMeshFilter(m_activated_layers_uids) ;
              m_next_layer_event_time += m_layer_event_period ;
            }
          }
#endif

          Alien::ILinearSolver * solver = options()->linearSolver();
          solver->init() ;

          if (not solver->hasParallelSupport() and m_parallel_mng->commSize() > 1)
          {
            fatal() << "Current solver has not a parallel support for solving linear system : skip it";
          }

          ///////////////////////////////////////////////////////////////////////////
          //
          // MATRIX FILLING
          //
          {
            Alien::ProfiledMatrixBuilder builder(m_matrixA,
                                                 Alien::ProfiledMatrixOptions::eResetValues);
            ENUMERATE_ITEMPAIR(Cell, Cell, icell, m_cell_cell_connection)
            {
                const Cell & cell = *icell;
                double diag = dii(cell) ;

                Integer i = m_allUIndex[cell.localId()];
                builder(i,i) += diag;
                ENUMERATE_SUB_ITEM(Cell, isubcell, icell)
                {
                  const Cell& subcell = *isubcell;
                  double off_diag = fij(cell,subcell) ;
                  builder(i,i) += off_diag;
                  Integer j = m_allUIndex[subcell.localId()];
                  builder(i,j) -= off_diag;
                }
            }
            {
              Alien::VectorWriter vB(m_vectorB);
              ENUMERATE_FACE(iface,allCells().outerFaceGroup())
              {
                if(m_face_type[iface]==2)
                {
                  double off_diag = funck(m_face_center[iface]) ;
                  Cell const& cell = iface->boundaryCell() ;
                  const Integer i = m_allUIndex[cell.localId()];

                  builder(i,i) += off_diag;
                  vB[i] += off_diag*m_bu[iface] ;
                }
              }
            }
            builder.finalize();
          }


          solver->solve(m_matrixA,m_vectorB,m_vectorX) ;

          Alien::SolverStatus status = solver->getStatus();
          if(status.succeeded)
          {

            Alien::VectorReader reader(m_vectorX);
            ENUMERATE_CELL(icell,m_areaU.own())
            {
              const Integer iIndex = m_allUIndex[icell->localId()];
              m_x[icell] = reader[iIndex] ;
              m_u[icell] = reader[iIndex] ;
              m_ul[icell] = reader[iIndex] ;
            }
            m_x.synchronize() ;
            m_u.synchronize() ;
            m_ul.synchronize() ;
          }
          solver->getSolverStat().print(Universe().traceMng(), status, "Linear Solver : ") ;
          */

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

#ifndef USE_PRECICE_V3
            if((m_layer_event_period>0) && (m_layers.size() > 0) && (m_time > m_next_layer_event_time))
            {
              ++m_new_layer_id ;
              if(m_new_layer_id<m_layers.size())
              {
                info()<<"ACTIVATE NEW LAYER : "<<m_new_layer_id<<" "<<m_layers[m_new_layer_id].size();
                m_activated_layers_uids.reserve(m_activated_layers_uids.size()+m_layers[m_new_layer_id].size()) ;
                ENUMERATE_CELL(icell,m_layers[m_new_layer_id])
                {
                  m_activated_layers_uids.add(icell->uniqueId()) ;
                }
                m_cawf_mng->setAppMeshFilter(m_activated_layers_uids) ;
                m_next_layer_event_time += m_layer_event_period ;
              }
            }
#endif
            info() << " VALIDATESTEP " ;
          }
          else
          {
            ++sub_iter;
            m_global_deltat = 0. ;
            info() << " ITERATE COUPLING " ;
            info() << " Global Time " << m_global_time() << " Iter " << m_global_iteration();
          }

          m_linear_solver->end() ;
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
      computePressure() ;
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
            Real rho0    = options()->surfaceDensity() ;
            Real drhodp0 = options()->surfaceCompressibility() ;
            ENUMERATE_CELL(icell,new_event)
            {
              m_rho[*icell]    = rho0 ;
              m_drhodp[*icell] = drhodp0 ;
              m_layer_id[*icell] = m_new_layer_id ;
            }

            FaceGroup top_boundary = mesh()->faceFamily()->findGroup("TopBoundary") ;
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
                mesh()->faceFamily()->createGroup("TopBoundary",top_face_lids,true) ;
            }

            FaceGroup const& top_face_boundary = mesh()->faceFamily()->findGroup("TopBoundary");
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

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/


ARCANE_REGISTER_MODULE_COMPACTION(CompactionModule);
