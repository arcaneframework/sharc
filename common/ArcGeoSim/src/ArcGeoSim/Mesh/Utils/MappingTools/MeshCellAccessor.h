// -*- C++ -*-
#ifndef AMCLIGHT_MESH_UTILS_MESHCELLACCESSOR_H
#define AMCLIGHT_MESH_UTILS_MESHCELLACCESSOR_H


#include <arcane/IMesh.h>

#include <ArcGeoSim/Mesh/Utils/MappingTools/OrthonormalBoundingBox.h>

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class MeshCellAccessor Accessor to the mesh
 *
 * This is imported from OpenFlow's UnstructuredMesh3DCellAccessor,
 * MeshAccessor3D and AbstractMeshAccessor2 classes.
 */
class MeshCellAccessor
{
public:
  /** Constructeur de la classe */
  MeshCellAccessor(IMesh* mesh) :
      m_mesh(mesh), m_b_box_is_computed(false)
  {
  }

  /** Destructeur de la classe */
  virtual ~MeshCellAccessor()
  {
  }

public:
  OrthonormalBoundingBox getBoundingBox(bool force_compute=false);

  void getCellBoundingBox(Integer elt_idx, OrthonormalBoundingBox& bbox);

  /**
   * @return Number of elements of the mesh accessed by the accessor
   */
  Integer nbElements()
  {
    return m_mesh->nbCell();
  }

  IMesh* getMesh() {
    return m_mesh;
  }

private:
  /** Mesh processed by the accessor */
  IMesh* m_mesh;

  /** Check if the bounding box has been computed */
  bool m_b_box_is_computed;

  /** Bounding box */
  OrthonormalBoundingBox m_b_box;

private:
  OrthonormalBoundingBox computeBoundingBox();

};

#endif /* AMCLIGHT_MESH_UTILS_MESHCELLACCESSOR_H */
