#ifndef BILINEAR2D_H
#define BILINEAR2D_H
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
 
#include "ArcGeoSim/Utils/ItemTools.h"
#include <arcane/MathUtils.h>

/*!
  \struct Bilinear
  \author Soleiman YOUSEF
  \brief Piecewise bilinear functions
*/

struct Bilinear2D
{
public:
  Bilinear2D(const Real& a_hx,         // cell size in x direction
	   const Real& a_hy,         // cell size in y direction
	   const Real& a_hz,         // cell size in z direction
	   const Real3 & a_center,   // center
	   const SharedArray<Real> & a_coef // polynom coeficient [a,b,c,d]
	    ): m_hx(a_hx),
	       m_hy(a_hy),
	       m_hz(a_hz),
	       m_center(a_center),
	       m_coef(a_coef)
  {
    // DO NOTHING
  }

  // Return the value of the Bilinear2D function
  inline const Real eval(const Real3 & node) const
  {
    Real result;
    Real coefX = (node[2]-m_center[2])/m_hx;
    Real coefY = (node[1]-m_center[1])/m_hy;
    Real coef0 = (coefX-0.5)*(coefY-0.5);
    Real coef1 = (coefX+0.5)*(coefY-0.5);
    Real coef2 = (coefX-0.5)*(coefY+0.5);
    Real coef3 = (coefX+0.5)*(coefY+0.5);
    result=  m_coef[0]*coef0
           + m_coef[1]*coef1
           + m_coef[2]*coef2
           + m_coef[3]*coef3;
    return result;
  }

  // Return the values of the derivative functions
  inline const Real3 gradient(const Real3 & node) const
  {
    Real3 result;
    Real coefX = (node[2]-m_center[2])/m_hx;
    Real coefY = (node[1]-m_center[1])/m_hy;
    Real coef0_gx = (coefY-0.5)/m_hx;
    Real coef1_gx = (coefY-0.5)/m_hx;
    Real coef2_gx = (coefY+0.5)/m_hx;
    Real coef3_gx = (coefY+0.5)/m_hx;
    Real coef0_gy = (coefX-0.5)/m_hy;
    Real coef2_gy = (coefX-0.5)/m_hy;
    Real coef1_gy = (coefX+0.5)/m_hy;
    Real coef3_gy = (coefX+0.5)/m_hy;
    result[0]=   m_coef[0]*coef0_gx
            + m_coef[1]*coef1_gx
            + m_coef[2]*coef2_gx
            + m_coef[3]*coef3_gx;

    result[1]=    m_coef[0]*coef0_gy
            + m_coef[1]*coef1_gy
            + m_coef[2]*coef2_gy
            + m_coef[3]*coef3_gy;

    result[2]= 0.;
    return result;
  }

/*
  // Return the value of the Bilinear2D function
  inline const Real eval(const Real3 & node) const
  {
    Real result;
    result=  m_coef[0]*(node[2]-m_center[2])/m_hx*(node[1]-m_center[1])/m_hy
           + m_coef[1]*(node[2]-m_center[2])/m_hx
           + m_coef[2]*(node[1]-m_center[1])/m_hy
           + m_coef[3];
    return result;
  }

  // Return the values of the derivative functions
  inline const Real3 gradient(const Real3 & node) const
  {
    Real3 result;
     
    result[0]= (m_coef[0]/m_hx)*(node[1]-m_center[1])/m_hy
             + (m_coef[1]/m_hx);

    result[1]=   (m_coef[0]/m_hy)*(node[2]-m_center[2])/m_hx
               + (m_coef[2]/m_hy);

    result[2]= 0.;
    return result;
  }
*/
private:
  Real m_hx;         // cell size in x direction
  Real m_hy;         // cell size in y direction
  Real m_hz;         // cell size in z direction
  Real3 m_center;   // center
  SharedArray<Real>  m_coef; // polynom coeficient [a,b,c,d,e]
};

 

#endif 
