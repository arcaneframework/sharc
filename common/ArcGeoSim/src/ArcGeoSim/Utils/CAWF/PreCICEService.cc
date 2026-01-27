// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include <arcane/ArcaneVersion.h>
#include <arcane/utils/Math.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/ITimeLoopMng.h>
#include <arcane/Timer.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/MathUtils.h>
#include <arcane/IMeshModifier.h>
#include <arcane/IMesh.h>
#include <arcane/IParallelMng.h>
#include <arcane/ItemRefinementPattern.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/VariableTypedef.h>
#include <arcane/IMesh.h>
#include <arcane/ItemTypes.h>
#include <arcane/VariableBuildInfo.h>
#include <boost/shared_ptr.hpp>

#include <arcane/mesh/GraphDoFs.h>
#include <arcane/mesh/DynamicMesh.h>

#include <vector>
#include <cmath>

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Utils/ArrayUtils.h"
#include "ArcGeoSim/Mesh/Utils/GraphUtils.h"

#ifdef USE_PRECICE
#ifdef USE_PRECICE_V3
#include "precice/precice.hpp"
#include "precice/UserAppTools.hpp"
#else
#include "precice/SolverInterface.hpp"
#endif
#endif

#include "ArcGeoSim/Utils/CAWF/ICAWFMng.h"

#include "PreCICE_axl.h"

#ifdef USE_PRECICE_V3
class UserConvergenceOp
: public precice::IUserAppConvergenceOp
{
public :
    UserConvergenceOp(int max_iter)
  : _max_iter(max_iter)
  {

  }

  void newMeasurementSeries()
  {
      _iter = 0 ;
      _convergence = false ;
  }

  void setConvergence(bool value)
  {
      _convergence = value ;
  }

  double measure(
       const Eigen::VectorXd &oldValues,
       const Eigen::VectorXd &newValues)
  {
      ++_iter ;
      _convergence = _convergence || (_iter >= _max_iter) ;
      //std::cout<<"USER APP MEASURE : "<<_iter<<" "<<_max_iter<<" CONV="<<_convergence<<std::endl ;
      return 0. ;
  }

   /// Returns true, if the last measurement indicates convergence.
   bool isConvergence() const {
       return _convergence ;
   }
private :
  int _iter = 0 ;
  int _max_iter = -1 ;
  bool _convergence = false ;
};
#endif

