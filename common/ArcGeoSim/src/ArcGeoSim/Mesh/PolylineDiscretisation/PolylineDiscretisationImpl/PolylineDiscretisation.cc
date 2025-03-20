#include "PolylineDiscretisation.h"

#include <arcane/ItemEnumerator.h>

#include <arcane/VariableTypes.h>



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PolylineDiscretisation::PolylineDiscretisation(ArcGeoSim::IMeshAccessorToolsMng* mesh_accessor_tools_mng)
: m_mesh_accessor_tools_mng(mesh_accessor_tools_mng)
{
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void PolylineDiscretisation::
init()
{
  if (m_initialized) return;

  m_tools = m_mesh_accessor_tools_mng->getMeshTools();
  m_acc   = m_mesh_accessor_tools_mng->getMeshAccessor();
  m_grid  = m_mesh_accessor_tools_mng->getRegularGrid();
  m_mesh  = m_acc->getMesh();

  m_mesh_cell_map.resize(m_acc->nbElements());

  m_initialized = true;
}
/*---------------------------------------------------------------------------*/
void PolylineDiscretisation::
update()
{
  OrthonormalBoundingBox bbox = m_acc->getBoundingBox(true);
  m_grid->initBoxes(bbox);
  m_grid->initCellsSizeIsotrope();
  m_grid->insertAll(m_acc->getMesh());
  m_mesh_cell_map.resize(m_acc->nbElements());
}
/*---------------------------------------------------------------------------*/
void PolylineDiscretisation::run(const Polyline & well)
{
  if (m_debug)
  {
    m_inter.setTraceManager(m_trace_mng);
  }
  initialize();

  Integer current_grid_cell;
//  m_coord_found.clear();
  for (Integer segmentCurIdx = 0; segmentCurIdx < well.getNumberOfSegments(); segmentCurIdx++)
  {
    well.getSegment(segmentCurIdx, m_segment_cur);

    m_current_color++;
    addIntersectionPoint(segmentCurIdx, -1, 0);

    computeIntersectedGridCellWithCurrentSegment();
    for (Integer c = 0; c < m_intersected_grid_cells.size(); c++)
    {
      current_grid_cell = m_intersected_grid_cells.at(c);

      m_grid->getElementsFromCell(current_grid_cell, m_intersected_mesh_cell);

      for (IntegerSharedArray::iterator it = m_intersected_mesh_cell.begin(); it != m_intersected_mesh_cell.end(); it++)
      {
        m_current_mesh_cell = *it;

        if (m_mesh_cell_map[m_current_mesh_cell] != m_current_color)
        {
          intersectCellWithSegment(segmentCurIdx, m_current_mesh_cell);
          m_mesh_cell_map[m_current_mesh_cell] = m_current_color;
        }
      }
    }
    addIntersectionPoint(segmentCurIdx, -1, 1);
  }
}

void PolylineDiscretisation::initialize()
{
  m_data.clear();

  m_current_color = 0;

  m_mesh_cell_map.fill(m_current_color);
}

void PolylineDiscretisation::computeIntersectedGridCellWithCurrentSegment()
{
  m_intersected_grid_cells.clear();

  Real3 polylineCellBFL;
  polylineCellBFL.assign(math::min(m_segment_cur.getP1X(), m_segment_cur.getP2X()),
      math::min(m_segment_cur.getP1Y(), m_segment_cur.getP2Y()),
      math::min(m_segment_cur.getP1Z(), m_segment_cur.getP2Z()));

  Real3 polylineCellTBR;

  polylineCellTBR.assign(math::max(m_segment_cur.getP1X(), m_segment_cur.getP2X()),
      math::max(m_segment_cur.getP1Y(), m_segment_cur.getP2Y()),
      math::max(m_segment_cur.getP1Z(), m_segment_cur.getP2Z()));

  IntegerSharedArray polylineIdxCellBFL(3);
  m_grid->getIJK(polylineCellBFL, polylineIdxCellBFL);
  IntegerSharedArray polylineIdxCellTBR(3);
  m_grid->getIJK(polylineCellTBR, polylineIdxCellTBR);

  Integer iInfP = math::min(math::max(polylineIdxCellBFL[0], 0), m_grid->getNbX() - 1);
  Integer iSupP = math::min(math::max(polylineIdxCellTBR[0], 0), m_grid->getNbX() - 1);
  Integer jInfP = math::min(math::max(polylineIdxCellBFL[1], 0), m_grid->getNbY() - 1);
  Integer jSupP = math::min(math::max(polylineIdxCellTBR[1], 0), m_grid->getNbY() - 1);
  Integer kInfP = math::min(math::max(polylineIdxCellBFL[2], 0), m_grid->getNbZ() - 1);
  Integer kSupP = math::min(math::max(polylineIdxCellTBR[2], 0), m_grid->getNbZ() - 1);

  for (Integer k = kInfP; k <= kSupP; k++)
  {
    for (Integer j = jInfP; j <= jSupP; j++)
    {
      for (Integer i = iInfP; i <= iSupP; i++)
      {
        m_intersected_grid_cells.add(m_grid->getIJK(i, j, k));
      }
    }
  }
}

void PolylineDiscretisation::intersectCellWithSegment(Integer segmentIdx, Integer currentCellIdx)
{
  m_coord_found.clear();
  VariableNodeReal3 & nodes_coords = m_mesh->nodesCoordinates();

  Integer triangulation;

  Real3 pt0, pt1, pt2, pt3;

  Real coordonnee;

  Cell current_cell = m_mesh->meshItemInternalList()->cells.item(currentCellIdx);
  ENUMERATE_FACE(iface, current_cell.faces())
  {
    const Integer nb_nodes = iface->nbNode() ;
    if (nb_nodes == 3)
    {
      pt0 = nodes_coords[iface->node(0)];
      pt1 = nodes_coords[iface->node(1)];
      pt2 = nodes_coords[iface->node(2)];

      computeIntersection(pt0, pt1, pt2);
      coordonnee = m_inter.getIntersectionValue(0);
      if (m_inter.getNbIntersection() > 0 && m_coord_found.contains(coordonnee) == false)
      {
        addIntersectionPoint(segmentIdx, m_current_mesh_cell, coordonnee);
      }
    }
    else if (nb_nodes == 4)
    {
      pt0 = nodes_coords[iface->node(0)];
      pt1 = nodes_coords[iface->node(1)];
      pt2 = nodes_coords[iface->node(2)];
      pt3 = nodes_coords[iface->node(3)];

      triangulation = m_tools->getFaceTriangulation(0, pt0, pt1, pt2, pt3);

      if (triangulation == 1)
      {
        computeIntersection(pt0, pt1, pt3);
        coordonnee = m_inter.getIntersectionValue(0);
        if (m_inter.getNbIntersection() > 0 && m_coord_found.contains(coordonnee) == false)
        {
          addIntersectionPoint(segmentIdx, m_current_mesh_cell, coordonnee);
        }

        computeIntersection(pt1, pt2, pt3);
        coordonnee = m_inter.getIntersectionValue(0);
        if (m_inter.getNbIntersection() > 0 && m_coord_found.contains(coordonnee) == false)
        {
          addIntersectionPoint(segmentIdx, m_current_mesh_cell, coordonnee);
        }
      }
      else
      {
        computeIntersection(pt0, pt1, pt2);
        coordonnee = m_inter.getIntersectionValue(0);
        if (m_inter.getNbIntersection() > 0 && m_coord_found.contains(coordonnee) == false)
        {
          addIntersectionPoint(segmentIdx, m_current_mesh_cell, coordonnee);
        }

        computeIntersection(pt2, pt3, pt0);
        coordonnee = m_inter.getIntersectionValue(0);
        if (m_inter.getNbIntersection() > 0 && m_coord_found.contains(coordonnee) == false)
        {
          addIntersectionPoint(segmentIdx, m_current_mesh_cell, coordonnee);
        }
      }
    }
    else if (nb_nodes > 4)
    {
      Real3 fcenter(0,0,0) ;
      SharedArray<Real3> coords(nb_nodes+1) ;

      // compute polygon centroid
      for(Integer i=0;i<nb_nodes;++i)
      {
        coords[i] = nodes_coords[iface->node(i)] ;
        fcenter += coords[i] ;
      }
      // triangular geometric decomposition of the polygon
      fcenter /= nb_nodes ;
      coords[nb_nodes] = coords[0] ;
      for(Integer j=0;j<nb_nodes;++j)
      {
        computeIntersection(coords[j], coords[j+1], fcenter);
        coordonnee = m_inter.getIntersectionValue(0);
        if (m_inter.getNbIntersection() > 0 && m_coord_found.contains(coordonnee) == false)
        {
          addIntersectionPoint(segmentIdx, m_current_mesh_cell, coordonnee);
        }
      }
    }
    else
    {
      throw Arcane::FatalErrorException(A_FUNCINFO,"number of faces not supported");
    }
 }
}

void PolylineDiscretisation::computeIntersection(const Real3& point0, const Real3& point1, const Real3& point2)
{
  m_triangle.setPoints(point0, point1, point2);
  m_inter.compute(m_segment_cur, m_triangle, m_pt_inter);
}

void PolylineDiscretisation::addIntersectionPoint(Integer segment, Integer cell, Real coord)
{
  m_data.add(segment, cell, coord);

  m_coord_found.add(coord);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
