// -*- C++ -*-
#ifndef ARCGEOSIM_NUMERICS_DISCRETIZATION_MULTIPOINTSSTENCIL_H
#define ARCGEOSIM_NUMERICS_DISCRETIZATION_MULTIPOINTSSTENCIL_H

#include "ArcGeoSim/Numerics/NumericalScheme/Stencil.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/Contact.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/ContactGroup.h"
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcNum {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
class MultiPointsStencil
{
public:
  virtual ~MultiPointsStencil() {}
  virtual void init(const Arcane::Face& f) = 0 ;
  virtual void init(const Contact& f) = 0 ;
  virtual void init(const Arcane::FaceEnumerator& f) = 0 ;
  virtual void init(const ContactEnumerator& f) = 0 ;
  virtual void begin() = 0 ;
  virtual void next() = 0 ;
  virtual bool end()  = 0 ;
  virtual const Stencil::Cell& cell() = 0 ;
  virtual size_t size() const = 0 ;
  virtual const Stencil::Cell& backBoundaryCell() = 0 ;
  virtual const Stencil::Cell& frontCell() = 0 ;
  virtual void finalize() = 0 ;
};

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_NUMERICS_DISCRETIZATION_MULTIPOINTSSTENCIL_H */
