//
// Created by encheryg on 20/12/23.
//

#ifndef TRACEOPERATOR_H
#define TRACEOPERATOR_H

#include <vector>
#include <array>

#include <arcane/utils/ArcaneGlobal.h>
#include <arcane/utils/Real3.h>
#include <arcane/ItemTypes.h>

#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"

class TraceOperator {
public:
  /**
   * @param face or contact
   * @return the point where the trace value is evaluated
   */
  const Arcane::Real3& point(const Discretization::Face& disFace) const ;
  const Arcane::Real3& point(const Discretization::FaceEnumerator& disFaceE) const ;
  const Arcane::Real3& point(const Arcane::Face& face) const ;
  const Arcane::Real3& point(const Arcane::FaceEnumerator& face) const ;
  const Arcane::Real3& point(const Contact& c) const ;
  /**
   * @param face
   * @brief To start iterating on the cells used to reconstruct the trace value
   */
  void begin(const Discretization::Face& disFace) ;
  void begin(const Discretization::FaceEnumerator& disFaceE) ;
  void begin(const Arcane::Face& face) ;
  void begin(const Arcane::FaceEnumerator& fe) ;
  void begin(const Contact& c) ;
  void next() ;
  bool end() ;
  const Arcane::Cell& cell() ;
  Arcane::Real weight() ;

protected:
  TraceOperator(DiscretizationGeometry& dg) : m_dg(dg) {} ;

  DiscretizationGeometry& m_dg ;

  Arcane::Real3UniqueArray m_centers ;

  using ArcCellRealPairVector = Arcane::UniqueArray<std::pair<Arcane::Cell, Arcane::Real>> ;
  /// Weights used for the centers or for the approximated trace
  Arcane::UniqueArray<ArcCellRealPairVector> m_weights ;

  ArcCellRealPairVector::const_iterator m_weightsIterator ;
  ArcCellRealPairVector::const_iterator m_weightsEndIterator ;
};


#endif
