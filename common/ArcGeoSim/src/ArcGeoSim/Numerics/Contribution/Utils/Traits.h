// -*- C++ -*-
#pragma once

#include "ArcGeoSim/Numerics/Contribution/Utils/ContainerKind.h"
#include "ArcGeoSim/Numerics/Contribution/Utils/ItemKind.h"

#include "ArcGeoSim/Numerics/Contribution/Variable/Variable.h"
#include "ArcGeoSim/Numerics/Contribution/Variable/PartialVariable.h"
#include "ArcGeoSim/Numerics/Contribution/Variable/VariableArray.h"
#include "ArcGeoSim/Numerics/Contribution/Variable/VariableScalar.h"


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcNum {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


template<ItemKind::eType IK>
struct ItemTraits;

template<>
struct ItemTraits<ItemKind::Node>
{
  typedef Arcane::Node Type;
};

template<>
struct ItemTraits<ItemKind::Cell>
{
  typedef Arcane::Cell Type;
};

template<>
struct ItemTraits<ItemKind::Face>
{
  typedef Arcane::Face Type;
};

template<>
struct ItemTraits<ItemKind::IntegerIndex>
{
  typedef Arcane::Integer Type;
};

template<>
struct ItemTraits<ItemKind::None>
{
  typedef void Type;
};

/*---------------------------------------------------------------------------*/

template<ItemKind::eType IK>
struct SupportTraits
{
  typedef Arcane::ItemGroupT<typename ItemTraits<IK>::Type> Type;
};

template<>
struct SupportTraits<ItemKind::IntegerIndex>
{
  typedef Range Type;
};

template<>
struct SupportTraits<ItemKind::None>
{
  struct NoSupport {};

  typedef NoSupport Type;
};

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

template<ContainerKind::eType CK, ItemKind::eType IK>
struct ValuesContainerTraits;

template<ItemKind::eType IK>
struct ValuesContainerTraits<ContainerKind::Variable, IK>
{
  typedef Arcane::MeshVariableScalarRefT<typename ItemTraits<IK>::Type,Arcane::Real> Type;
};

template<ItemKind::eType IK>
struct ValuesContainerTraits<ContainerKind::PartialVariable, IK>
{
  typedef Arcane::MeshPartialVariableScalarRefT<typename ItemTraits<IK>::Type,Arcane::Real> Type;
};

template<>
struct ValuesContainerTraits<ContainerKind::Array, ItemKind::IntegerIndex>
{
  typedef Arcane::VariableRefArrayT<Arcane::Real> Type;
};

template<>
struct ValuesContainerTraits<ContainerKind::Scalar, ItemKind::None>
{
  typedef Arcane::VariableRefScalarT<Arcane::Real> Type;
};

/*---------------------------------------------------------------------------*/

template<ContainerKind::eType CK, ItemKind::eType IK>
struct DerivativesContainerTraits;

template<ItemKind::eType IK>
struct DerivativesContainerTraits<ContainerKind::Variable, IK>
{
  typedef Arcane::MeshVariableArrayRefT<typename ItemTraits<IK>::Type,Arcane::Real> Type;
};

template<ItemKind::eType IK>
struct DerivativesContainerTraits<ContainerKind::PartialVariable, IK>
{
  typedef Arcane::MeshPartialVariableArrayRefT<typename ItemTraits<IK>::Type,Arcane::Real> Type;
};

template<>
struct DerivativesContainerTraits<ContainerKind::Array, ItemKind::IntegerIndex>
{
  typedef Arcane::VariableRefArray2T<Arcane::Real> Type;
};

template<>
struct DerivativesContainerTraits<ContainerKind::Scalar, ItemKind::None>
{
  typedef Arcane::VariableRefArrayT<Arcane::Real> Type;
};

/*---------------------------------------------------------------------------*/

template<ContainerKind::eType CK, ItemKind::eType IK>
struct ContributionContainerTraits;

template<ItemKind::eType IK>
struct ContributionContainerTraits<ContainerKind::Variable, IK>
{
  typedef audi::Variable<typename ItemTraits<IK>::Type> Type;
};

template<ItemKind::eType IK>
struct ContributionContainerTraits<ContainerKind::PartialVariable, IK>
{
  typedef audi::PartialVariable<typename ItemTraits<IK>::Type> Type;
};

template<>
struct ContributionContainerTraits<ContainerKind::Array, ItemKind::IntegerIndex>
{
  typedef audi::VariableArray Type;
};

template<>
struct ContributionContainerTraits<ContainerKind::Scalar, ItemKind::None>
{
  typedef audi::VariableScalar Type;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
}

