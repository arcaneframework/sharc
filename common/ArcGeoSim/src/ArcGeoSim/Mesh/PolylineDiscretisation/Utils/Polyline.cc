#include "Polyline.h"

#include "Segment.h"



/*---------------------------------------------------------------------------*/
void Polyline::addPoint(const Real3& point)
{
  m_points.add(point);
}

/*---------------------------------------------------------------------------*/
void Polyline::getPoint(Integer point_index, Real3& point) const
{
  if (point_index >= 0 && point_index < m_points.size())
  {
    point.assign(m_points[point_index]);
  }
  else
  {
    throw FatalErrorException(
        String::format("Polyline - index of point out of bounds: {0} (max: {1})", point_index, m_points.size()));
  }
}

/*---------------------------------------------------------------------------*/
Integer Polyline::getNumberOfPoints() const
{
  return m_points.size();
}

/*---------------------------------------------------------------------------*/
void Polyline::getSegment(Integer segment_index, Segment& segment) const
{
  if (segment_index >= 0 && segment_index < m_points.size() - 1)
  {
    segment.setPoints(m_points[segment_index], m_points[segment_index + 1]);
  }
  else
  {
    throw FatalErrorException(
        String::format("Polyline - index of segment out of bounds: {0} (max: {1})", segment_index,
            m_points.size() - 1));

  }
}

/*---------------------------------------------------------------------------*/
void Polyline::getPoints(Integer segment_index, Real3& point1, Real3& point2) const
{
  if (segment_index >= 0 && segment_index < m_points.size() - 1)
  {
    point1.assign(m_points[segment_index]);
    point2.assign(m_points[segment_index + 1]);
  }
  else
  {
    throw FatalErrorException(
        String::format("Polyline - index of segment out of bounds: {0} (max: {1})", segment_index,
            m_points.size() - 1));
  }
}

/*---------------------------------------------------------------------------*/
Integer Polyline::getNumberOfSegments() const
{
  if (m_points.size() > 1)
  {
    return m_points.size() - 1;
  }
  return 0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
