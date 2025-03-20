// -*- C++ -*-
#ifndef ARCGEOSIM_MESH_MESH_POLYLINEDISCRETISATION_POLYLINEDISCRETISATION_H
#define ARCGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINEDISCRETISATION_H


#include <arcane/IMesh.h>

#include "ArcGeoSim/Mesh/AccessorToolsMng/IMeshAccessorToolsMng.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshTools.h"

#include "ArcGeoSim/Mesh/PolylineDiscretisation/Utils/IntersectionSegmentTriangle.h"
#include "ArcGeoSim/Mesh/PolylineDiscretisation/Utils/Polyline.h"
#include "ArcGeoSim/Mesh/PolylineDiscretisation/Utils/Segment.h"
#include "ArcGeoSim/Mesh/PolylineDiscretisation/Utils/Triangle.h"

#include "PolylineDiscretisationData.h"

#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshCellAccessor.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/RegularGrid.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class Polylinediscretisation
 *
 * This is imported from OpenFlow's PolylineDiscretization class.
 */
class PolylineDiscretisation
{
public:

  /** Constructeur de la classe */
  PolylineDiscretisation(ArcGeoSim::IMeshAccessorToolsMng * mng);

  /** Destructeur de la classe */
  ~PolylineDiscretisation()
  {}

public:

  /**
   *
   */
  void init();

  /** update discretiser after mesh changes */
  void update();

  /** run the discretisation of a well defined by a polyline */
  void run(const Polyline & well);

  /** get the discretisation data of the computed well */
  PolylineDiscretisationData getDiscretisationData()
  {
    return m_data;
  }

  void setTraceManager(ITraceMng* trace_mng)
  {
    m_debug = true;
    m_trace_mng = trace_mng;
  }

private:
  ArcGeoSim::IMeshAccessorToolsMng* m_mesh_accessor_tools_mng = nullptr ;
  /** Current Mesh */
  IMesh*            m_mesh  = nullptr;
  MeshTools*        m_tools = nullptr;
  MeshCellAccessor* m_acc   = nullptr;
  RegularGrid*      m_grid  = nullptr;

  /** debug flag for trace purpose */
  bool m_debug = false ;
  /** trace manager */
  ITraceMng* m_trace_mng = nullptr ;

  /** Polyline Discretisation data */
  PolylineDiscretisationData m_data;

  RealSharedArray m_coord_found;

  Segment m_segment_cur;
  Integer m_current_color     = -1;
  Integer m_current_mesh_cell = -1;


  IntersectionSegmentTriangle m_inter;

  Triangle m_triangle;
  Real3 m_pt_inter;


  UniqueArray<Integer> m_intersected_grid_cells;
  UniqueArray<Integer> m_mesh_cell_map;
  UniqueArray<Integer> m_intersected_mesh_cell;

  bool m_initialized = false ;

private:
  void computeIntersectedGridCellWithCurrentSegment();

  void initialize();

  void intersectCellWithSegment(Integer segmentIdx, Integer currentCell);
  void computeIntersection(const Real3& point0, const Real3& point1, const Real3& point2);

  void addIntersectionPoint(Integer segment, Integer Cell, Real coord);
};

#endif /* ARCGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINEDISCRETISATION_H */
