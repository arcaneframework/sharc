//
// Created by encheryg on 12/03/24.
//

#include "ADFiniteVolumeStencil.h"

ADFiniteVolumeStencil::ADFiniteVolumeStencil(const DiscretizationConnectivity& dc) : m_dc(dc), m_initDone(false) {}

void ADFiniteVolumeStencil::init(const Discretization::Face& f) {_init(f) ;}
void ADFiniteVolumeStencil::init(const Arcane::Face& f) {_init(m_dc.face(f)) ;}
void ADFiniteVolumeStencil::init(const Contact& c) {_init(m_dc.face(c)) ;}
void ADFiniteVolumeStencil::init(const Discretization::FaceEnumerator & fe) {init(*fe) ;}
void ADFiniteVolumeStencil::init(const Arcane::FaceEnumerator& fe) {init(*fe) ;}
void ADFiniteVolumeStencil::init(const ContactEnumerator& ce) {init(*ce) ;}
void ADFiniteVolumeStencil::begin() {_checkInit() ;
  m_stencilCellIterator = m_stencils[m_stencilFace.localId()].begin() ; _createCell() ;}
void ADFiniteVolumeStencil::next() {_checkInit() ; ++m_stencilCellIterator ; _createCell() ;}
bool ADFiniteVolumeStencil::end() {return m_stencilCellIterator == m_stencils[m_stencilFace.localId()].end() ; }
const ArcNum::Stencil::Cell& ADFiniteVolumeStencil::cell() {_checkInit() ; return m_cell ;}
size_t ADFiniteVolumeStencil::size() const {_checkInit() ; return m_stencils[m_stencilFace.localId()].size() ;}
size_t ADFiniteVolumeStencil::maxSize() const {return m_maxStencilSize ;}
const ArcNum::Stencil::Cell& ADFiniteVolumeStencil::backBoundaryCell() {_checkInit() ; return m_backBoundaryCell ;}
const ArcNum::Stencil::Cell& ADFiniteVolumeStencil::frontCell() {_checkInit() ; return m_frontCell ;}
void ADFiniteVolumeStencil::finalize() {_checkInit() ; m_initDone = false ;}

void ADFiniteVolumeStencil::_init(const Discretization::Face& f) {
  auto& stencil(m_stencils[f.localId()]) ;
  const auto stencilSize(stencil.size()) ;
  if(m_dc.isCellGroupBoundary(f) || m_dc.isSubDomainBoundary(f)) {
    const auto& disBoundaryCell(m_dc.boundaryCell(f)) ;
    m_backBoundaryCell = ArcNum::Stencil::Cell(m_dc.cell(disBoundaryCell), stencilSize,
                                   stencil.at(disBoundaryCell)) ;
    m_frontCell = ArcNum::Stencil::Cell() ;
  } else {
    const auto& disBackCell(m_dc.backCell(f)) ;
    m_backBoundaryCell = ArcNum::Stencil::Cell(m_dc.cell(disBackCell), stencilSize, stencil.at(disBackCell)) ;
    const auto& disFrontCell(m_dc.frontCell(f)) ;
    m_frontCell = ArcNum::Stencil::Cell(m_dc.cell(disFrontCell), stencilSize, stencil.at(disFrontCell)) ;
  }
  m_stencilFace = f ;
  m_stencilCellIterator = stencil.begin() ;
  m_initDone = true ;
}

void ADFiniteVolumeStencil::_checkInit() const {
  if(!m_initDone)
    m_dc.mesh() -> traceMng() -> fatal() << "Stencil not initialized" ;
}

void ADFiniteVolumeStencil::_createCell() {
  if(not end()) {
    auto& stencil(m_stencils[m_stencilFace.localId()]) ;
    const auto& disCell(m_stencilCellIterator -> first) ;
    m_cell = ArcNum::Stencil::Cell(m_dc.cell(disCell), stencil.size(), stencil.at(disCell)) ;
  }
}
