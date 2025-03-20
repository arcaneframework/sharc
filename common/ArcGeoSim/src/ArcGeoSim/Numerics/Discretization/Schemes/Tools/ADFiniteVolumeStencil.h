//
// Created by encheryg on 12/03/24.
//

#ifndef ADFINITEVOLUMESTENCIL_H
#define ADFINITEVOLUMESTENCIL_H

//#include "ArcGeoSim/Physics/Law2/Contribution/ContributionAccessor.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/MultiPointsStencil.h"
#include "ArcGeoSim/Numerics/Discretization/DiscretizationConnectivity.h"

class ADFiniteVolumeStencil : public ArcNum::MultiPointsStencil {
public:
  ADFiniteVolumeStencil(const DiscretizationConnectivity& dc) ;

  void init(const Discretization::Face& f) ;
  void init(const Arcane::Face& f) ;
  void init(const Contact& f) ;
  void init(const Discretization::FaceEnumerator& f) ;
  void init(const Arcane::FaceEnumerator& f) ;
  void init(const ContactEnumerator& f) ;
  void begin() ;
  void next() ;
  bool end() ;
  const ArcNum::Stencil::Cell& cell() ;
  size_t size() const ;
  size_t maxSize() const ;
  const ArcNum::Stencil::Cell& backBoundaryCell() ;
  const ArcNum::Stencil::Cell& frontCell() ;
  void finalize() ;

protected:
  void _init(const Discretization::Face& f) ;
  void _checkInit() const ;
  void _createCell() ;

  Arcane::UniqueArray<std::map<Discretization::Cell, Arcane::Integer, std::less<Arcane::DoF>>> m_stencils ;
  const DiscretizationConnectivity& m_dc ;
  bool m_initDone = false ;
  ArcNum::Stencil::Cell m_cell ;
  ArcNum::Stencil::Cell m_backBoundaryCell ;
  ArcNum::Stencil::Cell m_frontCell ;
  std::map<Discretization::Cell, Arcane::Integer>::iterator m_stencilCellIterator ;
  Discretization::Face m_stencilFace ;
  std::size_t m_maxStencilSize = 0 ;
};


#endif //ADFINITEVOLUMESTENCIL_H
