#include "RegularGrid.h"

#include <arcane/utils/Array2.h>
#include <arcane/utils/FatalErrorException.h>

#include <boost/range/algorithm.hpp>
#include <arcane/ItemTypes.h>
#include <arcane/ItemGroup.h>
#include <arcane/ItemEnumerator.h>
#include <set>

Integer const RegularGrid::m_default_nb_elts_per_cell       = 6;
Real const    RegularGrid::m_default_epsilon                = 1e-6;
Real const    RegularGrid::m_default_threshold_augmentation = 0.02;

/*---------------------------------------------------------------------------*/
/**
 * Basic constructor. Build a grid using an accessor, the number of cells on each axis and the
 * geometry bounding box. The constructor initializes the different parameter of the grid
 * without allocating the multimap. So after calling the constructor, the grid is very light in
 * memory.
 *
 * @param acc Mesh accessor of the current geometry
 * @param nx Number of cells on the X axis
 * @param ny Number of cells on the Y axis
 * @param nz Number of cells on the Z axis
 * @param box Bounding box of the current geometry
 */
RegularGrid::RegularGrid(MeshCellAccessor* acc,
                         Integer nx,
                         Integer ny,
                         Integer nz,
                         IBoundingBox& box,
                         Integer nb_elts_per_cell,
                         Real epsilon,
                         Real threshold_augmentation)
 : m_epsilon(epsilon)
 , m_threshold_augmentation(threshold_augmentation)
 , m_nb_elts_per_cell(nb_elts_per_cell)
{
  m_mesh_cell_acc = acc;

  // Retrieves the bounding box of the geometry and the increased bounding box
  initBoxes(box);

  // Computes the different length parameters of the grid (deltaX, xmin, xmax ...)
  initCellsSize(nx, ny, nz, RegularGrid::BASIC, m_increased_box);

  // Computes the number of cells in the grid
  m_nb_of_cells = m_nb_x * m_nb_y * m_nb_z;

  m_loaded = false;
}

/**
 * This constructor allows the user to specify the number of elements per cell in the case of an
 * isotropic grid.
 *
 * @param acc Mesh accessor of the current geometry
 * @param gt The grid type
 * @param box Bounding box of the current geometry
 * @param nb_elts_per_cell Number of elements per cell wanted
 */
RegularGrid::RegularGrid(MeshCellAccessor* acc,
                         GridType gt,
                         IBoundingBox& box,
                         Integer nb_elts_per_cell,
                         Real epsilon,
                         Real threshold_augmentation)
: m_epsilon(epsilon)
, m_threshold_augmentation(threshold_augmentation)

{
  m_mesh_cell_acc = acc;

  // Specify the number of elements per cell
  if (nb_elts_per_cell > 0)
  {
    m_nb_elts_per_cell = nb_elts_per_cell;
  }

  // Retrieve the bounding box of the geometry and the increased bounding box
  initBoxes(box);
  // If the user call this method to create a basic grid without specifying the number of
  // cells, a default anisotropic grid will be create
  if (gt == RegularGrid::BASIC)
  {
    throw FatalErrorException(String::format("RegularGrid::BASIC - GridType not implemented."));
  }
  else
  {
    // Compute the different length parameters of the grid (deltaX, xmin, xmax ...)
    initCellsSize(0, 0, 0, gt, m_increased_box);
  }

  // Computes the number of cells in the grid
  m_nb_of_cells = m_nb_x * m_nb_y * m_nb_z;

  m_loaded = false;
}

/**
 * Retrieve the geometry bounding box and computes the increased bounding box (2% more than the
 * original)
 *
 * @param box Bounding box of the geometry
 */
void RegularGrid::initBoxes(IBoundingBox& box)
{
  m_b_box.set(box);
  m_increased_box.set(box);
  m_increased_box.increaseBBox(m_threshold_augmentation, m_epsilon);
}

/**
 * Compute the different parameters of the regular grid (isotropic or anisotropic case)
 *
 * @param nx Number of cells on the X axis
 * @param ny Number of cells on the Y axis
 * @param nz Number of cells on the Z axis
 * @param gt Grid type
 * @param box Bounding box of the current geometry
 */
void RegularGrid::initCellsSize(int nx, int ny, int nz, GridType gt, const OrthonormalBoundingBox& box)
{

  switch (gt)
  {
    case ANISOTROPE:
      throw FatalErrorException("RegularGrid::initCellsSize - GridType ANISOTROPE not implemented.");
      break;
    case ANISOTROPE_Z_1:
      throw FatalErrorException("RegularGrid::initCellsSize - GridType ANISOTROPE_Z_1 not implemented.");
      break;
    case ISOTROPE:
      initCellsSizeIsotrope();
      break;
    case BASIC:
      initCellsSizeBasic(nx, ny, nz, box);
      break;
  }
}

