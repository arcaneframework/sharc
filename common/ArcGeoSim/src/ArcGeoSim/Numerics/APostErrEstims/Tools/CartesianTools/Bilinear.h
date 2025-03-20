#ifndef BILINEAR_H
#define BILINEAR_H
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
 
#include "ArcGeoSim/Utils/ItemTools.h"
#include <arcane/MathUtils.h>

/*!
  \struct Bilinear
  \author Soleiman YOUSEF
  \brief Piecewise bilinear functions
*/

struct Bilinear
{
public:
  Bilinear(const Real& a_hx,         // cell size in x direction
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

  // Return the value of the bilinear function
  inline const Real eval(const Real3 & node) const
  {
    Real result;
    result=m_coef[0]*((node[0]-m_center[0])/m_hx)*((node[1]-m_center[1])/m_hy)
                    *((node[2]-m_center[2])/m_hz)
           + m_coef[1]*(node[0]-m_center[0])/m_hx*(node[1]-m_center[1])/m_hy
           + m_coef[2]*(node[1]-m_center[1])/m_hy*(node[2]-m_center[2])/m_hz
           + m_coef[3]*(node[2]-m_center[2])/m_hz*(node[0]-m_center[0])/m_hx
           + m_coef[4]*(node[0]-m_center[0])/m_hx
           + m_coef[5]*(node[1]-m_center[1])/m_hy
           + m_coef[6]*(node[2]-m_center[2])/m_hz
           + m_coef[7];
    return result;
  }

  // Return the values of the derivative functions
  inline const Real3 gradient(const Real3 & node) const
  {
    Real3 result;
     
    result[0]=   (m_coef[0]/m_hx)*((node[1]-m_center[1])/m_hy)
                                 *((node[2]-m_center[2])/m_hz)
	       + (m_coef[1]/m_hx)*(node[1]-m_center[1])/m_hy 
	       + (m_coef[3]/m_hx)*(node[2]-m_center[2])/m_hz
	       + (m_coef[4]/m_hx);

    result[1]=   (m_coef[0]/m_hy)*((node[0]-m_center[0])/m_hx)
                                 *((node[2]-m_center[2])/m_hz)
               + (m_coef[2]/m_hy)*(node[2]-m_center[2])/m_hz 
               + (m_coef[1]/m_hy)*(node[0]-m_center[0])/m_hx
               + (m_coef[5]/m_hy);

    result[2]=   (m_coef[0]/m_hz)*((node[1]-m_center[1])/m_hy)
                                 *((node[0]-m_center[0])/m_hx)
               + (m_coef[3]/m_hz)*(node[0]-m_center[0])/m_hx 
               + (m_coef[2]/m_hz)*(node[1]-m_center[1])/m_hy
               + (m_coef[6]/m_hz);

    return result;
  }

private:
  Real m_hx;         // cell size in x direction
  Real m_hy;         // cell size in y direction
  Real m_hz;         // cell size in z direction
  Real3 m_center;   // center
  SharedArray<Real>  m_coef; // polynom coeficient [a,b,c,d,e]
};

 

#endif 
