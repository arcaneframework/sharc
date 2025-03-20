#include "MeshTools.h"

#include <arcane/MathUtils.h>
#include <arcane/ItemEnumerator.h>
#include <arcane/VariableTypes.h>


/*---------------------------------------------------------------------------*/
Integer MeshTools::getFaceTriangulation(
    Real seuil,
    const Real3 &pt0,
    const Real3 &pt1,
    const Real3 &pt2,
    const Real3 &pt3)
{
  // Calcul vecteurs normaux normalises
  Real3 n1 = computeVectorNormal(pt0, pt1, pt3);
  Real3 n2 = computeVectorNormal(pt2, pt3, pt1);
  Real3 n3 = computeVectorNormal(pt1, pt2, pt0);
  Real3 n4 = computeVectorNormal(pt3, pt0, pt2);

  // Calcul produit scalaires
  Real scalar1 = math::abs(math::dot(n1,n2));
  Real scalar2 = math::abs(math::dot(n3,n4));

  // Calcul angle minimal
  Real scalar_max = math::max(scalar1, scalar2);

  Real triangulation ;
  if (seuil > (1 - scalar_max))
  {
    triangulation = 0;
  }
  else
  {
    if (scalar_max == scalar1)
    {
      triangulation = 1;
    }
    else
    {
      triangulation = 2;
    }
  }

  return triangulation;
}

/*---------------------------------------------------------------------------*/
Real3 MeshTools::computeVectorNormal(const Real3& p0, const Real3& p1, const Real3& p2)
{
  Real3 tmp1 = p1 - p0 ;
  Real3 tmp2 = p2 - p0 ;

  Real3 v = math::crossProduct3(tmp1,tmp2) ;
  v.normalize();
  return v;
}

/**
 * Check if the point is included in the cell
 *
 * @param mesh the current mesh
 * @param my_cell
 * @param point a point
 *
 * @return true if the point is included in the cell
 */