/**
 * Compute the dimensions of an isotropic grid
 */
void RegularGrid::initCellsSizeIsotrope()
{
  // Isotropic case, first we compute the common interval between each cell (same on the
  // X, Y and Z axis)
  Real delta = computeDimensionsIsoGrid();
  // Second, we compute the other parameters
  computeCellsSizeIso(delta);
}

/**
 * Compute the dimensions of a basic grid defined by the number of cells along each axis
 *
 * @param nx Number of cells on the X axis
 * @param ny Number of cells on the Y axis
 * @param nz Number of cells on the Z axis
 * @param box Bounding box of the current geometry
 */
void RegularGrid::initCellsSizeBasic(int nx, int ny, int nz,const OrthonormalBoundingBox& box)
{
  m_nb_x = nx;
  m_nb_y = ny;
  m_nb_z = nz;

  // Made to avoid accuracy issues
  m_b_box.setXMax(box.getXMax() + m_epsilon);
  m_b_box.setYMax(box.getYMax() + m_epsilon);
  m_b_box.setZMax(box.getZMax() + m_epsilon);

  m_b_box.setXMin(box.getXMin() - m_epsilon);
  m_b_box.setYMin(box.getYMin() - m_epsilon);
  m_b_box.setZMin(box.getZMin() - m_epsilon);

  computeDelta();

  setCoordinateLimits();
}

/**
 * Compute the common delta of an isotropic grid depending of the geometry size and the number
 * of elements per cell
 */
Real RegularGrid::computeDimensionsIsoGrid()
{
  /** Number of elements from the geometry contained inside the grid */
  Integer nb_of_elements = m_mesh_cell_acc->nbElements();

  // Compute the grid threshold (number of elements per cell)
  Integer M = computeThreshold(nb_of_elements);

  Real x = m_b_box.getXMax() - m_b_box.getXMin();
  Real y = m_b_box.getYMax() - m_b_box.getYMin();
  Real z = m_b_box.getZMax() - m_b_box.getZMin();

  RealSharedArray tmp;
  tmp.resize(3);
  tmp[0] = x;
  tmp[1] = y;
  tmp[2] = z;
  boost::sort(tmp);

  Real a = tmp[0], b = tmp[1], c = tmp[2];

  if (a * a * M < b * c)
  {
    a = math::sqrt((b * c) / M);

    if (b * b * M < a * c)
    {
      a = c / M;
    }
  }

  x = math::max(x, a);
  y = math::max(y, a);
  z = math::max(z, a);

  Real d = math::pow(M / (x * y * z), 1. / 3.);

  Real di = x * d;
  Real dj = y * d;
  Real dk = z * d;

  m_nb_x = clamp(1, M, (Integer) di);
  m_nb_y = clamp(1, M, (Integer) dj);
  m_nb_z = clamp(1, M, (Integer) dk);

  Real delta = math::max(x / m_nb_x, math::max(y / m_nb_y, z / m_nb_z));

  return delta;
}

/**
 * Clamp a value between a min and a max boundaries
 *
 * @param min Minimum value
 * @param max Maximum value
 * @param val Value to be clamped
 *
 * @return The clamped value
 */
Integer RegularGrid::clamp(Integer min, Integer max, Integer val)
{
  return math::min(math::max(val, min), max);
}

/**
 * Compute the cell parameters of an isotropic grid using its length interval
 *
 * @param delta Space between each cell
 */
void RegularGrid::computeCellsSizeIso(Real delta)
{

  // Compute the center of the geometry
  Real midX = m_b_box.getXMax() + m_b_box.getXMin();
  Real midY = m_b_box.getYMax() + m_b_box.getYMin();
  Real midZ = m_b_box.getZMax() + m_b_box.getZMin();

  Real3 center(midX, midY, midZ);

  center *= 0.5;

  Real3 min;
  Real3 max;
  Real3 diag(m_nb_x * delta, m_nb_y * delta, m_nb_z * delta);
  diag *= 0.5;

  // Compute the bounding box of the grid
  {
    min.assign(center);
    max.assign(center);

    min.sub(diag);
    max.add(diag);

    m_b_box.reset();

    m_b_box.addPoint(min);
    m_b_box.addPoint(max);
  }

  {
    Real3 epsilon(m_epsilon, m_epsilon, m_epsilon);

    min.assign(center);
    max.assign(center);

    min.sub(diag);
    max.add(diag);

    min.add(epsilon);
    max.sub(epsilon);

    m_increased_box.reset();

    m_increased_box.addPoint(min);
    m_increased_box.addPoint(max);

  }

  computeDelta();

  setCoordinateLimits();
}

