#include <arcane/MathUtils.h>
#include "IntersectionSegmentTriangle.h"
#include "Segment.h"
#include "Triangle.h"



Real IntersectionSegmentTriangle::m_epsilon = 1e-6;

/**
 * Compute the intersection  between a segment and a triangle
 *
 * @param elem1 a segment
 * @param elem2 a triangle
 *
 * @param[out] point_inter the "first" intersection point
 */
bool IntersectionSegmentTriangle::compute(const Segment& elem1, const Triangle& elem2, Real3& point_inter)
{
  return compute(elem1.getPoint1(), elem1.getPoint2(), elem2.getPoint1(), elem2.getPoint2(), elem2.getPoint3(),
      point_inter);
}

/**
 * Compute the intersection between a segment and a triangle defined by their points
 *
 * @param sp1 first point of the segment
 * @param sp2 second point of the segment
 * @param tp1 first point of the triangle
 * @param tp2 second point of the triangle
 * @param tp3 third point of the triangle
 *
 * @param[out] point_inter the "first" intersection point
 */
bool IntersectionSegmentTriangle::compute(
    const Real3& sp1,
    const Real3& sp2,
    const Real3& tp1,
    const Real3& tp2,
    const Real3& tp3,
    Real3& point_inter)
{
  m_nb_inter = 0;

  m_a.assign(sp1);
  m_b.assign(sp2);

  m_t0.assign(tp1);
  m_t1.assign(tp2);
  m_t2.assign(tp3);

  m_sdir = sp2 - sp1;

  m_a0 = sp1 - m_t0;
  m_b0 = sp2 - m_t0;

  if (m_debug)
  {
    m_trace_mng->info() << "tp2= " << tp2.x << " " << tp2.y << " " << tp2.z << " ";
  }

  if (m_debug)
  {
    m_trace_mng->info() << "tp1= " << tp1.x << " " << tp1.y << " " << tp1.z << " ";
  }

  m_t10 = tp2 - tp1;
  m_t20 = tp3 - tp1;

  m_n = math::crossProduct3(m_t10, m_t20);

  Real n2 = m_n.abs2();

  setCrossValue(math::dot(m_n, m_sdir));

  if (n2 < 1e-6)
  {
    return false;
  }

  Real volA = math::dot(m_n, m_a0);
  Real volB = math::dot(m_n, m_b0);

  Integer sideA = volA > 1e-6 ? 1 : volA < -1e-6 ? -1 : 0;
  Integer sideB = volB > 1e-6 ? 1 : volB < -1e-6 ? -1 : 0;

  if (m_debug)
  {
    m_trace_mng->info() << "sideA=" << sideA;
    m_trace_mng->info() << "sideB=" << sideB;
  }

  if (sideA * sideB > 0)
  {
    return false;
  }

  if (sideA == 0 && sideB == 0)
  {
    return coplanarCompute(sp1, sp2, point_inter);
  }

  Real vol = math::abs(volA) + math::abs(volB);

  if (m_debug)
  {
    m_trace_mng->info() << "vol=" << vol;
  }

  if (sideA < sideB)
  {
    m_n = math::crossProduct3(m_a0, m_b0);
  }
  else
  {
    m_n = math::crossProduct3(m_b0, m_a0);
  }

  if (m_debug)
  {
    m_trace_mng->info() << "m_n= " << m_n.x << " " << m_n.y << " " << m_n.z << " ";
  }

  if (m_debug)
  {
    m_trace_mng->info() << "m_t10= " << m_t10.x << " " << m_t10.y << " " << m_t10.z << " ";
  }

  Real alpha2 = math::dot(m_n, m_t10) / vol;

  if (m_debug)
  {
    m_trace_mng->info() << "alpha2=" << alpha2;
  }

  if (alpha2 < -1e-6)
  {
    return false;
  }

  Real alpha1 = -math::dot(m_n, m_t20) / vol;

  if (m_debug)
  {
    m_trace_mng->info() << "alpha1=" << alpha1;
  }

  if (alpha1 < -1e-6)
  {
    return false;
  }

  Real alpha0 = 1. - alpha1 - alpha2;

  if (m_debug)
  {
    m_trace_mng->info() << "alpha0=" << alpha0;
  }

  if (alpha0 < -1e-6)
  {
    return false;
  }

  point_inter = alpha0 * m_t0;
  point_inter += alpha1 * m_t1;
  point_inter += alpha2 * m_t2;

  if (m_debug)
  {
    m_trace_mng->info() << "pointInter= " << point_inter.x << " " << point_inter.y << " " << point_inter.z;
  }

  m_bary[0][0] = alpha0;
  m_bary[0][1] = alpha1;
  m_bary[0][2] = alpha2;

  m_b -= m_a;

  m_t[0] = computeT(m_a, m_b, point_inter);

  m_pt_inter[0].assign(point_inter);
  m_nb_inter = 1;

  return true;
}