class PreCICEService
: public ArcanePreCICEObject
{

public:
    typedef Integer lid_type ;
    typedef Int64   uid_type ;

  //! Constructeur de la classe
  PreCICEService(const Arcane::ServiceBuildInfo & sbi)
  : ArcanePreCICEObject(sbi)
  {
    ;
  }

  //! Destructeur
  virtual ~PreCICEService() {}

public:
  
  //====================================================================================================
  // Les méthodes d'interface
  //====================================================================================================
  void baseInit();
  void init();
  void init(ICouplingMesh* mesh);

  void initMesh();
  void initMesh(Arcane::ConstArrayView<Int64> uids);

  void initData();

  bool isAppLeader() ;

#ifdef USE_PRECICE_V3
  void updateMesh() ;
  void updateMesh(ICouplingMesh* mesh) ;
  void setAppMeshFilter(Arcane::ConstArrayView<Int64> uids);
#else
  void setAppMeshFilter(Arcane::ConstArrayView<Int64> uids);
  void activateMeshFilter(Arcane::String const& mesh_name,bool values);
#endif

  void start();

  void update() ;

  void finalize() ;

  void startTimeStep() ;

  void endTimeStep() ;

  Real initialTimeStep() ;

  Real newTimeStep(Real current_dt) ;

  bool isCouplingOngoing() ;

  bool validateCurrentTimeStep() ;

  void setTimeIterStateOp(ICAWFMng::ITimeIterStateOp* op)
  {
      m_state_op = op ;
  }

  void setConvergence(bool values) ;

private:
  void _markFullFilledWrite();
  template<typename ItemType>
  void _updateMeshVariable(const String var_name, const String family_name, const ItemGroup group);
  template<typename ItemType>
  void _writeMeshVariable(const Integer i, const String family_name, const ItemGroup group);

  IParallelMng* m_parallel_mng = nullptr;
  eItemKind     m_vertex_kind  = Arcane::IK_Cell;
#ifdef USE_PRECICE
#ifdef USE_PRECICE_V3
  typedef precice::Participant     SolverInterfaceType ;
#else
  typedef precice::SolverInterface SolverInterfaceType ;
#endif
  std::unique_ptr<SolverInterfaceType> m_precice_interface ;
#endif
  int           m_app_id       = 0 ;
  String        m_app_name     = "App0" ;
  String        m_config_file  = "./precice-config.xml" ;

  bool          m_evolutive_mesh = false ;
  String        m_mesh_name      = "Mesh0" ;
  Integer       m_mesh_id        = 0 ;
  int           m_dimensions     = 0 ;

  std::size_t                 m_nb_vertices    = 0 ;
  std::vector<double>         m_vertices;
  std::vector<int>            m_vertex_ids;
  std::vector<uid_type>       m_vertex_uids;
  std::map<uid_type,lid_type> m_vertex_uid2lid ;
  std::vector<int>            m_evol_vertex_ids;
  std::vector<int>            m_evol_vertex_lids;
  std::map<uid_type,lid_type> m_evol_vertex_uid2lid ;

  std::vector<double> m_readScalarBuffer;
  std::vector<std::vector<double>> m_writeScalarBuffer;

  struct Var
  {
      String  m_name ;
      String  m_var_name ;
      eItemKind m_item_kind ;
      Integer m_id ;
  } ;
  UniqueArray<Var> m_read_datas ;
  UniqueArray<Var> m_write_datas ;

  ICAWFMng::ITimeIterStateOp* m_state_op = nullptr ;

#ifdef USE_PRECICE_V3
  std::unique_ptr<precice::IUserAppConvergenceOp> m_user_convergence_op ;
#endif


  Real          m_initial_dt = 0. ;
  Real          m_dt = 0. ;
  bool          m_current_time_step_is_valid = false ;
  Integer       m_max_iter = 0 ;
  Integer       m_iter     = 0 ;
  bool          m_iter_convergence = false ;

  bool          m_is_initialized = false;
  bool          m_read_then_write = true; // else write first then read

};



//==========================================================================================
// initialization
//==========================================================================================
void
PreCICEService ::baseInit()
{
  m_parallel_mng = subDomain()->parallelMng();


  m_app_id          = options()->appId() ;
  m_app_name        = options()->appName() ;
  m_config_file     = options()->configFile() ;
  m_mesh_name       = options()->meshName() ;
  m_evolutive_mesh  = options()->evolutiveMesh() ;
  m_max_iter        = options()->maxIter() ;
  m_read_then_write = options()->readThenWrite() ;
  m_iter            = 0 ;
  m_iter_convergence = false ;


  m_precice_interface.reset(new SolverInterfaceType(m_app_name.localstr(),
                                                    m_config_file.localstr(),
                                                    m_parallel_mng->commRank(),
                                                    m_parallel_mng->commSize()));
#ifdef USE_PRECICE_V3
  m_dimensions = m_precice_interface->getMeshDimensions(m_mesh_name.localstr());
#else
  m_mesh_id     = m_precice_interface->getMeshID(m_mesh_name.localstr());
  m_dimensions  = m_precice_interface->getDimensions();
#endif
  for(Integer i=0; i< options()->readData.size();++i)
  {
      auto const& opt = options()->readData[i] ;
#ifdef USE_PRECICE_V3
      Integer id = -1 ;
#else
      auto id = m_precice_interface->getDataID(opt->name().localstr(), m_mesh_id) ;
#endif
      m_read_datas.add( Var{opt->name(),opt->varName(),opt->itemKind(),id}) ;
  }
  for(Integer i=0;i<options()->writeData.size();++i)
  {
      auto const& opt = options()->writeData[i] ;
#ifdef USE_PRECICE_V3
      Integer id = -1 ;
#else
      auto id = m_precice_interface->getDataID(opt->name().localstr(), m_mesh_id) ;
#endif
      m_write_datas.add(Var{opt->name(),opt->varName(),opt->itemKind(),id}) ;
  }

#ifdef USE_PRECICE_V3
  if(options()->appConvCrit.size()>0)
  {
      auto max_iter  = options()->appConvCrit[0]->maxIter() ;
      auto mesh_name = options()->appConvCrit[0]->meshName() ;
      auto data_name = options()->appConvCrit[0]->dataName() ;
      bool suffice   = options()->appConvCrit[0]->suffice() ;
      bool strict    = options()->appConvCrit[0]->strict() ;
      m_user_convergence_op.reset(new UserConvergenceOp(max_iter)) ;
      m_precice_interface->addUserAppConvergenceMeasure(mesh_name.localstr(),
                                                        data_name.localstr(),
                                                        suffice,
                                                        strict,
                                                        m_user_convergence_op.get()) ;
  }
#endif
}