/**
 * Compute the coordinates of the cell containing a point P
 *
 * @param P a random point represented as a Tuple3d
 * @param[out] tab an int array storing the cell coordinates
 */
void RegularGrid::getIJK(const Real3& P, ArrayView<Integer> tab) const
{
  Integer i = (Integer) ((P.x - m_x_min) * m_inv_delta_x);
  Integer j = (Integer) ((P.y - m_y_min) * m_inv_delta_y);
  Integer k = (Integer) ((P.z - m_z_min) * m_inv_delta_z);
  tab[0] = math::min(m_nb_x-1,math::max(0,i)) ;
  tab[1] = math::min(m_nb_y-1,math::max(0,j)) ;
  tab[2] = math::min(m_nb_z-1,math::max(0,k)) ;
}

/**
 * Compute the cell number relative to its coordinate in the grid
 *
 * @param i Coordinate of the cell on the X axis
 * @param j Coordinate of the cell on the Y axis
 * @param k Coordinate of the cell on the Z axis
 *
 * @return an Integer representing the cell number
 */
Integer RegularGrid::getIJK(Integer i, Integer j, Integer k) const
{
  if (i < 0 || i >= m_nb_x)
  {
    throw FatalErrorException(String::format("RegularGrid - I out of range: {0} (max: {1})", i, m_nb_x - 1));
  }

  if (j < 0 || j >= m_nb_y)
  {
    throw FatalErrorException(String::format("RegularGrid - J out of range: {0} (max: {1})", j, m_nb_y - 1));
  }

  if (k < 0 || k >= m_nb_z)
  {
    throw FatalErrorException(String::format("RegularGrid - K out of range: {0} (max: {1})", k, m_nb_z - 1));
  }

  return (k * m_nb_y + j) * m_nb_x + i;
}

/**
 * Retrieve the different element indexes from a cell
 *
 * @param i The cell index where we are looking for elements
 * @param[out] it Empty if the cell is empty or an Array of integer containing the list of the different
 *         elements indexes found in the cell
 */
void RegularGrid::getElementsFromCell(Integer i, UniqueArray<Integer>& it) const
{
  it.clear();
  std::map<Integer, IntegerSharedArray>::const_iterator cell_to_elt = m_cell_to_elt_map.find(i);
  if (cell_to_elt != m_cell_to_elt_map.end())
  {
    it.addRange(cell_to_elt->second);
  }
}
void RegularGrid::getCellIndexesFromBox(const Real3& minCorner,
                                        const Real3& maxCorner,
                                        UniqueArray<Integer>& indexes) const
{
  UniqueArray<Integer> minIJK(3) ;
  UniqueArray<Integer> maxIJK(3) ;
  getIJK(minCorner,minIJK) ;
  getIJK(maxCorner,maxIJK) ;
  Integer ni = maxIJK[0]-minIJK[0]+1 ;
  Integer nj = maxIJK[1]-minIJK[1]+1 ;
  Integer nk = maxIJK[2]-minIJK[2]+1 ;
  Integer nb_cells = ni*nj*nk;
  indexes.clear() ;
  indexes.reserve(nb_cells) ;
  for(Integer k=minIJK[2];k<maxIJK[2]+1;++k)
    for(Integer j=minIJK[1];j<maxIJK[1]+1;++j)
      for(Integer i=minIJK[0];i<maxIJK[0]+1;++i)
        indexes.add(getIJK(i,j,k)) ;
}

void
RegularGrid::getElementsFromCellIndexes(ConstArrayView<Integer> indexes,UniqueArray<Integer>& lids) const
{
  // WARNING AN ELEMENT CAN BELONGS TO SEVERAL CELLS : NEED TO CHECK THAT THEY ARE ALREADY INSERTED
  std::set<Integer> lid_set ;
  for(Integer i=0;i<indexes.size();++i)
  {
    std::map<Integer, IntegerSharedArray>::const_iterator cell_to_elt = m_cell_to_elt_map.find(indexes[i]);
    if (cell_to_elt != m_cell_to_elt_map.end())
    {
      for( auto lid : cell_to_elt->second )
        lid_set.insert(lid) ;
    }
  }
  for(auto lid : lid_set )
    lids.add(lid) ;
}

/**
 * Allocate the different structures used to index the elements. Insert each element of the
 * geometry into its corresponding cell. (Faster method)
 */
