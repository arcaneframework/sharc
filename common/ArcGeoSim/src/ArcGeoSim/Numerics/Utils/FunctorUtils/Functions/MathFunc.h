#ifndef MATHFUNC_H
#define MATHFUNC_H

/*!
 * \file MathFunc.h 
 * \brief Operations mathematiques standard applicables aux tableaux
 * \date 01/04/2008
 * \author Xavier TUNC
 */

//! Operations mathematiques sur les tableaux
/*! Definition des operations mathematiques applicables aux tableaux. \n
 *  Les operations sin, asin, sinh, cos, acos, acosh, tan, atan, atanh, exp, sqrt, log, log10 et abs sont implementes. \n
 */

//! \brief Macro permettant de generer les operations mathematiques

#define MATHFUNC(NAME,FCT,DFCT) \
template <typename T> \
class NAME \
{ \
public : \
  typedef typename T::value_type value_type; \
  static inline value_type getValue(const T & left, const T & right, int index) \
    { \
      return FCT(left.getValue(index)); \
    } \
    static inline value_type getDerivedValue(const T & left, const T & right, int ligne, int colonne) \
    { \
      return DFCT; \
    } \
}; \
template <typename T> \
inline Expr<T , NAME<T>, T > FCT(const T & a) { \
  typedef NAME<T> value_type; \
  return Expr<T , NAME<T>, T > (a,a); \
} \

MATHFUNC(sinArray,sin, left.getDerivedValue(ligne,colonne)*cos(left.getValue(ligne)))
MATHFUNC(asinArray,asin, left.getDerivedValue(ligne,colonne)/sqrt(1-left.getValue(ligne)*left.getValue(ligne)))
MATHFUNC(sinhArray,sinh, left.getDerivedValue(ligne,colonne)*cosh(left.getValue(ligne)))
//MATHFUNC(asinhArray,asinh)

MATHFUNC(cosArray,cos, -left.getDerivedValue(ligne,colonne)*sin(left.getValue(ligne)))
MATHFUNC(acosArray,acos, -left.getDerivedValue(ligne,colonne)/sqrt(1-left.getValue(ligne)*left.getValue(ligne)))
MATHFUNC(coshArray,cosh, left.getDerivedValue(ligne,colonne)*sinh(left.getValue(ligne)))
//MATHFUNC(acoshArray,acosh)

MATHFUNC(tanArray,tan, left.getDerivedValue(ligne,colonne)*(1+tan(left.getValue(ligne))*tan(left.getValue(ligne))))
MATHFUNC(atanArray,atan, left.getDerivedValue(ligne,colonne)/sqrt(1+left.getValue(ligne)*left.getValue(ligne)))
MATHFUNC(tanhArray,tanh, left.getDerivedValue(ligne,colonne)/(cosh(left.getValue(ligne))*cosh(left.getValue(ligne))))
//MATHFUNC(atanhArray,atanh)

MATHFUNC(expArray,exp, left.getDerivedValue(ligne,colonne)*exp(left.getValue(ligne)))
MATHFUNC(sqrtArray,sqrt, left.getDerivedValue(ligne,colonne)/(2*sqrt(left.getValue(ligne))))
MATHFUNC(lnArray,log, left.getDerivedValue(ligne,colonne)/left.getValue(ligne))
MATHFUNC(log10Array,log10, left.getDerivedValue(ligne,colonne)/(log(value_type(10))*left.getValue(ligne)))
MATHFUNC(absArray,abs, abs(left.getDerivedValue(ligne,colonne)))

#undef MATHFUNC

#endif