void
PreCICEService ::init()
{
  if(m_is_initialized) return ;

  baseInit() ;

  m_nb_vertices = ownCells().size() ;
  m_vertices.resize(m_nb_vertices * m_dimensions);
  m_vertex_ids.resize(m_nb_vertices);
  m_vertex_uids.reserve(m_nb_vertices);

  switch(m_vertex_kind)
  {
      case Arcane::IK_Cell:
      {
          int i = 0 ;
          int icount = 0 ;
          ENUMERATE_CELL(icell,ownCells())
          {
              m_vertex_uids.push_back(icell->uniqueId()) ;
              Real3 x;
              ENUMERATE_NODE(inode,icell->nodes())
                {
                  x += m_node_coord[*inode];
                }
              x /= icell->nbNode();
              for (int j = 0; j < m_dimensions; j++)
                {
                  m_vertices[j + m_dimensions*i] = x[j];
                }
              ++i;
          }
      }
      break ;
      default:
          fatal()<<"Not yet implemented Vertex kind : "<<m_vertex_kind;
  }
  for(int lid=0;lid<m_vertex_uids.size();++lid)
  {
      m_vertex_uid2lid[m_vertex_uids[lid]] = lid ;
  }

#ifndef USE_PRECICE_V3
  if(options()->meshFilter.size()>0)
  {
    for(int i=0;i<options()->meshFilter.size();++i)
      {
        String mesh_name = options()->meshFilter[i]->mesh() ;
        bool filter = options()->meshFilter[i]->filter() ;
        if(filter)
          activateMeshFilter(mesh_name,filter) ;
      }
  }
#endif
  info()<<"PRECISE IS INITIALIZED : DT INIT ="<<m_dt;
  m_is_initialized = true ;
}

void
PreCICEService ::init(ICouplingMesh* mesh)
{
  if(m_is_initialized) return ;

  baseInit() ;

  mesh->initializeCAWF(m_vertices,m_vertex_uids) ;
  m_nb_vertices = m_vertex_uids.size() ;
  for(int lid=0;lid<m_vertex_uids.size();++lid)
  {
      //info()<<"VERTEX["<<lid<<"] UID="<<m_vertex_uids[lid]<<" COORD("<<m_vertices[3*lid]<<","<<m_vertices[3*lid+1]<<","<<m_vertices[3*lid+2]<<")";
      m_vertex_uid2lid[m_vertex_uids[lid]] = lid ;
  }


#ifdef USE_PRECICE
  m_vertex_ids.resize(m_nb_vertices);

#endif

  info()<<"PRECISE IS INITIALIZED : DT INIT ="<<m_dt;
  m_is_initialized = true ;
}

