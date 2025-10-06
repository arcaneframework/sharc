#ifndef IFUNCTOR_H_
#define IFUNCTOR_H_

namespace ArcGeoSim {
class IFunctor
{
public:
  virtual ~IFunctor() { }
  virtual Real eval(Real x) const = 0 ;
  virtual void eval(Real* y, Real* dy,const Real x) const = 0 ;
};
}
#endif /*IFUNCTOR_H_*/
