#ifndef Quadratic2D_H_
#define QUADRATIC2D_H_
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "ArcGeoSim/Utils/ItemTools.h"
#include <arcane/MathUtils.h>

using namespace Arcane;
/*!
  \struct Quadratic
  \author Soleiman YOUSEF
  \brief Piecewise quadratic functions
*/

struct Quadratic2D
{
public:
  Quadratic2D(const Real& a_hx,         // cell size in x direction
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

  // Return the value of the Quadratic2D function
  inline const Real eval(const Real3 & node) const
  {

    Real result =   m_coef[0]*std::pow((node[2]-m_center[2])/m_hx,2)
                  + m_coef[1]*std::pow((node[1]-m_center[1])/m_hy,2)
                  + m_coef[2]*std::pow((node[2]-m_center[2])/m_hx,1)
                  + m_coef[3]*std::pow((node[1]-m_center[1])/m_hy,1)
                  + m_coef[4];

    return result;
  }

  // Return the values of the derivative functions
  inline const Real3 gradient(const Real3 & node) const
  {
    Real3 result;

    result[0]=2*m_coef[0]*(node[2]-m_center[2])/std::pow(m_hx,2)
      +m_coef[2]/m_hx;

    result[1]=2*m_coef[1]*(node[1]-m_center[1])/std::pow(m_hy,2)
      +m_coef[3]/m_hy;

    result[2]=0.;

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