void
PreCICEService::initMesh()
{

#ifdef USE_PRECICE
#ifdef USE_PRECICE_V3
  m_precice_interface->setMeshVertices(m_mesh_name.localstr(), m_vertices, m_vertex_ids);
#else
  m_precice_interface->setMeshVertices(m_mesh_id, m_nb_vertices, m_vertices.data(), m_vertex_ids.data());
#endif
  info()<<"ALLOCATE SEND RECV BUFFER : "<<m_nb_vertices;
  m_readScalarBuffer.resize(m_nb_vertices);
  m_writeScalarBuffer.resize(m_write_datas.size());
  for(auto& buffer : m_writeScalarBuffer)
      buffer.resize(m_nb_vertices) ;

#endif
  /*
  {
      std::ofstream file("precice-mesh.txt") ;
      for(std::size_t i=0;i<m_nb_vertices;++i)
      {
          file<<m_vertex_ids[i]<<" "<<m_vertices[3*i]<<" "<<m_vertices[3*i+1]<<" "<<m_vertices[3*i+2]<<std::endl ;
      }
  }*/
}

void
PreCICEService::initMesh(Arcane::ConstArrayView<Int64> uids)
{
  info()<<"PreCICEService::setAppMeshFilter";
#ifdef USE_PRECICE_V3
  m_evol_vertex_ids.resize(uids.size()) ;
  m_evol_vertex_lids.resize(uids.size()) ;
  std::vector<double> vertices(uids.size()*m_dimensions);
  m_evol_vertex_uid2lid.clear() ;
  for(std::size_t i=0;i<uids.size();++i)
  {
    m_evol_vertex_lids[i] = m_vertex_uid2lid[uids[i]] ;
    m_evol_vertex_ids[i] = i ;
    m_evol_vertex_uid2lid[uids[i]] = i ;
    for (int j = 0; j < m_dimensions; j++)
    {
      vertices[j + m_dimensions*i] = m_vertices[j + m_dimensions*m_evol_vertex_lids[i]];
    }
  }
  m_precice_interface->setMeshVertices(m_mesh_name.localstr(), vertices, m_evol_vertex_ids);

  auto nb_vertices = uids.size() ;
  m_readScalarBuffer.resize(nb_vertices);
  m_writeScalarBuffer.resize(m_write_datas.size());
  for(auto& buffer : m_writeScalarBuffer)
      buffer.resize(nb_vertices) ;
  info()<<"PreCICE::BUFFER SIZE : "<<nb_vertices;
#else
  initMesh() ;
  std::vector<int> lids(uids.size()) ;
  for(std::size_t i=0;i<uids.size();++i)
  {
    lids[i] = m_vertex_uid2lid[uids[i]] ;
  }
  info()<<"PreCICE::setAppMeshFilter : "<<m_mesh_id;
  m_precice_interface->setMeshFilter(m_mesh_id,(Integer)lids.size(),lids.data()) ;
  //m_precice_interface->activateMeshFilter(m_mesh_id,true) ;
  info()<<"After PreCICE::setAppMeshFilter : "<<m_mesh_id;
#endif
}

void
PreCICEService::initData()
{
#ifdef USE_PRECICE_V3
  m_precice_interface->initialize();
#else
  m_precice_interface->initializeData();
#endif
}

void
PreCICEService ::start()
{

#ifdef USE_PRECICE
#ifdef USE_PRECICE_V3
  m_precice_interface->initialize();
  m_initial_dt = m_precice_interface->getMaxTimeStepSize();
#else
  m_initial_dt = m_precice_interface->initialize();
#endif
  m_dt = m_initial_dt ;
#endif
}
bool
PreCICEService::isAppLeader()
{
    return m_app_id==0 ;
}


