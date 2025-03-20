#include "MeshCellAccessor.h"

#include <arcane/ItemEnumerator.h>
#include <arcane/utils/FatalErrorException.h>
#include <arcane/VariableTypes.h>



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/**
 * @return the bounding box of the mesh processed by this accessor
 */
OrthonormalBoundingBox MeshCellAccessor::getBoundingBox(bool force_compute)
{
  if (!force_compute && m_b_box_is_computed)
  {
    return m_b_box;
  }

  m_b_box = computeBoundingBox();

  return m_b_box;
}

/**
 * Compute the bounding box of the mesh processed by this accessor
 *
 */
OrthonormalBoundingBox MeshCellAccessor::computeBoundingBox()
{
  m_b_box.reset();
  VariableNodeReal3 & nodes_coords = m_mesh->nodesCoordinates();

  ENUMERATE_NODE(inode,m_mesh->allNodes())
  {
    m_b_box.addPoint(nodes_coords[inode]);
  }

  // Default initialization when the box has not been set properly
  if (m_b_box.getXMin() > m_b_box.getXMax()
      || m_b_box.getYMin() > m_b_box.getYMax()
      || m_b_box.getZMin() > m_b_box.getZMax()) {
    m_b_box.addPoint(Real3(0,0,0));
    m_b_box.addPoint(Real3(1,1,1));
  }
  
  m_b_box_is_computed = true;
  return m_b_box;
}

/**
 * Get the element bounding box found at the elt_idx argument
 *
 * @param elt_idx Index of the element accessed
 * @param[out] Bounding box of the element corresponding to the index
 */
void MeshCellAccessor::getCellBoundingBox(Integer elt_idx, OrthonormalBoundingBox& bbox)
{
  bbox.reset();

  // Index checking
  if (elt_idx >= m_mesh->nbCell() || elt_idx < 0)
  {
    throw FatalErrorException(
        String::format("CellBoundingBox - index of cell out of bounds: {0} (max: {1})", elt_idx, m_mesh->nbCell() - 1));
  }

  const Cell& myCell = m_mesh->meshItemInternalList()->cells.item(elt_idx);
  VariableNodeReal3 & nodes_coords = m_mesh->nodesCoordinates();

  // We build the bounding box
  ENUMERATE_NODE(inode,myCell.nodes())
  {
    bbox.addPoint(nodes_coords[inode]);
  }

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
