// -*- C++ -*-
#ifndef ARGEOSIM_MESH_POLYLINEDISCRETISATION_SEGMENT_H
#define ARGEOSIM_MESH_POLYLINEDISCRETISATION__SEGMENT_H

#include <arcane/utils/Real3.h>

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class Segment defining a segment (two points).
 */
class Segment
{
public:

  /** Constructeur de la classe */
  Segment()
  {
  }

  /** Constructeur de la classe avec initialisation */
  Segment(const Real3& v1, const Real3& v2)
  {
    setPoints(v1, v2);
  }

  /** Destructeur de la classe */
  virtual ~Segment()
  {
  }

public:
  /** Set the points of the segment */
  void setPoints(const Real3& v1, const Real3& v2);

  /** Get the first point */
  Real3 getPoint1() const;
  /** Get the second point */
  Real3 getPoint2() const;

  /** Get the x coordinate of the first point */
  Real getP1X() const;
  /** Get the y coordinate of the first point */
  Real getP1Y() const;
  /** Get the z coordinate of the first point */
  Real getP1Z() const;

  /** Get the x coordinate of the second point */
  Real getP2X() const;
  /** Get the y coordinate of the second point */
  Real getP2Y() const;
  /** Get the z coordinate of the second point */
  Real getP2Z() const;

private:
  /** First point of the segment */
  Real3 m_p1;

  /** Secodn point of the segment */
  Real3 m_p2;
};

#endif /* ARGEOSIM_MESH_POLYLINEDISCRETISATION_SEGMENT_H */