#ifdef USE_PRECICE_V3
void
PreCICEService::updateMesh()
{
  m_precice_interface->resetMesh(m_mesh_name.localstr()) ;

  m_nb_vertices = ownCells().size() ;
  m_vertices.resize(m_nb_vertices * m_dimensions);
  m_vertex_ids.resize(m_nb_vertices);
  m_vertex_uids.reserve(m_nb_vertices);

  switch(m_vertex_kind)
  {
      case Arcane::IK_Cell:
      {
          int i = 0 ;
          int icount = 0 ;
          ENUMERATE_CELL(icell,ownCells())
          {
              m_vertex_uids.push_back(icell->uniqueId()) ;
              Real3 x;
              ENUMERATE_NODE(inode,icell->nodes())
                {
                  x += m_node_coord[*inode];
                }
              x /= icell->nbNode();
              for (int j = 0; j < m_dimensions; j++)
                {
                  m_vertices[j + m_dimensions*i] = x[j];
                }
              ++i;
          }
      }
      break ;
      default:
          fatal()<<"Not yet implemented Vertex kind : "<<m_vertex_kind;
  }
  for(int lid=0;lid<m_vertex_uids.size();++lid)
  {
      m_vertex_uid2lid[m_vertex_uids[lid]] = lid ;
  }

  m_precice_interface->setMeshVertices(m_mesh_name.localstr(), m_vertices, m_vertex_ids);
}


void
PreCICEService::updateMesh(ICouplingMesh* mesh)
{

  m_precice_interface->resetMesh(m_mesh_name.localstr()) ;

  mesh->initializeCAWF(m_vertices,m_vertex_uids) ;
  m_nb_vertices = m_vertex_uids.size() ;
  for(int lid=0;lid<m_vertex_uids.size();++lid)
  {
      m_vertex_uid2lid[m_vertex_uids[lid]] = lid ;
  }
  m_vertex_ids.resize(m_nb_vertices);

  m_precice_interface->setMeshVertices(m_mesh_name.localstr(), m_vertices, m_vertex_ids);
}
#endif

void
PreCICEService::setAppMeshFilter(Arcane::ConstArrayView<Int64> uids)
{
  info()<<"PreCICEService::setAppMeshFilter";
#ifdef USE_PRECICE_V3
  m_evol_vertex_ids.resize(uids.size()) ;
  m_evol_vertex_lids.resize(uids.size()) ;
  m_evol_vertex_uid2lid.clear() ;
  std::vector<double> vertices(uids.size()*m_dimensions);
  for(std::size_t i=0;i<uids.size();++i)
  {
    m_evol_vertex_ids[i] = i ;
    m_evol_vertex_lids[i] = m_vertex_uid2lid[uids[i]] ;
    m_evol_vertex_uid2lid[uids[i]] = i ;
    for (int j = 0; j < m_dimensions; j++)
    {
      vertices[j + m_dimensions*i] = m_vertices[j + m_dimensions*m_evol_vertex_lids[i]];
    }
  }
  m_precice_interface->resetMesh(m_mesh_name.localstr()) ;
  m_precice_interface->setMeshVertices(m_mesh_name.localstr(), vertices, m_evol_vertex_ids);

  auto nb_vertices = uids.size() ;
  info()<<"ALLOCATE SEND RECV BUFFER : "<<nb_vertices;

  m_readScalarBuffer.resize(nb_vertices);
  m_writeScalarBuffer.resize(m_write_datas.size());
  for(auto& buffer : m_writeScalarBuffer)
      buffer.resize(nb_vertices) ;
#else
  std::vector<int> lids(uids.size()) ;
  for(std::size_t i=0;i<uids.size();++i)
  {
    lids[i] = m_vertex_uid2lid[uids[i]] ;
  }
  info()<<"PreCICE::setAppMeshFilter : "<<m_mesh_id;
  m_precice_interface->setMeshFilter(m_mesh_id,(Integer)lids.size(),lids.data()) ;
  //m_precice_interface->activateMeshFilter(m_mesh_id,true) ;
  info()<<"After PreCICE::setAppMeshFilter : "<<m_mesh_id;
#endif
}

#ifndef USE_PRECICE_V3
void PreCICEService::activateMeshFilter(Arcane::String const& mesh_name, bool value)
{
  int mesh_id = m_precice_interface->getMeshID(std::string(mesh_name.localstr())) ;
  m_precice_interface->activateMeshFilter(mesh_id,value) ;
}
#endif

void
PreCICEService::update()
{
}

void
PreCICEService ::finalize()
{
    if(m_precice_interface.get())
        m_precice_interface->finalize();

    m_is_initialized = false ;
}

