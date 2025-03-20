//
// Created by encheryg on 20/06/24.
//

#ifndef GREEDYCONORMALDECOMPOSITIONCOMPUTER_H
#define GREEDYCONORMALDECOMPOSITIONCOMPUTER_H

#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/ConormalDecompositionComputer.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/TraceOperator.h"

class TrihedralConormalDecompositionComputer : public ConormalDecompositionComputer {
public:
  /**
   * @details Algorithm 3.1 from https://doi.org/10.1515/RJNAMM.2009.014
   * @param[in] dg unified view of the geometry
   * @param[in] traceOp trace operator
   * @param[in] conductivity tensor values in each cell
   * @param[in] group cell group on which the decomposition should be computed
   */
  TrihedralConormalDecompositionComputer(DiscretizationGeometry& dg,
                                         TraceOperator& traceOp,
                                         const Arcane::VariableCellReal3x3& conductivity) ;

};


#endif //GREEDYCONORMALDECOMPOSITIONCOMPUTER_H
