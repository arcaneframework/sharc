//
// Created by encheryg on 12/03/24.
//

#ifndef ADCELLCENTEREDFINITEVOLUMESCHEME_H
#define ADCELLCENTEREDFINITEVOLUMESCHEME_H

#include <arcane/utils/ArcaneGlobal.h>
#include <arcane/ItemTypes.h>
#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Numerics/Contribution/Contribution.h"
#include "ArcGeoSim/Numerics/Contribution/Variable/Variable.h"
#include "ArcGeoSim/Numerics/Contribution/Variable/PartialVariable.h"
#include "ArcGeoSim/Numerics/Discretization/Schemes/Tools/ADFiniteVolumeStencil.h"

namespace ADFiniteVolumeUtils {
  template<typename T>
  struct CellTrait {
    using type = Arcane::Cell;

    CellTrait(const Arcane::Cell &cell, const std::size_t size,
              const Arcane::Integer index) :
        m_cell(cell) {}

    type m_cell;
  };

  template<>
  struct CellTrait<ArcNum::Contribution> {
    using type = ArcNum::Stencil::Cell;

    CellTrait(const Arcane::Cell &cell, const std::size_t size,
              const Arcane::Integer index) :
        m_cell(cell, size, index) {}

    type m_cell;
  };

  template<typename T>
  struct VariableTrait {
    using type = Arcane::VariableCellReal;
  };

  template<>
  struct VariableTrait<ArcNum::Contribution> {
    using type = ArcNum::audi::Variable<Arcane::Cell>;
  };

  template<typename T>
  struct PartialVariableTrait {
    using type = Arcane::PartialVariableCellReal;
  };

  template<>
  struct PartialVariableTrait<ArcNum::Contribution> {
    using type = ArcNum::audi::PartialVariable<Arcane::Cell>;
  };

  template<typename T>
  struct ValueTrait {
    static Arcane::Real value(const T &v) { return v; }
  } ;

  template<>
  struct ValueTrait<ArcNum::Contribution> {
    static Arcane::Real value(const ArcNum::Contribution& v) { return v.value() ; }
  } ;
}

class ADFiniteVolumeScheme {
public:
  enum Weight {TPFA, NLTPFA, NLMPFA, AVGMPFA} ;

  virtual void computeFlux(const Arcane::Face& f,
                           const Arcane::VariableCellReal& u,
                           const Arcane::PartialVariableFaceReal& pu,
                           Arcane::Real& flux,
                           const Arcane::Real* gravityPtr = nullptr,
                           const std::shared_ptr<const Arcane::PartialVariableCellReal>& puPtr = nullptr,
                           const std::shared_ptr<const Arcane::PartialVariableFaceBool>& useCellPUPtr = nullptr) = 0 ;

  virtual void computeFlux(const Arcane::Face& f,
                           const ArcNum::audi::Variable<Arcane::Cell>& u,
                           const Arcane::PartialVariableFaceReal& pu,
                           ArcNum::Contribution& flux,
                           const ArcNum::Contribution* gravityPtr = nullptr,
                           const std::shared_ptr<const ArcNum::audi::PartialVariable<Arcane::Cell>>& puPtr = nullptr,
                           const std::shared_ptr<const Arcane::PartialVariableFaceBool>& useCellPUPtr = nullptr) = 0 ;

  virtual void computeFlux(const FaceFaceContact& f,
                           const ArcNum::audi::Variable<Arcane::Cell>& u,
                           const Arcane::PartialVariableFaceReal& pu,
                           ArcNum::Contribution& flux,
                           const ArcNum::Contribution* gravityPtr = nullptr,
                           const std::shared_ptr<const ArcNum::audi::PartialVariable<Arcane::Cell>>& puPtr = nullptr,
                           const std::shared_ptr<const Arcane::PartialVariableFaceBool>& useCellPUPtr = nullptr) = 0 ;

  virtual std::shared_ptr<ADFiniteVolumeStencil> stencil() = 0 ;
};

#endif //ADCELLCENTEREDFINITEVOLUMESCHEME_H