/*---------------------------------------------------------------------------*/
bool IntersectionSegmentTriangle::coplanarCompute(const Real3& sp1, const Real3& sp2, Real3& result)
{
  Short maxC = maxCoord(m_n);

  if (maxC == 0)
  {
    setZY();
    swap(m_t0, m_t1);
  }
  else if (maxC == 1)
  {
    setXZ();
    swap(m_t0, m_t1);
  }

  if (!compute2D())
  {
    return false;
  }

  if (maxC == 0)
  {
    setZY(m_a);
    setZY(m_b);
  }
  else if (maxC == 1)
  {
    setXZ(m_a);
    setXZ(m_b);
  }

  m_a0.assign(sp1);
  m_b0.assign(sp2);

  m_b0.sub(m_a0);

  m_pt_inter[0] = m_a;

  m_t[0] = computeT(m_a0, m_b0, m_a);
  result.assign(m_a);

  if (m_nb_inter > 1)
  {
    m_pt_inter[1] = m_b;
    m_t[1] = computeT(m_a0, m_b0, m_b);
  }

  return true;
}

/*---------------------------------------------------------------------------*/
Real IntersectionSegmentTriangle::computeT(const Real3& o, const Real3 & v, const Real3 & p)
{
  Integer maxC = maxCoord(v);

  switch (maxC)
  {
    case 0:
      return computeT(o.x, v.x, p.x);
    case 1:
      return computeT(o.y, v.y, p.y);
    case 2:
      return computeT(o.z, v.z, p.z);
    default:
      return -1;
  }
}

/*---------------------------------------------------------------------------*/
Real IntersectionSegmentTriangle::computeT(Real o, Real v, Real p)
{
  return clamp((p - o) / v);
}

/*---------------------------------------------------------------------------*/
Real IntersectionSegmentTriangle::clamp(Real t)
{
  return t < 0 ? 0 : 1 < t ? 1 : t;
}

/*---------------------------------------------------------------------------*/
bool IntersectionSegmentTriangle::compute2D()
{
  if (m_n.z < 0)
  {
    swap(m_t0, m_t1);
  }

  if (!isecEdge2D(m_t0, m_t1))
  {
    return false;
  }
  if (!isecEdge2D(m_t1, m_t2))
  {
    return false;
  }
  if (!isecEdge2D(m_t2, m_t0))
  {
    return false;
  }

  m_nb_inter = 2;

  if ((m_a - m_b).abs2() < 1e-6)
  {
    m_nb_inter = 1;
  }

  return true;
}

