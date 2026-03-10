#pragma once

#include "CAWF/PreCICE/DynamicMeshMng/IDynamicMeshMng.h"

#include "Compaction_axl.h"

using namespace Arcane;

class CompactionModule
: public ArcaneCompactionObject
, public ArcGeoSim::ICAWFMng::ITimeIterStateOp
{
 public:
  typedef Alien::ArcaneTools::BasicIndexManager IndexMangerType ;
  typedef std::unique_ptr<IndexMangerType>      IndexMangerPtrType ;
  typedef Alien::ArcaneTools::Space             SpaceType ;
  typedef Alien::ArcaneTools::IIndexManager::ScalarIndexSet  ScalarIndexSetType ;

  //! Constructor
  CompactionModule(const Arcane::ModuleBuildInfo & mbi)
    : ArcaneCompactionObject(mbi){}

  //! Destructor
  virtual ~CompactionModule() {};

public:
  //! Initialization
  void init();
  //! Run the test
  void test();
  void computePressure();

  void saveOldState() ;
  void reloadOldState() ;

private :

  ShArc::IDynamicMeshMng* m_dynamic_mesh_mng = nullptr;
  ArcGeoSim::ICAWFMng*    m_cawf_mng         = nullptr ;

  Real          m_dt = 0. ;
  Real          m_time = 0. ;
  int           m_iter = 0 ;
  int           m_max_iter = 0 ;

  IParallelMng* m_parallel_mng = nullptr;

  Real                           m_gravity               = 9.8;
  Real                           m_top_z                 = 0. ;
  Real                           m_event_period          = 0. ;
  Real                           m_next_event_time       = 0. ;
  Integer                        m_event_index           = 0 ;
  Integer                        m_new_layer_id          = 0 ;

  UniqueArray<Arcane::CellGroup> m_layers ;
  Real                           m_layer_event_period    = 0. ;
  Real                           m_layer_event_time      = 0. ;
  Real                           m_next_layer_event_time = 0. ;
  UniqueArray<Int64>             m_activated_layers_uids;

  IGeometryMng *                 m_geometry_mng          = nullptr;
  NoOptimizationGeometryPolicy   m_geometry_policy ;
  Alien::ILinearSolver *         m_linear_solver         = nullptr;
  /*
  ItemGroup                 m_areaU ;
  CellCellGroup             m_cell_cell_connection;
  CellCellGroup             m_all_cell_cell_connection;

  Integer                   m_localSize  = 0;
  Integer                   m_globalSize = 0;
  Alien::MatrixDistribution m_mdist;
  Alien::VectorDistribution m_vdist;
  IndexMangerPtrType        m_index_manager;
  ScalarIndexSetType        m_indexSetU;
  SpaceType                 m_space;
  Alien::Vector             m_vectorB;
  Alien::Vector             m_vectorBB;
  Alien::Vector             m_vectorX;
  Alien::Matrix             m_matrixA ;*/


  Alien::UniqueArray<Integer>  m_allUIndex;



};

