// -*- C++ -*-
#ifndef ARGEOSIM_MESH_POLYLINEDISCRETISATION_TRIANGLE_H
#define ARGEOSIM_MESH_POLYLINEDISCRETISATION_TRIANGLE_H

#include <arcane/utils/Real3.h>

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class Triangle defining a triangle (three points).
 */
class Triangle
{
public:

  /** Constructeur de la classe */
  Triangle()
  {
  }

  /** Constructeur de la classe avec initialisation */
  Triangle(const Real3& v1, const Real3& v2, const Real3& v3)
  {
    setPoints(v1, v2, v3);
  }

  /** Destructeur de la classe */
  virtual ~Triangle()
  {
  }

public:

  /** Set the points of the triangle */
  void setPoints(const Real3& v1, const Real3& v2, const Real3& v3);

  /** Get the first point */
  Real3 getPoint1() const;
  /** Get the second point */
  Real3 getPoint2() const;
  /** Get the third point */
  Real3 getPoint3() const;

private:
  /** First point of the segment */
  Real3 m_p1;

  /** Second point of the segment */
  Real3 m_p2;

  /** Third point of the segment */
  Real3 m_p3;
};

#endif /* ARGEOSIM_MESH_POLYLINEDISCRETISATION_TRIANGLE_H */