/*---------------------------------------------------------------------------*/
bool IntersectionSegmentTriangle::isecEdge2D(const Real3& t0, const Real3& t1)
{
  m_a0 = m_a - t0;
  m_b0 = m_b - t0;
  m_t10 = t1 - t0;

  Real ab = math::vecMul2D(m_a0, m_b0);

  if (ab < 1e-10)
	return false; //compute1D();

  Real alpha, beta;
  Real alpha_, beta_;
  Real3* pta;

  alpha = math::vecMul2D(m_t10, m_a0);
  beta = math::vecMul2D(m_t10, m_b0);

  if (alpha <= beta)
  {
    alpha_ = alpha;
    beta_ = beta;
    pta = &m_a;
  }
  else
  {
    alpha_ = beta;
    beta_ = alpha;
    pta = &m_b;
  }

  if (beta_ < -1e-6)
  {
    return false;
  }
  if (alpha_ < 0)
  {
    *pta = ab / (beta - alpha) * m_t10;
    pta->add(t0);
  }

  return true;
}
/*---------------------------------------------------------------------------*/
bool IntersectionSegmentTriangle::compute1D() {

        Real3 max = math::max(math::max(m_t1,m_t2),math::max(m_a,m_b));
        Real3 min = math::min(math::min(m_t1,m_t2),math::min(m_a,m_b));
        Real xlenght = max.x - min.x;
        Real ylenght = max.y - min.y;
        Real zlenght = max.z - min.z;


        if (xlenght > ylenght && xlenght >zlenght) {
            return compute1D(m_t1.x, m_t2.x, m_a.x, m_b.x);
        }
        else if (ylenght > xlenght && ylenght > zlenght)
        	return compute1D(m_t1.y, m_t2.y, m_a.y, m_b.y);
        else {
            return compute1D(m_t1.z, m_t2.z, m_a.z, m_b.z);
        }
    }
/*---------------------------------------------------------------------------*/
bool IntersectionSegmentTriangle::compute1D(double a1, double b1, double a2, double b2) {
        if (a1 > b1) {
            double tmp = a1;
            a1 = b1;
            b1 = tmp;
        }
        if (a2 > b2) {
            double tmp = a2;
            a2 = b2;
            b2 = tmp;
        }

        if (b1 < a2 || b2 < a1) {
            return false;
        }

        m_t[0] = (math::max(a1, a2) - a1) / (b1 - a1);
        m_t[1] = (math::max(a2, a1) - a2) / (b2 - a2);

        return true;
}
/*---------------------------------------------------------------------------*/

void IntersectionSegmentTriangle::setZY()
{
  setZY(m_a);
  setZY(m_b);
  setZY(m_t0);
  setZY (m_t1);
  setZY (m_t2);
  setZY(m_n);
}

/*---------------------------------------------------------------------------*/

void IntersectionSegmentTriangle::setZY(Real3& p)
{
  Real tmp = p.x;
  p.x = p.z;
  p.z = tmp;
}
/*---------------------------------------------------------------------------*/

void IntersectionSegmentTriangle::setXZ()
{
  setXZ(m_a);
  setXZ(m_b);
  setXZ(m_t0);
  setXZ (m_t1);
  setXZ (m_t2);
  setXZ(m_n);
}

/*---------------------------------------------------------------------------*/
void IntersectionSegmentTriangle::setXZ(Real3& p)
{
  Real tmp = p.y;
  p.y = p.z;
  p.z = tmp;
}

/*---------------------------------------------------------------------------*/

Short IntersectionSegmentTriangle::maxCoord(const Real3& p)
{
  Real x = math::abs(p.x);
  Real y = math::abs(p.y);
  Real z = math::abs(p.z);

  Short coord = 0;
  if (y > x)
  {
    y = x;
    coord = 1;
  }

  if (z > x)
  {
    coord = 2;
  }

  return coord;
}

/*---------------------------------------------------------------------------*/
void IntersectionSegmentTriangle::swap(Real3& a, Real3& b)
{
  Real tmp;

  tmp = a.x;
  a.x = b.x;
  b.x = tmp;

  tmp = a.y;
  a.y = b.y;
  b.y = tmp;

  tmp = a.z;
  a.z = b.z;
  b.z = tmp;

}

/*---------------------------------------------------------------------------*/
void IntersectionSegmentTriangle::setCrossValue(Real d)
{
  if (d >= m_epsilon)
  {
    m_cross_value = 1;
  }
  else if (d <= -m_epsilon)
  {
    m_cross_value = -1;
  }
  else
  {
    m_cross_value = 0;
  }
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
