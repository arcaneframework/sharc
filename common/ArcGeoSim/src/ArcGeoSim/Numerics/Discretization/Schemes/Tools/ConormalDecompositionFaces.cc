//
// Created by encheryg on 18/01/24.
//

#include "ConormalDecompositionFaces.h"

ConormalDecompositionFaces::ConormalDecompositionFaces(const DiscretizationConnectivity& dc,
                                                       const ConormalDecompositionComputer& cdc,
                                                       const Arcane::Real maxRatioThreshold)
                                                       : m_dc(dc) {
#ifdef USE_ARCANE_V3
  m_faceIndices.resize(m_dc.faceFamily()->itemFamily()->maxLocalId()) ;
#else
  m_faceIndices.resize(m_dc.faceFamily()->maxLocalId()) ;
#endif

  const auto _selectStencilFaces =
      [&cdc, this, maxRatioThreshold](const auto& face, const auto& group, const auto ind) {
    auto alphaAbsMax(0.) ;
    ENUMERATE_DISCRETIZATION_FACE(jFace, group) {
      const auto alphaAbs(std::fabs(cdc.coeff(face, ind, jFace.index()))) ;

      if (alphaAbs > alphaAbsMax) {
        alphaAbsMax = alphaAbs ;
      }
    }

    auto& indices(m_faceIndices[face.localId()][static_cast<int>(ind)]) ;
    ENUMERATE_DISCRETIZATION_FACE(jFace, group) {
      const auto alphaAbs(std::fabs(cdc.coeff(face, ind, jFace.index()))) ;

      if (alphaAbs > maxRatioThreshold * alphaAbsMax) {
        indices.add(jFace.index()) ;
      }
    }
  };

  ENUMERATE_DISCRETIZATION_FACE(iFace, m_dc.innerFaces()) {
    const auto& backCell(m_dc.backCell(iFace)) ;

    const Discretization::ConnectedItems backFaces(m_dc.faces(backCell)) ;

    const auto& frontCell(m_dc.frontCell(iFace)) ;

    const Discretization::ConnectedItems frontFaces(m_dc.faces(frontCell)) ;

    const std::array<Discretization::ConnectedItems, 2> localFaceGroups({std::ref(backFaces),
                                                                         std::ref(frontFaces)}) ;

    for (auto ind: {Discretization::FaceCellInd::Back, Discretization::FaceCellInd::Front}) {
      const auto intInd(static_cast<int>(ind)) ;
      _selectStencilFaces(*iFace, localFaceGroups[intInd], ind) ;
    }
  }

  ENUMERATE_DISCRETIZATION_FACE(iFace, m_dc.outerFaces()) {
    const auto &boundaryCell(m_dc.boundaryCell(iFace));

    const Discretization::ConnectedItems boundaryFaces(m_dc.faces(boundaryCell));

    _selectStencilFaces(*iFace, boundaryFaces, Discretization::FaceCellInd::Back) ;
  }
}

const Arcane::IntegerUniqueArray& ConormalDecompositionFaces::indices(const Discretization::Face& face,
                                                                      const Discretization::FaceCellInd& ind) const {
  return m_faceIndices[face.localId()][static_cast<int>(ind)] ;
}

const Arcane::IntegerUniqueArray& ConormalDecompositionFaces::indices(const Arcane::Face& face,
                                                                        const Discretization::FaceCellInd& ind) const {
  return indices(m_dc.face(face), ind) ;
}

const Arcane::IntegerUniqueArray& ConormalDecompositionFaces::indices(const Contact& c,
                                                                      const Discretization::FaceCellInd& ind) const {
  return indices(m_dc.face(c), ind) ;
}
