// -*- C++ -*-
#ifndef AMCLIGHT_MESH_UTILS_ORTHONORMALBOUNDINGBOX_H
#define AMCLIGHT_MESH_UTILS_ORTHONORMALBOUNDINGBOX_H


#include <ArcGeoSim/Mesh/Utils/MappingTools/IBoundingBox.h>
#include <arcane/utils/Real3.h>

using namespace Arcane;

class OrthonormalBoundingBox: public IBoundingBox
{
public:

  /** Constructeur de la classe */
  OrthonormalBoundingBox();

  /** Destructeur de la classe */
  virtual ~OrthonormalBoundingBox()
  {
  }

  bool contains(Real3 const& coord) const
  {
    if(coord.x<m_x_min || coord.x > m_x_max)
      return false ;
    else
    {
      if(coord.y<m_y_min || coord.y > m_y_max)
        return false ;
      else
      {
        if(coord.z<m_z_min || coord.z > m_z_max)
          return false ;
        else
          return true ;
      }
    }
  }


private:
  Real m_x_min;
  Real m_x_max;

  Real m_y_min;
  Real m_y_max;

  Real m_z_min;
  Real m_z_max;
  
public:

  void set(IBoundingBox& bbox);

  void addPoint(Real3 tuple);
  void addPoint(Real x, Real y, Real z);

  void reset();

  void increaseBBox(Real t, Real eps);

  Real getXMin() const
  {
    return m_x_min;
  }

  Real getXMax() const
  {
    return m_x_max;
  }

  Real getYMin() const
  {
    return m_y_min;
  }

  Real getYMax() const
  {
    return m_y_max;
  }

  Real getZMin() const
  {
    return m_z_min;
  }

  Real getZMax() const
  {
    return m_z_max;
  }

  Real3 getMinCorner() const {
    return Real3(m_x_min,m_y_min,m_z_min) ;
  }

  Real3 getMaxCorner() const {
    return Real3(m_x_max,m_y_max,m_z_max) ;
  }


  /*---------------------------------------------------------------------------*/
  void setXMin(Real x_min)
  {
    m_x_min = x_min;
    ;
  }

  /*---------------------------------------------------------------------------*/

  void setXMax(Real x_max)
  {
    m_x_max = x_max;
  }

  /*---------------------------------------------------------------------------*/

  void setYMin(Real y_min)
  {
    m_y_min = y_min;
  }

  /*---------------------------------------------------------------------------*/

  void setYMax(Real y_max)
  {
    m_y_max = y_max;
  }

  /*---------------------------------------------------------------------------*/

  void setZMin(Real z_min)
  {
    m_z_min = z_min;
  }

  /*---------------------------------------------------------------------------*/

  void setZMax(Real z_max)
  {
    m_z_max = z_max;
  }

};

#endif /* AMCLIGHT_MESH_UTILS_ORTHONORMALBOUNDINGBOX_H */
