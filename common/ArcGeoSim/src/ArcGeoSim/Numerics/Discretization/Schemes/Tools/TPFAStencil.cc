//
// Created by encheryg on 12/03/24.
//

#include "TPFAStencil.h"

TPFAStencil::TPFAStencil(const DiscretizationConnectivity& dc) : ADFiniteVolumeStencil(dc) {
#ifdef USE_ARCANE_V3
  Arcane::IItemFamily *faceFamily_asIItemFamily = m_dc.faceFamily()->itemFamily();
#else
  Arcane::IItemFamily *faceFamily_asIItemFamily = m_dc.faceFamily();
#endif

  m_stencils.resize(faceFamily_asIItemFamily->maxLocalId()) ;

  m_maxStencilSize = 0 ;
  ENUMERATE_DISCRETIZATION_FACE(iFace, m_dc.innerFaces()) {
    const auto faceLId(iFace->localId()) ;
    auto &localMap(m_stencils[faceLId]) ;
    localMap.clear() ;

    const auto& backCell(m_dc.backCell(iFace)) ;
    localMap.insert(std::make_pair(backCell, 0)) ;

    const auto& frontCell(m_dc.frontCell(iFace)) ;
    localMap.insert(std::make_pair(frontCell, 1)) ;

    m_maxStencilSize = std::max(m_maxStencilSize, localMap.size()) ;
  }

  ENUMERATE_DISCRETIZATION_FACE(iFace, m_dc.outerFaces()) {
    const auto faceLId(iFace->localId());
    auto &localMap(m_stencils[faceLId]);
    localMap.clear();

    const auto &boundaryCell(m_dc.boundaryCell(iFace));
    localMap.insert(std::make_pair(boundaryCell, 0)) ;

    m_maxStencilSize = std::max(m_maxStencilSize, localMap.size()) ;
  }
}