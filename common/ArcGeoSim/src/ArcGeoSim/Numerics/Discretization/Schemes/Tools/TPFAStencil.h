//
// Created by encheryg on 12/03/24.
//

#ifndef TPFASTENCIL_H
#define TPFASTENCIL_H

#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/ADFiniteVolumeStencil.h"

class TPFAStencil  : public ADFiniteVolumeStencil {
public:
  TPFAStencil(const DiscretizationConnectivity& dc) ;

private:
  friend class TPFAScheme ;
};

#endif //TPFASTENCIL_H