void
PreCICEService::startTimeStep()
{
#ifdef USE_PRECICE
     if(m_read_then_write) _markFullFilledWrite();
#ifndef USE_PRECICE_V3
    if (m_precice_interface->isReadDataAvailable())
#endif
    {
        bool update_read_var = options()->updateReadVar() ;
        for(Integer i=0;i<m_read_datas.size();++i)
        {
          auto data_id          = m_read_datas[i].m_id ;
          auto const& data_name = m_read_datas[i].m_name ;
          auto const& var_name  = m_read_datas[i].m_var_name ;
          auto nb_vertices = m_readScalarBuffer.size() ;
          info()<<"RECEIVING VAR NAME : "<<var_name<<" DATA NAME : "<<data_name<<" SIZE : "<<m_readScalarBuffer.size();
          m_readScalarBuffer.assign(nb_vertices,-1) ;
#ifdef USE_PRECICE_V3
          if(m_evolutive_mesh)
            m_precice_interface->readData(m_mesh_name.localstr(),
                                          data_name.localstr(),
                                          m_evol_vertex_ids,
                                          m_dt,
                                          m_readScalarBuffer) ;
          else
            m_precice_interface->readData(m_mesh_name.localstr(),
                                          data_name.localstr(), m_vertex_ids,
                                          m_dt,
                                          m_readScalarBuffer);

#else
          m_precice_interface->readBlockScalarData(data_id, m_nb_vertices, m_vertex_ids.data(), m_readScalarBuffer.data());
#endif
#ifdef CAWF_DEBUG
          {
              //STATS
             Real check_sum = 0. ;
             int icount = 0 ;
             for(auto v : m_readScalarBuffer)
             {
                 info()<<"RBUF["<<icount++<<"]="<<v;
                 check_sum += v ;
             }
             info()<<var_name<<" : SCALARBUFFER["
                                      <<m_readScalarBuffer[0]<<","
                                      <<m_readScalarBuffer[1]<<","
                                      <<m_readScalarBuffer[2]<<","
                                      <<m_readScalarBuffer[3]<<","
                                      <<m_readScalarBuffer[4]<<","
                                      <<"...."
                                      <<m_readScalarBuffer[m_nb_vertices-1]<<"]";
             info()<<"RECV CHECKSUM["<<var_name<<"]"<<check_sum;
          }
#endif
          {
            auto const& item_kind = m_read_datas[i].m_item_kind ;
            switch(item_kind)
            {
              case Arcane::IK_Cell: _updateMeshVariable<Arcane::Cell>(var_name, ownCells().itemFamily()->name(), ownCells());
                break;
              case Arcane::IK_DoF: _updateMeshVariable<Arcane::DualNode>(var_name, mesh::GraphDoFs::dualNodeFamilyName(), GRAPH(mesh())->dualNodeFamily()->allItems().own());
                break;
              default:
                fatal()<<"unknown item kind "<<item_kind;
                }
          }
        }
    }
#endif
    m_current_time_step_is_valid = false ;
}

void
PreCICEService::endTimeStep()
{
#ifdef USE_PRECICE
#ifndef USE_PRECICE_V3
    if (m_precice_interface->isWriteDataRequired(m_dt))
#endif
    {
        for(Integer i=0;i<m_write_datas.size();++i)
        {
        auto const& item_kind = m_write_datas[i].m_item_kind ;
        switch(item_kind)
            {
          case Arcane::IK_Cell: _writeMeshVariable<Arcane::Cell>(i, ownCells().itemFamily()->name(), ownCells());
            break;
          case Arcane::IK_DoF: _writeMeshVariable<Arcane::DualNode>(i, mesh::GraphDoFs::dualNodeFamilyName(), GRAPH(mesh())->dualNodeFamily()->allItems().own());
            break;
         default:
            fatal()<<"unknown item kind "<<item_kind;
            }
        }
    }
#endif
}


Real PreCICEService::initialTimeStep()
{
  return m_initial_dt ;
}

