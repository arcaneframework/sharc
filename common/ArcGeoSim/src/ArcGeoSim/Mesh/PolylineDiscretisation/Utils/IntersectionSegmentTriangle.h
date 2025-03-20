// -*- C++ -*-
#ifndef ARGEOSIM_MESH_POLYLINEDISCRETISATION_INTERSECTIONSEGMENTTRIANGLE_H
#define ARGEOSIM_MESH_POLYLINEDISCRETISATION_INTERSECTIONSEGMENTTRIANGLE_H



#include <arcane/utils/Real2.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/Array.h>

using namespace Arcane;

class Segment;
class Triangle;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class IntersectionSegmentTriangle Intersection segment - triangle
 *
 * This is imported from OpenFlow's IntersectionSegmentTriangle class.
 */
class IntersectionSegmentTriangle
{
public:

  /** Constructeur de la classe */
  IntersectionSegmentTriangle()
  {
    m_cross_value = 0;
    m_nb_inter = 0;
    m_pt_inter.resize(2);
    m_bary.resize(2);
    m_debug = false;
  }

  /** Destructeur de la classe */
  virtual ~IntersectionSegmentTriangle()
  {
  }

public:

  bool compute(const Segment& elem1, const Triangle& elem2, Real3& point_inter);

  bool compute(
      const Real3& sp1,
      const Real3& sp2,
      const Real3& tp1,
      const Real3& tp2,
      const Real3& tp3,
      Real3& point_inter);

  /**
   * get the number of intersection points
   *
   * @return the number of intersection
   */
  Integer getNbIntersection()
  {
    return m_nb_inter;
  }

  /**
   * return an intersection point
   *
   * @param i index of a point in the list containing the intersection points
   * @param[out] p result
   */
  void getIntersection(Short i, Real3& p)
  {
    p.assign(m_pt_inter[i]);
  }

  /**
   * return the position of an intersection on the segment. between 0 and 1 : 0 for start point, 1
   * for end point.
   *
   * @param i index of a point in the list containing the intersection points
   * @return the position
   */
  Real getIntersectionValue(Short i)
  {
    return m_t[i];
  }

  /** debug purpose - set the trace manager */
  void setTraceManager(ITraceMng* trace_mng)
  {
    m_debug = true;
    m_trace_mng = trace_mng;
  }

private:
  Integer m_cross_value;
  Integer m_nb_inter;
  static Real m_epsilon;

  /** trace manager */
  ITraceMng* m_trace_mng;
  /** debug flag for trace purpose */
  bool m_debug;

  /** the intersection point(s) */
  Real3SharedArray m_pt_inter;

  Real3SharedArray m_bary;
  Real2 m_t;

  Real3 m_a, m_b;
  Real3 m_t0, m_t1, m_t2;
  Real3 m_sdir;
  Real3 m_a0, m_b0;
  Real3 m_t10, m_t20;
  Real3 m_n;

private:
  void setCrossValue(Real d);
  bool coplanarCompute(const Real3& sp1, const Real3& sp2, Real3& result);
  Integer maxCoord(const Real3& p);
  void swap(Real3& a, Real3& b);
  void setZY();
  void setZY(Real3& p);
  void setXZ();
  void setXZ(Real3& p);
  Real computeT(const Real3& o, const Real3 & v, const Real3 & p);
  Real computeT(Real o, Real v, Real p);
  Real clamp(Real t);

  bool compute2D();
  bool isecEdge2D(const Real3& t0, const Real3& t1);

  bool compute1D();
  bool compute1D(double a1, double b1, double a2, double b2);
};

#endif /* ARGEOSIM_MESH_POLYLINEDISCRETISATION_INTERSECTIONSEGMENTTRIANGLE_H */
