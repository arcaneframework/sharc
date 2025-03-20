//
// Created by encheryg on 15/01/24.
//

#include "ConormalDecompositionStencil.h"

ConormalDecompositionStencil::ConormalDecompositionStencil(const DiscretizationConnectivity& dc,
                                                           const ConormalDecompositionFaces& cdf)
                                                           : ADFiniteVolumeStencil(dc){
#ifdef USE_ARCANE_V3
  Arcane::IItemFamily* faceFamily(m_dc.faceFamily()->itemFamily());
#else
  Arcane::IItemFamily* faceFamily(m_dc.faceFamily());
#endif
  m_stencils.resize(faceFamily->maxLocalId()) ;

  m_maxStencilSize = 0 ;

  ENUMERATE_DISCRETIZATION_FACE(iFace, m_dc.innerFaces()) {
    const auto& backCell(m_dc.backCell(iFace)) ;

    const Discretization::ConnectedItems backFaces(m_dc.faces(backCell)) ;

    const auto& frontCell(m_dc.frontCell(iFace)) ;

    const Discretization::ConnectedItems frontFaces(m_dc.faces(frontCell)) ;

    const std::array<Discretization::ConnectedItems, 2> localFaceGroups({std::ref(backFaces),
                                                                         std::ref(frontFaces)}) ;

    std::set<Discretization::Cell> localCellSet ;
    localCellSet.clear() ;

    for (auto ind: {Discretization::FaceCellInd::Back, Discretization::FaceCellInd::Front}) {
      const auto intInd(static_cast<int>(ind)) ;
      for(auto j : cdf.indices(*iFace, ind)) {
        const auto& jFace(localFaceGroups[intInd][j]) ;

        if (m_dc.isSubDomainBoundary(jFace)) {
          const auto &boundaryCell(m_dc.boundaryCell(jFace)) ;
          localCellSet.insert(boundaryCell) ;
        } else {
          const auto &stencilBackCell(m_dc.backCell(jFace)) ;
          localCellSet.insert(stencilBackCell) ;
          const auto &stencilFrontCell(m_dc.frontCell(jFace)) ;
          localCellSet.insert(stencilFrontCell) ;
        }
      }
    }

    const auto faceLId(iFace->localId()) ;
    auto &localMap(m_stencils[faceLId]) ;
    localMap.clear() ;
    Arcane::Integer localCount(0) ;
    for (auto cell: localCellSet) {
      localMap.insert(std::make_pair(cell, localCount)) ;
      ++localCount ;
    }
    m_maxStencilSize = std::max(m_maxStencilSize, localMap.size()) ;
  }

  ENUMERATE_DISCRETIZATION_FACE(iFace, m_dc.outerFaces()) {
    const auto &boundaryCell(m_dc.boundaryCell(iFace));

    const Discretization::ConnectedItems boundaryFaces(m_dc.faces(boundaryCell));

    std::set<Discretization::Cell> localCellSet;
    localCellSet.clear();

    for (auto ind: {Discretization::FaceCellInd::Back, Discretization::FaceCellInd::Front}) {
      for(auto j : cdf.indices(*iFace, ind)) {
        const auto& jFace(boundaryFaces[j]) ;

        if (m_dc.isSubDomainBoundary(jFace)) {
          const auto &stencilBoundaryCell(m_dc.boundaryCell(jFace)) ;
          localCellSet.insert(stencilBoundaryCell) ;
        } else {
          const auto &stencilBackCell(m_dc.backCell(jFace)) ;
          localCellSet.insert(stencilBackCell) ;
          const auto &stencilFrontCell(m_dc.frontCell(jFace)) ;
          localCellSet.insert(stencilFrontCell) ;
        }
      }
    }

    const auto faceLId(iFace->localId());
    auto &localMap(m_stencils[faceLId]);
    localMap.clear();
    Arcane::Integer localCount(0);
    for (auto cell: localCellSet) {
      localMap.insert(std::make_pair(cell, localCount));
      ++localCount;
    }
    m_maxStencilSize = std::max(m_maxStencilSize, localMap.size()) ;
  }
}
