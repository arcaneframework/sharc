// -*- C++ -*-
#ifndef AMCLIGHT_MESH_UTILS_MESHTOOLS_H
#define AMCLIGHT_MESH_UTILS_MESHTOOLS_H




#include <arcane/utils/Array.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/Real3.h>
#include <arcane/IMesh.h>
#include "OrthonormalBoundingBox.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class MeshTools Tools for the mesh
 *
 * The getFaceTriangulation method is imported from OpenFlow's AlgoVisuTools class.
 * The isInCell, isInTriangle and getCenter methods are imported from the OpenFlow's Cell class.
 */
class MeshTools
{
public:

  /** Constructeur de la classe */
  MeshTools(IMesh* mesh)
  : m_nodes_coords(mesh->nodesCoordinates())
  {}

  /** Destructeur de la classe */
  virtual ~MeshTools()
  {}

public:

  Integer getFaceTriangulation(Real seuil, const Real3 &pt0, const Real3 &pt1, const Real3 &pt2, const Real3 &pt3);

  bool isInCell(const Cell& my_cell, Real3 point, Real epsilon);

  bool isInTriangle(const Real3& p, const Real3& pt1, const Real3& pt2, const Real3& pt3);

  Real3 getCenter(const ItemWithNodes& item);

  void setTraceManager(ITraceMng* trace_mng)
  {
    m_debug = true;
    m_trace_mng = trace_mng;
  }

private:
  VariableNodeReal3& m_nodes_coords ;

  bool               m_debug = false;
  ITraceMng*         m_trace_mng = nullptr ;

  Real3 computeVectorNormal(const Real3& p0, const Real3& p1, const Real3& p2);

  int computeCheckInTetra(
      Real value,
      Real val0,
      Real val1,
      Real val2,
      Real val3,
      const Real3& v0,
      const Real3& v1,
      const Real3& v2,
      const Real3& bary,
      const Real3& pt,
      Real epsilon,
      bool is_front_cell,
      bool& is_on_cell_face);

  bool isInTetra(
      Real value,
      Real val0,
      Real val1,
      Real val2,
      Real val3,
      const Real3& v0,
      const Real3& v1,
      const Real3& v2,
      const Real3& bary,
      const Real3& pt,
      Real epsilon,
      bool is_front_cell,
      bool& is_on_cell_face);

  Real crossProduct(const Real3& v0, const Real3& v1, const Real3& v2, const Real3& v3);

};

#endif /* AMCLIGHT_MESH_UTILS_MESHTOOLS_H */
