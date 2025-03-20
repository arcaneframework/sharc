//
// Created by encheryg on 05/01/24.
//

#include "HarmonicAverageTraceOperator.h"

HarmonicAverageTraceOperator::HarmonicAverageTraceOperator(DiscretizationGeometry& dg,
                                                           const Arcane::VariableCellReal3x3& conductivity) : TraceOperator(dg) {
  DiscretizationCellVariableRefTReaderWrapper<Arcane::Real3x3> varWrapper(*dg.discretizationConnectivity(), conductivity) ;

  const Centers<Discretization::Face> faceCenters(&m_dg) ;
  const OrientedUnitNormals<Discretization::Face> normals(&m_dg) ;
  const Centers<Discretization::Cell> cellCenters(&m_dg) ;

  const auto dc(m_dg.discretizationConnectivity()) ;

  const auto backInd{Discretization::FaceCellInd::Back} ;
  const auto frontInd{Discretization::FaceCellInd::Front} ;

  const auto backInt = (int)backInd ;
  const auto frontInt = (int)frontInd ;

#ifdef USE_ARCANE_V3
  const auto faceMaxLId(dc -> faceFamily()->itemFamily()->maxLocalId()) ;
#else
  const auto faceMaxLId(dc -> faceFamily()->maxLocalId()) ;
#endif
  m_weights.resize(faceMaxLId) ;
  m_centers.resize(faceMaxLId) ;

  ENUMERATE_DISCRETIZATION_FACE(iFace, dc -> innerFaces()) {
    Arcane::Real nLambdaNs[2] ;
    Arcane::Real dists[2] ;
    Arcane::Real3 projCellCenters[2] ;
    Arcane::Real3 diffLambdas[2] ;
    const auto& backCell = dc -> backCell(iFace) ;
    const auto& frontCell= dc -> frontCell(iFace) ;
    for(const auto ind : {backInd, frontInd}) {
      const auto& cell= (ind == backInd) ? backCell : frontCell ;
      const auto i((int)ind) ;

      const auto& cellCenter= cellCenters(cell) ;

      const auto& outUnitNormal= normals(iFace, ind) ;
      const auto& faceCenter = faceCenters(iFace, ind) ;

      dists[i] = DiscretizationGeometry::hyperplaneDistance(outUnitNormal, cellCenter,faceCenter) ;

      const auto& lambda = varWrapper[cell] ;

      nLambdaNs[i] = Arcane::math::scaMul(outUnitNormal, Arcane::math::prodTensVec(lambda,outUnitNormal)) ;

      projCellCenters[i] = cellCenter + dists[i] * outUnitNormal ;

      diffLambdas[i] = Arcane::math::prodTensVec(lambda,normals(iFace, backInd)) -
          nLambdaNs[i] * normals(iFace, backInd) ;
    }

    const Arcane::Real coeffs[]{dists[frontInt] * nLambdaNs[backInt],dists[backInt] * nLambdaNs[frontInt]} ;
    const auto coeffsSum(coeffs[backInt] + coeffs[frontInt]) ;

    const auto lId(iFace -> localId()) ;
    m_centers[lId] = Arcane::Real3::zero() ;
    m_weights[lId].resize(2) ;
    for(const auto ind : {backInd, frontInd}) {
      const auto i((int)ind) ;
      const auto& cell =(ind == backInd) ? dc -> cell(backCell) : dc -> cell(frontCell) ;
      m_weights[lId][i] = std::make_pair(cell, coeffs[i] / coeffsSum) ;
      m_centers[lId] += m_weights[lId][i].second * projCellCenters[i] ;
    }

     m_centers[lId] += dists[backInt] * dists[frontInt] / coeffsSum * (diffLambdas[backInt] - diffLambdas[frontInt]) ;
  }

  ENUMERATE_DISCRETIZATION_FACE(iFace, dc -> outerFaces()) {
    const auto lId(iFace -> localId()) ;

    const auto& cell= dc -> boundaryCell(iFace) ;

    m_weights[lId].resize(1) ;
    m_weights[lId][0] = std::make_pair(dc -> cell(cell), 1.) ;

    const auto& outUnitNormal= normals(iFace, Discretization::FaceCellInd::Back) ;
    const auto& cellCenter= cellCenters(cell) ;
    const auto& faceCenter= faceCenters(iFace, Discretization::FaceCellInd::Back) ;

    const auto& lambda= varWrapper[cell] ;
    const auto nLambdaN(Arcane::math::scaMul(outUnitNormal,
                                              Arcane::math::prodTensVec(lambda,outUnitNormal))) ;
    const auto dist(DiscretizationGeometry::hyperplaneDistance(outUnitNormal,
                                                               cellCenter,faceCenter)) ;

    m_centers[lId] = cellCenter + dist / nLambdaN * Arcane::math::prodTensVec(lambda,outUnitNormal) ;
    // Choice made in the previous version of the nltpfa scheme
    //m_centers[lId] = cellCenter + dist * outUnitNormal ;
  }
}