void RegularGrid::insertAll(IMesh* mesh)
{
  Integer index = 0;
  // Min boundary of the element bounding boxes
  Real3 MinBound;
  // Max boundary of the element bounding boxes
  Real3 MaxBound;
  // Temporary array used to store cell coordinates
  UniqueArray<Integer> tab(3);

  /** Number of elements from the geometry contained inside the grid */
  Integer nb_of_elements = m_mesh_cell_acc->nbElements();

  // Matrix storing the coordinates of the left front lower cell and the right bottom top cell
  IntegerSharedArray2 bbox_coords(6, nb_of_elements);

  // MultiMap linking elements and cells, core of the grid
//  cellToEltMap = new SoftMultiMap();

  // Temporary bounding box used to store the bbox of the mesh elements
  OrthonormalBoundingBox tempEltBox;

  // For an element, find the cells crossing its bounding box, storing the two extreme ones,
  // and increasing the number of element per cell
  m_cell_to_elt_map.clear();
  ENUMERATE_CELL(icell,mesh->allActiveCells())
  //for (int nb_elt = 0; nb_elt < nb_of_elements; nb_elt++)
  {
	  Cell cell=*icell;
	  const Int32 cell_lid= cell.localId();

    // Reset the temporary bounding box
    tempEltBox.reset();
    // Get the bounding box of the current element
    // ****THIS IS THE HEAVY COMPUTATION OF THE REGULAR GRID****
    m_mesh_cell_acc->getCellBoundingBox(cell_lid, tempEltBox);

    //TODO PAS BESOIN?
//    if (!check(tempEltBox))
//    {
//      continue;
//    }

    // Increase this bounding box to avoid accuracy issues
    tempEltBox.increaseBBox(0.01, m_epsilon);
    // Get the min and max boundaries of the bbox
    MinBound.assign(tempEltBox.getXMin(), tempEltBox.getYMin(), tempEltBox.getZMin());
    MaxBound.assign(tempEltBox.getXMax(), tempEltBox.getYMax(), tempEltBox.getZMax());
    // Find the cells around the element bounding box
    getIJK(MinBound, tab);
    bbox_coords[0][cell_lid] = math::max(tab[0], 0);
    bbox_coords[1][cell_lid] = math::max(tab[1], 0);
    bbox_coords[2][cell_lid] = math::max(tab[2], 0);
    getIJK(MaxBound, tab);
    bbox_coords[3][cell_lid] = math::min(tab[0] + 1, getNbX());
    bbox_coords[4][cell_lid] = math::min(tab[1] + 1, getNbY());
    bbox_coords[5][cell_lid] = math::min(tab[2] + 1, getNbZ());
    // For each cell containing the element, we increase the number of elements found inside
    // this cell and we increase the general element per cell counter
    for (Integer i = bbox_coords[0][cell_lid]; i < bbox_coords[3][cell_lid]; i++)
    {
      for (Integer j = bbox_coords[1][cell_lid]; j < bbox_coords[4][cell_lid]; j++)
      {
        for (Integer k = bbox_coords[2][cell_lid]; k < bbox_coords[5][cell_lid]; k++)
        {
          index = getIJK(i, j, k);
          m_cell_to_elt_map[index].add(cell_lid);
        }
      }
    }
  }

  // Specify that the grid has been filled
  m_loaded = true;
}

/**
 * Compute the number of cells of the grid depending of the number of elements per cell (used
 * for isotropic case)
 *
 * @param n Number of elements of the geometry
 * @return The number of cells built in the isotropic grid
 */
Integer RegularGrid::computeThreshold(Integer n)
{

  n /= m_nb_elts_per_cell;
  if (n == 0)
  {
    n = 5;
  }
  return n;
}

void RegularGrid::setCoordinateLimits()
{
// Compute the coordinate limits of the grid
  m_x_min = m_b_box.getXMin();
  m_y_min = m_b_box.getYMin();
  m_z_min = m_b_box.getZMin();

  m_x_max = m_b_box.getXMax();
  m_y_max = m_b_box.getYMax();
  m_z_max = m_b_box.getZMax();
}

void RegularGrid::computeDelta()
{
  // Compute delta and its invert on each axis
  m_delta_x = (m_b_box.getXMax() - m_b_box.getXMin()) / m_nb_x;
  m_delta_y = (m_b_box.getYMax() - m_b_box.getYMin()) / m_nb_y;
  m_delta_z = (m_b_box.getZMax() - m_b_box.getZMin()) / m_nb_z;

  if (m_delta_x < 1e-5)
  {
    m_inv_delta_x = 0.;
  }
  else
  {
    m_inv_delta_x = 1.0 / m_delta_x;
  }
  if (m_delta_y < 1e-5)
  {
    m_inv_delta_y = 0.;
  }
  else
  {
    m_inv_delta_y = 1.0 / m_delta_y;
  }
  if (m_delta_z < 1e-5)
  {
    m_inv_delta_y = 0.;
  }
  else
  {
    m_inv_delta_z = 1.0 / m_delta_z;
  }

}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