bool MeshTools::isInCell(const Cell& my_cell, Real3 point, Real epsilon)
{
  if (m_debug)
  {
    m_trace_mng->info() << "Recherche de " << point << " dans la cellule " << my_cell.localId();
  }

  Real3 barycenter = getCenter(my_cell);

  Integer valMin, indexMin = 0, res = 0;

  bool is_in_a_good_tetra = false;
  Real value = 0, val0 = 0, val1 = 0, val2 = 0, val3 = 0;

  Integer triangulation;

  bool is_on_cell_face = false;

  Cell front_cell;
  ENUMERATE_FACE(iface,my_cell.faces())
  {
    front_cell = iface->frontCell();

    bool is_front_cell;
    if (front_cell == my_cell)
    {
      is_front_cell = true;
      if (m_debug)
      {
        m_trace_mng->info() << "FRONT CELL == TRUE";
      }
    }
    else
    {
      is_front_cell = false;
    }

    // On regarde comment trianguler la face courante
    if (iface->nbNode() == 3)
    {
      triangulation = 0;
    }
    else
    {
      valMin = iface->node(0).localId();
      indexMin = 0;
      for (int j = 1; j < iface->nbNode(); j++)
      {
        if (valMin > iface->node(j).localId())
        {
          valMin = iface->node(j).localId();
          indexMin = j;
        }
      }
      triangulation = (indexMin % 2) + 1;
    }

    // On regarde les tetras formées à partir de cette face
    if (triangulation == 0 || triangulation == 1)
    {
      res = computeCheckInTetra(value, val0, val1, val2, val3,
                                m_nodes_coords[iface->node(0)],
                                m_nodes_coords[iface->node(1)],
                                m_nodes_coords[iface->node(2)],
                                barycenter,
                                point,epsilon,
                                is_front_cell,
                                is_on_cell_face);

      if (m_debug)
      {
        m_trace_mng->info() << "Res 1: " << res;
      }

      if (triangulation == 1 && res <= 0)
      {
        //m_v_tmp1.assign(nodes_coords[iface->node(0)]);
        //m_v_tmp2.assign(nodes_coords[iface->node(2)]);
        //m_v_tmp3.assign(nodes_coords[iface->node(3)]);

        res = computeCheckInTetra(value, val0, val1, val2, val3,
                                  m_nodes_coords[iface->node(0)],
                                  m_nodes_coords[iface->node(2)],
                                  m_nodes_coords[iface->node(3)],
                                  barycenter, point,
                                  epsilon,
                                  is_front_cell,
                                  is_on_cell_face);
        if (m_debug)
        {
          m_trace_mng->info() << "Res 2: " << res;
          if (res == -2)
          {
            m_trace_mng->info() << "node 1: " << m_nodes_coords[iface->node(0)];
            m_trace_mng->info() << "node 2: " << m_nodes_coords[iface->node(2)];
            m_trace_mng->info() << "node 3: " << m_nodes_coords[iface->node(3)];
            m_trace_mng->info() << "barycenter: " << barycenter;
          }
        }

      }
    } // fin triangulation == 0 ou triangulation == 1
    else
    { // triangulation == 2
      //m_v_tmp1.assign(nodes_coords[iface->node(1)]);
      //m_v_tmp2.assign(nodes_coords[iface->node(2)]);
      //m_v_tmp3.assign(nodes_coords[iface->node(3)]);

      res = computeCheckInTetra(value, val0, val1, val2, val3,
                                m_nodes_coords[iface->node(1)],
                                m_nodes_coords[iface->node(2)],
                                m_nodes_coords[iface->node(3)],
                                barycenter, point,
                                epsilon,
                                is_front_cell,
                                is_on_cell_face);

      if (m_debug)
      {
        m_trace_mng->info() << "Res 3: " << res;
      }

      if (res <= 0)
      {
        //m_v_tmp1.assign(nodes_coords[iface->node(0)]);
        //m_v_tmp2.assign(nodes_coords[iface->node(1)]);
        //m_v_tmp3.assign(nodes_coords[iface->node(3)]);

        res = computeCheckInTetra(value, val0, val1, val2, val3,
                                  m_nodes_coords[iface->node(0)],
                                  m_nodes_coords[iface->node(1)],
                                  m_nodes_coords[iface->node(3)],
                                  barycenter, point,
                                  epsilon,
                                  is_front_cell,
                                  is_on_cell_face);

        if (m_debug)
        {
          m_trace_mng->info() << "Res 4: " << res;
        }

      }
    } // fin triangulation == 2

    if (res < 0)
    {
      if (m_debug)
      {
        m_trace_mng->info() << "res < 0";
      }

      return false;
    }
    if (res > 0)
    {
      if (m_debug)
      {
        m_trace_mng->info() << "res = " << res;
      }
      is_in_a_good_tetra = true;
    }
    if (is_on_cell_face)
    {
      if (m_debug)
      {
        m_trace_mng->info() << "on cell face";
      }

      return true;
    }
  }

  return is_in_a_good_tetra;
}

/**
 * @param value
 * @param val0
 * @param val1
 * @param val2
 * @param val3
 * @param v0
 * @param v1
 * @param v2
 * @param bary
 * @param pt
 * @return a negative int if we are sure the point is out of the tetrahedron defined by
 *         v0,v1,v2,bary, and 0 or a positive one it is in
 */
int MeshTools::computeCheckInTetra(
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
    bool& is_on_cell_face)
{
  int result = 0;

  value = crossProduct(v0, v1, v2, bary);
  if (is_front_cell == true)
  {
    value = -value;
  }
  if (m_debug)
  {
    m_trace_mng->info() << "computeCheckInTetra - value= " << value;
  }

  if (value == 0)
  {
    return result;
  }
  if (value > 0)
  {
    // on est dans un "bon" tetra
    if (isInTetra(value, val0, val1, val2, val3, v0, v1, v2, bary, pt, epsilon, is_front_cell,is_on_cell_face))
    {
      result++;
    }
  }
  else
  {
    // on est dans un "mauvais" tetra
    if (isInTetra(value, val0, val1, val2, val3, v0, v1, v2, bary, pt, epsilon, is_front_cell,is_on_cell_face))
    {
      result = -2;
    }
  }
  if (is_on_cell_face)
  {
    result = 1;
  }
  return result;
}

