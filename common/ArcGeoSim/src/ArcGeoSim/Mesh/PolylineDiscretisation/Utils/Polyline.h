// -*- C++ -*-
#ifndef ARGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINE_H
#define ARGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINE_H


#include <arcane/utils/Real3.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/String.h>
#include <arcane/utils/FatalErrorException.h>

class Segment;

using namespace Arcane;

class Polyline;

typedef SharedArray<Polyline> PolylineArray;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * Class Polyline defining a polyline.
 */
class Polyline
{
public:

  /** Constructeur de la classe */
  Polyline()
  {
  }

  /** Destructeur de la classe */
  virtual ~Polyline()
  {
  }

public:

  /** Add a point to the polyline */
  void addPoint(const Real3& point);

  /** get the point_index-th point of the polyline */
  void getPoint(Integer point_index, Real3& point) const;

  /** get number of points of the polyline */
  Integer getNumberOfPoints() const;

  /** get the segment_index-th segment of the polyline as a segment */
  void getSegment(Integer segment_index, Segment& segment) const;

  /** get the segment_index-th segment of the polyline as two points */
  void getPoints(Integer segment_index, Real3& point1, Real3& point2) const;

  /** get number of segments of the polyline */
  Integer getNumberOfSegments() const;

private:
  /** the points of the polyline */
  Real3SharedArray m_points;
};

#endif /* ARGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINE_H */
