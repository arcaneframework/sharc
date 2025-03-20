//
// Created by encheryg on 03/01/24.
//

#include <arcane/Item.h>
#include <arcane/ItemEnumerator.h>

#include "TraceOperator.h"

const Arcane::Real3& TraceOperator::point(const Discretization::Face& disFace) const {
  return m_centers[disFace.localId()] ;
}

const Arcane::Real3& TraceOperator::point(const Discretization::FaceEnumerator& disFaceE) const {
  return point(*disFaceE) ;
}

const Arcane::Real3& TraceOperator::point(const Arcane::Face& face) const {
  return point(m_dg.discretizationConnectivity() -> face(face)) ;
}

const Arcane::Real3& TraceOperator::point(const Arcane::FaceEnumerator& face) const {
  return point(*face) ;
}

const Arcane::Real3& TraceOperator::point(const Contact& c) const {
  return point(m_dg.discretizationConnectivity() -> face(c)) ;
}

void TraceOperator::begin(const Discretization::Face& disFace) {
  auto& ws(m_weights[disFace.localId()]) ;
  m_weightsIterator = ws.begin() ;
  m_weightsEndIterator = ws.end() ;
}

void TraceOperator::begin(const Discretization::FaceEnumerator& disFaceE) {
  begin(*disFaceE) ;
}

void TraceOperator::begin(const Arcane::Face& face) {
  begin(m_dg.discretizationConnectivity() -> face(face)) ;
}

void TraceOperator::begin(const Arcane::FaceEnumerator & fe) {begin(*fe) ;}

void TraceOperator::begin(const Contact& c) {
  begin(m_dg.discretizationConnectivity() -> face(c)) ;
}

void TraceOperator::next() {++m_weightsIterator ;}

bool TraceOperator::end() {return m_weightsIterator == m_weightsEndIterator ;}

const Arcane::Cell& TraceOperator::cell() {return m_weightsIterator -> first ;}

Arcane::Real TraceOperator::weight() {return m_weightsIterator -> second ;}