/**
 * @param value
 * @param val0
 * @param val1
 * @param val2
 * @param val3
 * @param v0
 * @param v1
 * @param v2
 * @param bary
 * @param pt
 * @return true if pt is in the tetrahedron defined by v0,v1,v2,bary
 */
bool MeshTools::isInTetra(
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
    bool& is_on_cell_face)
{
  val3 = crossProduct(v0, v1, v2, pt);
  if (math::abs(val3) < epsilon)
  {
    if (isInTriangle(pt, v0, v1, v2))
    {
      is_on_cell_face = true;
      return true;
    }
    else
    {
      return false;
    }
  }
  val0 = crossProduct(pt, v1, v2, bary);
  val1 = crossProduct(v0, pt, v2, bary);
  val2 = crossProduct(v0, v1, pt, bary);

  if (is_front_cell == true)
  {
    val0 = -val0;
    val1 = -val1;
    val2 = -val2;
    val3 = -val3;
  }

  if (m_debug)
  {
    m_trace_mng->info() << "isInTetra - value= " << value;
    m_trace_mng->info() << "isInTetra - val0..3= " << val0 << " " << val1 << " " << val2 << " " << val3 << " ";
  }

  if (value < 0)
  { // on est dans un "mauvais" tetra
    if (val0 <= 0 && val1 <= 0 && val2 <= 0 && val3 <= 0)
    {
      return true;
    }
  }
  else
  { // on est dans un "bon" tetra
    if (val0 >= 0 && val1 >= 0 && val2 >= 0 && val3 >= 0)
    {
      return true;
    }
  }
  return false;
}

/*---------------------------------------------------------------------------*/
Real MeshTools::crossProduct(const Real3& v0, const Real3& v1, const Real3& v2, const Real3& v3)
{
  //m_n1.assign(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
  //m_n2.assign(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
  //m_n3.assign(v3.x - v0.x, v3.y - v0.y, v3.z - v0.z);

  Real3 n1 = v2 - v0 ;
  Real3 n2 = v1 - v0 ;
  Real3 n3 = v3 - v0 ;
  Real3 v = math::crossProduct3(n1, n2);
  return math::dot(v, n3);
}

/**
 * Compute the center of an item
 *
 * @param mesh the current mesh
 * @param item an item with nodes
 *
 * @return the geometric center of the item
 */
Real3 MeshTools::getCenter(const ItemWithNodes& item)
{
  Real3 p(0, 0, 0);

  ENUMERATE_NODE(inode,item.nodes())
  {
    p += m_nodes_coords[inode];
  }
  p /= item.nbNode();
  return p;
}

/**
 * Compute if p is inside the triangle (p1,p2,p3). 3D computing
 *
 * @param p point to verify
 * @parap pt1 first point defining a triangle
 * @parap pt2 second point defining a triangle
 * @parap pt3 third point defining a triangle
 *
 * @return true if p is inside the triangle (p1,p2,p3), false otherwise.
 */
bool MeshTools::isInTriangle(const Real3& p, const Real3& pt1, const Real3& pt2, const Real3& pt3)
{
  // we check if the point is in the triangle plane.
  // compute the normal to the triangle
  Real3 v1 = pt2 - pt1;
  Real3 v2 = pt3 - pt1;
  Real3 normal = math::crossProduct3(v1, v2);

  Real3 vec = pt1 - p;

  if (math::abs(math::dot(normal, vec)) > 0.)
  {
    // the point is not on the triangle plane.
    return false;
  }
  v1 = pt2 - p;
  Real3 v3 = math::crossProduct3(vec, v1);
  Real alpha0 = math::dot(normal, v3);
  if (alpha0 < 0)
  {
    return false;
  }
  v2 = pt3 - p;
  v3 = math::crossProduct3(v1, v2);
  Real alpha1 = math::dot(normal, v3);
  if (alpha1 < 0)
  {
    return false;
  }
  v3 = math::crossProduct3(v2, vec);
  Real alpha2 = math::dot(normal, v3);
  if (alpha2 < 0)
  {
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
