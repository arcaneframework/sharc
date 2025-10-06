#include "ArcGeoSim/Mesh/Geometry/Static/StaticGeometry.h"
using namespace Arcane;
#include <arcane/utils/FatalErrorException.h>
#include <arcane/utils/NotImplementedException.h>



Real
StaticGeometry::
computeLength(const ItemWithNodes & item)
{
  throw FatalErrorException(A_FUNCINFO,"Not implemented");
}
Real
StaticGeometry::
computeLength(const Real3& m, const Real3& n)
{
  
  const Real3 d = m-n ;
  return math::sqrt(math::scaMul(d,d)) ;
}
