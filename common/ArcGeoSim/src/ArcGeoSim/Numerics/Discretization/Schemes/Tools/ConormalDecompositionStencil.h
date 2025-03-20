//
// Created by encheryg on 15/01/24.
//

#ifndef CONORMALDECOMPOSITIONSTENCIL_H
#define CONORMALDECOMPOSITIONSTENCIL_H

//#include "ArcGeoSim/Physics/Law2/Contribution/ContributionAccessor.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/ADFiniteVolumeStencil.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/ConormalDecompositionFaces.h"

class ConormalDecompositionStencil : public ADFiniteVolumeStencil {
public:
  ConormalDecompositionStencil(const DiscretizationConnectivity& dc, const ConormalDecompositionFaces& cdf) ;

private:
  friend class WOSSchemes ;
};

#endif //CONORMALDECOMPOSITIONSTENCIL_H
