#ifndef LAW_CONTRIBUTION_H
#define LAW_CONTRIBUTION_H


#include "ArcGeoSim/Numerics/AuDi/audi.h"
#include "ArcGeoSim/Numerics/Contribution/Contribution.h"
#include "ArcGeoSim/Numerics/Contribution/Stencil.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

  using Contribution = ::audi::sparse_expression<Arcane::Real>;
  using RootContribution = ::audi::scalar_wrapper<Arcane::Real>;
  using DenseContribution = ::audi::scalar<Arcane::Real>;
  constexpr auto initContribution = &::audi::pool_reset<Arcane::Real>;

  using Item = ArcNum::Stencil::Item ;

  template<typename T>
  using ItemT = ArcNum::Stencil::ItemT<T> ;

  using IntegerIndex = ItemT<Arcane::Integer> ;

END_LAW_NAMESPACE
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* LAW_CONTRIBUTION_H */