Real PreCICEService::newTimeStep(Real current_dt)
{
#ifdef USE_PRECICE
#ifdef USE_PRECICE_V3
  m_precice_interface->advance(current_dt);
  auto preciceDt = m_precice_interface->getMaxTimeStepSize();
  if(preciceDt>0.)
    m_dt = std::min(preciceDt, current_dt);
  else
    m_dt = current_dt ;
#else
  m_dt = m_precice_interface->advance(current_dt,m_iter_convergence);
  //m_dt = m_precice_interface->advance(current_dt);
#endif
#endif
  return m_dt ;
}

bool PreCICEService::isCouplingOngoing()
{
#ifdef USE_PRECICE
  if(m_precice_interface.get())
    return m_precice_interface->isCouplingOngoing() ;
  else
#endif
    return false ;
}

bool PreCICEService::validateCurrentTimeStep()
{
#ifdef USE_PRECICE
#ifdef USE_PRECICE_V3
  if (m_precice_interface->requiresReadingCheckpoint())
  {
    info() << "DUMMY: Reading iteration checkpoint";
    if(m_state_op)
	m_state_op->reloadOldState() ;
    m_current_time_step_is_valid = false ;
  }
  else
  {
    m_current_time_step_is_valid = true ;
    m_iter = 0 ;
    m_iter_convergence = false ;
  }
#else
  if (m_precice_interface->isActionRequired(precice::constants::actionReadIterationCheckpoint()))
  {
    info() << "DUMMY: Reading iteration checkpoint";
    if(m_state_op)
        m_state_op->reloadOldState() ;
    m_precice_interface->markActionFulfilled(precice::constants::actionReadIterationCheckpoint());
    m_current_time_step_is_valid = false ;
  }
  else
  {
    m_current_time_step_is_valid = true ;
    m_iter = 0 ;
    m_iter_convergence = false ;
  }
#endif
#endif
  info()<<"VALIDATE TIME STEP : "<<m_current_time_step_is_valid;
  return m_current_time_step_is_valid ;
}

void PreCICEService::setConvergence(bool values)
{
#ifdef USE_PRECICE_V3
    if(m_user_convergence_op.get())
        static_cast<UserConvergenceOp*>(m_user_convergence_op.get())->setConvergence(values) ;
#endif
    m_iter_convergence = values ;
}

template<typename ItemType>
void PreCICEService::_updateMeshVariable(const String var_name, const String family_name, const ItemGroup group)
{
  Arcane::MeshVariableScalarRefT<ItemType,Real> var(Arcane::VariableBuildInfo(mesh(), var_name, family_name, IVariable::PPrivate));
  bool update_read_var = options()->updateReadVar() ;
  if(m_evolutive_mesh)
  {
      Real old_sum = 0. ;
      Real new_sum = 0. ;
      ENUMERATE_ITEM(iitem,group)
      {
#ifdef USE_PRECICE_V3
        if (auto search = m_evol_vertex_uid2lid.find(iitem->uniqueId()); search != m_evol_vertex_uid2lid.end())
#else
        if (auto search = m_vertex_uid2lid.find(iitem->uniqueId()); search != m_vertex_uid2lid.end())
#endif
        {
          lid_type lid = search->second ;
          old_sum += var[iitem] ;
          new_sum += m_readScalarBuffer[lid] ;
          if(update_read_var)
              var[iitem] = m_readScalarBuffer[lid] ;
          //if(i==0)
          //info()<<"var["<<lid<<","<<iitem->uniqueId()<<"]"<<var[iitem]<<" "<<m_readScalarBuffer[lid] ;
        }
      }
      info()<<"CHECK["<<var_name<<"] : OLD="<<old_sum<<" NEW="<<new_sum;
  }
  else
  {
      Real old_sum = 0. ;
      Real new_sum = 0. ;
      int lid = 0 ;
      ENUMERATE_ITEM(iitem,group)
      {
          old_sum += var[iitem] ;
          new_sum += m_readScalarBuffer[lid] ;
          if(update_read_var)
              var[iitem] = m_readScalarBuffer[lid] ;
          ++lid ;
          //info()<<"var["<<i<<","<<iitem->uniqueId()<<"]"<<var[iitem] ;
      }
      info()<<"CHECK["<<var_name<<"] : OLD="<<old_sum<<" NEW="<<new_sum;
  }
}

