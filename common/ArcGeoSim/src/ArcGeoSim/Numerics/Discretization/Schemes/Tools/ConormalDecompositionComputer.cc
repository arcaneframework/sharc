//
// Created by encheryg on 13/01/24.
//

#include <array>
#include "ConormalDecompositionComputer.h"

ConormalDecompositionComputer::ConormalDecompositionComputer(DiscretizationGeometry& dg) : m_dg(dg) {
    const auto dc(m_dg.discretizationConnectivity()) ;
    m_maxNbFaces = 0 ;

    ENUMERATE_DISCRETIZATION_CELL(iCell, dc -> allCells()) {
      const Discretization::ConnectedItems faces(dc -> faces(iCell)) ;

      m_maxNbFaces = Arcane::math::max(m_maxNbFaces, faces.size());
    }

#ifdef USE_ARCANE_V3
    m_coeffs.resize(dc -> faceFamily()->itemFamily() -> maxLocalId()) ;
#else
    m_coeffs.resize(dc -> faceFamily() -> maxLocalId()) ;
#endif
    ENUMERATE_DISCRETIZATION_FACE(iFace, dc -> innerFaces()) {
      const auto faceLId(iFace->localId()) ;

      const auto& backCell= dc -> backCell(iFace) ;

      const Discretization::ConnectedItems backFaces(dc -> faces(backCell)) ;

      const auto nbBackFaces(backFaces.size()) ;

      m_coeffs[faceLId][static_cast<int>(Discretization::FaceCellInd::Back)].resize(nbBackFaces) ;

      const auto& frontCell= dc -> frontCell(iFace) ;

      const Discretization::ConnectedItems frontFaces(dc -> faces(frontCell)) ;

      const auto nbFrontFaces(frontFaces.size()) ;

      m_coeffs[faceLId][static_cast<int>(Discretization::FaceCellInd::Front)].resize(nbFrontFaces) ;
    }

    ENUMERATE_DISCRETIZATION_FACE(iFace, dc -> outerFaces()) {
      const auto faceLId(iFace->localId()) ;

      const auto& boundaryCell= dc -> boundaryCell(iFace) ;

      Discretization::ConnectedItems boundaryFaces(dc -> faces(boundaryCell)) ;

      const auto nbBoundaryFaces(boundaryFaces.size()) ;

      m_coeffs[faceLId][static_cast<int>(Discretization::FaceCellInd::Back)].resize(nbBoundaryFaces) ;
    }
}

Arcane::Real ConormalDecompositionComputer::coeff(const Discretization::Face& disFace, const Discretization::FaceCellInd ind,
                                                  const Arcane::Integer index) const {
  const auto lId(disFace.localId()) ;
  std::array<Arcane::RealUniqueArray,2> faceCoeffs;
  faceCoeffs = m_coeffs[lId] ;
  const auto iind(ind == Discretization::FaceCellInd::Back ? 0 : 1) ;
  Arcane::RealUniqueArray cellFaceCoeffs(faceCoeffs[iind]) ;
  return cellFaceCoeffs[index] ;
}

Arcane::Real ConormalDecompositionComputer::coeff(const Arcane::Face& face, const Discretization::FaceCellInd ind,
                                                  const Arcane::Integer index) const {
  return coeff(m_dg.discretizationConnectivity() -> face(face), ind, index) ;
}

Arcane::Real ConormalDecompositionComputer::coeff(const Contact& c, const Discretization::FaceCellInd ind,
                                                  const Arcane::Integer index) const {
  return coeff(m_dg.discretizationConnectivity() -> face(c), ind, index) ;
}

