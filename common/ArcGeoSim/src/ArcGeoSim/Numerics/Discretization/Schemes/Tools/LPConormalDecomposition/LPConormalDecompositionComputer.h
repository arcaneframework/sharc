//
// Created by encheryg on 13/01/24.
//

#ifndef LPCONORMALDECOMPOSITIONCOMPUTER_H
#define LPCONORMALDECOMPOSITIONCOMPUTER_H

#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/ConormalDecompositionComputer.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/TraceOperator.h"

class LPConormalDecompositionComputer : public ConormalDecompositionComputer {
public:
  /** @details Based on formulas (43)-(44-(45) in 10.1016/j.jcp.2017.09.003
   * @param[in] dg unified view of the geometry
   * @param[in] traceOp trace operator
   * @param[in] conductivity tensor values in each cell
   * @param[in] group cell group on which the decomposition should be computed
   */
  LPConormalDecompositionComputer(DiscretizationGeometry& dg,
                                  TraceOperator& traceOp,
                                  const Arcane::VariableCellReal3x3& conductivity) ;
};


#endif //LPCONORMALDECOMPOSITION_H
