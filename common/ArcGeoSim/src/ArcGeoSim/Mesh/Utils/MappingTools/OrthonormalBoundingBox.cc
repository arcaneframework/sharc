#include "OrthonormalBoundingBox.h"


/*---------------------------------------------------------------------------*/

OrthonormalBoundingBox::OrthonormalBoundingBox()
{
  reset();
}

void OrthonormalBoundingBox::set(IBoundingBox& bbox)
{
  m_x_min = bbox.getXMin();
  m_x_max = bbox.getXMax();
  m_y_min = bbox.getYMin();
  m_y_max = bbox.getYMax();
  m_z_min = bbox.getZMin();
  m_z_max = bbox.getZMax();
}

void OrthonormalBoundingBox::addPoint(Real3 tuple)
{
  addPoint(tuple.x, tuple.y, tuple.z);
}

void OrthonormalBoundingBox::addPoint(Real x, Real y, Real z)
{
  m_x_min = math::min(x, m_x_min);
  m_y_min = math::min(y, m_y_min);
  m_z_min = math::min(z, m_z_min);

  m_x_max = math::max(x, m_x_max);
  m_y_max = math::max(y, m_y_max);
  m_z_max = math::max(z, m_z_max);   
}

void OrthonormalBoundingBox::reset()
{
  m_x_min = m_y_min = m_z_min = +FloatInfo<Real>::maxValue();
  m_x_max = m_y_max = m_z_max = -FloatInfo<Real>::maxValue();
}

void OrthonormalBoundingBox::increaseBBox(Real t, Real eps)
{
  Real xcenter = (m_x_min + m_x_max) / 2.0;
  Real ycenter = (m_y_min + m_y_max) / 2.0;
  Real zcenter = (m_z_min + m_z_max) / 2.0;

  m_x_min = xcenter + (1 + t) * (m_x_min - eps - xcenter);
  m_x_max = xcenter + (1 + t) * (m_x_max + eps - xcenter);

  m_y_min = ycenter + (1 + t) * (m_y_min - eps - ycenter);
  m_y_max = ycenter + (1 + t) * (m_y_max + eps - ycenter);

  m_z_min = zcenter + (1 + t) * (m_z_min - eps - zcenter);
  m_z_max = zcenter + (1 + t) * (m_z_max + eps - zcenter);

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
