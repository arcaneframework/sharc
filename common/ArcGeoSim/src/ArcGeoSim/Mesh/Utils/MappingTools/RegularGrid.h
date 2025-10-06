// -*- C++ -*-
#ifndef AMCLIGHT_MESH_UTILS_REGULARGRID_H
#define AMCLIGHT_MESH_UTILS_REGULARGRID_H


#include <arcane/IMesh.h>
#include <arcane/utils/Array.h>

#include <ArcGeoSim/Mesh/Utils/MappingTools/OrthonormalBoundingBox.h>
#include <ArcGeoSim/Mesh/Utils/MappingTools/MeshCellAccessor.h>

#include <map>

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class RegularGrid: Class used to build regular grid.
 * The regular grid is represented by two groups of attributes, the dimensions ( number of
 * cells along each axis, space between each cell, bounding box) and the repartition of the elements
 * in the cells. The two different constructors are used to either build a
 * random grid with the number of cells given or a specific grid
 * Each constructor calls a sequence of initialization methods to compute the
 * dimensions. The insertion of the elements is managed by the public method InsertAll allowing a
 * lazy loading of the grid (heavy structure).
 *
 * This is imported from OpenFlow's RegularGrid2 class.
 */
class RegularGrid
{
public:
  /** Type of the grid */
  enum GridType
  {
    BASIC, /**< Regular grid with the number of cells chosen by the user */
    ISOTROPE, /**< Isotropic grid, equal delta along each axis */
    ANISOTROPE, /**< Anisotropic grid, equal number of cells along each axis */
    ANISOTROPE_Z_1 /**< Anisotropic grid, equal number of cells along axis X and Y */
  };

  typedef int IJKType[3] ;

  static Integer const m_default_nb_elts_per_cell ;
  static Real    const m_default_epsilon ;
  static Real    const m_default_threshold_augmentation ;

public:

  /** Constructeur de la classe avec specification du nombre de blocs en X/Y/Z */
  RegularGrid(MeshCellAccessor* acc,
              Integer nx, Integer ny, Integer nz,
              IBoundingBox& box,
              Integer nb_elts_per_cell = m_default_nb_elts_per_cell,
              Real epsilon=m_default_epsilon,
              Real threshold_augmentation=m_default_threshold_augmentation);

  /** Constructeur de la classe avec specification du nombre d'elements par cellule */
  RegularGrid(MeshCellAccessor* acc,
              GridType gt,
              IBoundingBox& box,
              Integer nb_elts_per_cell,
              Real epsilon=m_default_epsilon,
              Real threshold_augmentation=m_default_threshold_augmentation);

  /** Destructeur de la classe */
  ~RegularGrid()
  {
  }

  void insertAll(IMesh *mesh);

  /** is the regular grid loaded? */
  bool isLoaded()
  {
    return m_loaded;
  }

  /** get the number of cells along the X-axis */
  Integer getNbX()
  {
    return m_nb_x;
  }

  /** get the number of cells along the Y-axis */
  Integer getNbY()
  {
    return m_nb_y;
  }

  /** get the number of cells along the Z-axis */
  Integer getNbZ()
  {
    return m_nb_z;
  }

  /** get the map associating a cell of the grid with elements of the mesh */
  std::map<Integer, IntegerSharedArray> const & getMap() const
  {
    return m_cell_to_elt_map;
  }

  void getIJK(const Real3& P, ArrayView<Integer> tab) const;
  Integer getIJK(Integer i, Integer j, Integer k) const;

  void getCellIndexesFromBox(const Real3& minCorner, const Real3& maxCorner, UniqueArray<Integer>& indexes) const;

  void getElementsFromCellIndexes(ConstArrayView<Integer> indexes, UniqueArray<Integer>& lids) const ;

  void getElementsFromCell(Integer i, UniqueArray<Integer>& it) const;

  void initBoxes(IBoundingBox& box);
  void initCellsSize(int nx, int ny, int nz, GridType gt, const OrthonormalBoundingBox& box);
  void initCellsSizeIsotrope();
  void initCellsSizeBasic(int nx, int ny, int nz, const OrthonormalBoundingBox& box);

  IBoundingBox const& getInitialBoundinBox() const {
    return m_b_box;
  }

  IBoundingBox const& getIncreasedBoundingBox() const {
    return m_increased_box;
  }

private:
  /** Mesh Cell accessor */
  MeshCellAccessor* m_mesh_cell_acc;

  /** Number of cells on the X/Y/Z axis */
  Integer m_nb_x, m_nb_y, m_nb_z;

  /** Space between each cell on the X/Y/Z axis */
  Real m_delta_x, m_delta_y, m_delta_z;

  /** X/Y/Z minimum (of the Regular Grid) */
  Real m_x_min, m_y_min, m_z_min;

  /** X/Y/Z maximum (of the Regular Grid) */
  Real m_x_max, m_y_max, m_z_max;

  /** Invert of the space between each cell on the X/Y/Z axis */
  Real m_inv_delta_x, m_inv_delta_y, m_inv_delta_z;

  /** Number of cells of the regular grid */
  Integer m_nb_of_cells;

  /** Bounding box of the current regular grid */
  OrthonormalBoundingBox m_b_box;

  /** Increased bounding box made in order to contain each element of the initial geometry */
  OrthonormalBoundingBox m_increased_box;

  /** Map indexing the elements found inside the cells */
  std::map<Integer, IntegerSharedArray> m_cell_to_elt_map;

  /** Check if the multimap is filled (useful to lazy load the regulargrid) */
  bool m_loaded;

  /** Epsilon constant used to avoid accuracy issues */
  Real m_epsilon;

  /** Constant used to increase the bounding box (2% of the original) to be sure to catch all the elements */
  Real m_threshold_augmentation;

  /** Number of element per cell (in the isotropic case) */
  Integer m_nb_elts_per_cell;

private:
  Real computeDimensionsIsoGrid();

  Integer clamp(Integer min, Integer max, Integer val);
  void computeCellsSizeIso(Real delta);

  Integer computeThreshold(Integer n);

  void setCoordinateLimits();
  void computeDelta();
};

#endif /* AMCLIGHT_MESH_UTILS_REGULARGRID_H */
