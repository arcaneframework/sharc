#ifndef AMCLIGHT_MESH_UTILS_IBOUNDINGBOX_H
#define AMCLIGHT_MESH_UTILS_IBOUNDINGBOX_H


#include "arcane/utils/Numeric.h"

/**
 * Class Interface for the bounding box
 */
using namespace Arcane;

class IBoundingBox
{
public:
  
  /** Destructeur de la classe */
  virtual ~IBoundingBox() {}
  
public:

  virtual Real getXMin() const = 0;
  virtual Real getXMax() const = 0;

  virtual Real getYMin() const = 0;
  virtual Real getYMax() const = 0;

  virtual Real getZMin() const = 0;
  virtual Real getZMax() const = 0;

  virtual bool contains(Real3 const& coord) const = 0 ;
};

#endif /* AMCLIGHT_MESH_UTILS_IBOUNDINGBOX_H */
