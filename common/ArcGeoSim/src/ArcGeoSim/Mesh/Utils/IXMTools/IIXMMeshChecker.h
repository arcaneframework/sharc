#ifndef ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_IIXMMESHCHECKER_H
#define ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_IIXMMESHCHECKER_H


#include "ArcGeoSim/Utils/ArcGeoSim.h"

BEGIN_ARCGEOSIM_NAMESPACE

class IIXMMeshChecker

{

public:

  /** Destructeur de la classe */
  virtual ~IIXMMeshChecker() {}

  virtual void run() = 0;

  virtual void print() = 0;
};

END_ARCGEOSIM_NAMESPACE

#endif /* ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_IIXMMESHCHECKER_H */
