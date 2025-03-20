//
// Created by encheryg on 05/01/24.
//

#ifndef HARMONICAVERAGETRACEOPERATOR_H
#define HARMONICAVERAGETRACEOPERATOR_H

#include "TraceOperator.h"

class HarmonicAverageTraceOperator : public TraceOperator {
public:
  /** @details Based on formulas (2) and (3) in doi:10.1016/j.crma.2009.03.013
   * @todo Implementation of the correction proposed in doi:10.2118/195694-PA (see formulas (34)-(35))
   */

  /*** @param[in] dg unified view of the geometry
  * @param[in] conductivity tensor values in each cell
  *
  */
  /*HarmonicAverageTraceOperator(DiscretizationGeometry &dg,
                               const Arcane::VariableDoFReal3x3 &conductivity);*/

  /** @param[in] dg unified view of the geometry
  * @param[in] conductivity tensor values in each cell
  */
  HarmonicAverageTraceOperator(DiscretizationGeometry &dg,
                               const Arcane::VariableCellReal3x3 &conductivity);
};

#endif //HARMONICAVERAGETRACEOPERATOR_H