template<typename ItemType>
void PreCICEService::_writeMeshVariable(const Integer i, const String family_name, const ItemGroup group)
{
  auto data_id          = m_write_datas[i].m_id ;
  auto const& data_name = m_write_datas[i].m_name ;
  auto const& var_name  = m_write_datas[i].m_var_name ;
  auto& buffer = m_writeScalarBuffer[i] ;
  auto nb_vertices = buffer.size() ;
  info()<<"SENDING VAR NAME : "<<var_name<<" "<<data_name<<" SIZE : "<<nb_vertices;
  buffer.assign(nb_vertices,std::nan("")) ;
  buffer.assign(nb_vertices,-1.) ;
  Arcane::MeshVariableScalarRefT<ItemType,Real> var(Arcane::VariableBuildInfo(mesh(), var_name, family_name, IVariable::PPrivate));
  if(m_evolutive_mesh)
  {
      info()<<"EVOLUTIVE MESH : "<<group.size()<<" "<<m_nb_vertices;
      Real checksum = 0. ;
      ENUMERATE_ITEM(iitem,group)
      {
#ifdef USE_PRECICE_V3
        if (auto search = m_evol_vertex_uid2lid.find(iitem->uniqueId()); search != m_evol_vertex_uid2lid.end())
#else
        if (auto search = m_vertex_uid2lid.find(iitem->uniqueId()); search != m_vertex_uid2lid.end())
#endif
        {
          lid_type lid = search->second ;
          assert(lid<buffer.size()) ;
          buffer[lid] = var[iitem] ;
          //if(i==0)
          //info()<<"var["<<lid<<","<<iitem->uniqueId()<<"]"<<buffer[lid]<<" "<<var[iitem] ;
          checksum += buffer[lid] ;
        }
      }
      //info()<<"CHECKSUM : "<<checksum;
  }
  else
  {
      info()<<"STATIC MESH : "<<group.size()<<" "<<m_nb_vertices;
      int icount = 0 ;
      ENUMERATE_ITEM(iitem,group)
      {
        //info()<<"var["<<i<<","<<iitem->uniqueId()<<"]"<<var[iitem] ;
          buffer[icount++] = var[iitem] ;
      }
  }
  /*
  {
    int icount = 0 ;
    for(auto v : buffer)
    {
      info()<<"SBUF["<<icount++<<"]="<<v;
    }
  }*/
#ifdef USE_PRECICE_V3
  if(m_evolutive_mesh)
    m_precice_interface->writeData(m_mesh_name.localstr(),
                                   data_name.localstr(),
                                   m_evol_vertex_ids,
                                   buffer) ;
  else
   m_precice_interface->writeData(m_mesh_name.localstr(),
                                  data_name.localstr(),
                                  m_vertex_ids,
                                  buffer);
#else
  m_precice_interface->writeBlockScalarData(data_id, m_nb_vertices, m_vertex_ids.data(), buffer.data());

  if(!m_read_then_write) _markFullFilledWrite();
#endif

}


void PreCICEService::_markFullFilledWrite()
{
#ifdef USE_PRECICE_V3
  if(m_precice_interface->requiresWritingCheckpoint())
  {
      info() << "DUMMY: Writing iteration checkpoint";
      if(m_state_op)
	  m_state_op->saveOldState() ;
  }
#else
  if (m_precice_interface->isActionRequired(precice::constants::actionWriteIterationCheckpoint()))
  {
      info() << "DUMMY: Writing iteration checkpoint";
      if(m_state_op)
          m_state_op->saveOldState() ;
      m_precice_interface->markActionFulfilled(precice::constants::actionWriteIterationCheckpoint());
  }
#endif
}
//==============================================
// L'enregistrement du service
//==============================================
ARCANE_REGISTER_SERVICE_PRECICE(PreCICE,PreCICEService);


