// -*- C++ -*-
#pragma once

#include "ArcGeoSim/Numerics/Contribution/Variable/BaseVariable.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcNum {
BEGIN_AUDI_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
template<typename K>
class ScalarPartialVarProperty
{
public :
  typedef Arcane::MeshVariableScalarRefT<K,Arcane::Real> VarType ;
  typedef Arcane::MeshVariableArrayRefT<K,Arcane::Real>  DerivVarType;


  typedef typename VarType::ConstReturnReferenceType      ConstReturnReferenceType;
  typedef typename DerivVarType::ConstReturnReferenceType DerivativesReturnType;

  ScalarPartialVarProperty()
  {}

  ScalarPartialVarProperty(VarType* values)
  : m_values(values)
  {}


  ScalarPartialVarProperty(ScalarPartialVarProperty const& rhs)
  : m_values(rhs.m_values)
  , m_derivatives(rhs.m_derivatives)
  {}

  template<typename ItemT>
  ConstReturnReferenceType operator[](ItemT const& iter) const {
    const auto& values = *m_values;
#ifdef USE_ARCANE_V3
    if constexpr (std::is_same_v<ItemT,Integer>)
                   return values[Arcane::ItemLocalId(iter)];
    else
      return values[iter.internal()];
#else
    return values[iter];
#endif
  }

  template<typename I>
  inline DerivativesReturnType storedDerivatives(const I& iter) const {
    const auto& derivatives = *m_derivatives;
#ifdef USE_ARCANE_V3
    if constexpr (std::is_same_v<I,Integer>)
      return derivatives[Arcane::ItemLocalId(iter)];
    else
      return derivatives[iter.internal()];
#else
    return derivatives[iter];
#endif
  }

protected:
  VarType* m_values = nullptr;
  DerivVarType* m_derivatives = nullptr ;
};


template<typename K>
class PartialVariable
: public ScalarPartialVarProperty<K>
, private BaseVariable
{
public:

  typedef ScalarPartialVarProperty<K> Base ;
  typedef typename Base::VarType      VarType ;
  typedef typename Base::DerivVarType DerivVarType ;

  PartialVariable() {}

  PartialVariable(const PartialVariable& rhs)
  : Base(rhs)
  , BaseVariable(rhs) {}


  template<typename MappingT>
  PartialVariable(ConstArrayView<MappingT> offsets)
  : Base()
  , BaseVariable(offsets)
  {}

  virtual ~PartialVariable() {}

  void setValues(VarType* values)
  {
    this->m_values = values ;
  }

  void setDerivValues(DerivVarType* values)
  {
    this->m_derivatives = values ;
  }
public:

  inline const RootContribution& operator[](const K& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::Item"));
    return _contribution(item, 0);
  }

  inline const RootContribution& operator[](const Arcane::ItemEnumeratorT<K>& iter) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::ItemEnumeratorT<Arcane::Item>"));
    return _contribution(iter, 0);
  }

  inline const RootContribution& operator[](const ArcNum::Stencil::ItemT<K>& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() >= item.size()),("error, Law::Item size not suitable"));
    const Arcane::Integer index = item.index();
    return _contribution(item, index);
  }

private:
  template<typename ItemT>
  inline const RootContribution& _contribution(const ItemT& item, const Arcane::Integer& index) const
  {
    // sparse ad wrapper affectation
    RootContribution& ad_value = m_ad_value[index].first;
    ad_value.value() = Base::operator[](item);
    bool update_derivatives = m_ad_value[index].second;
    if(update_derivatives){
      auto* derivates = Base::storedDerivatives(item).unguardedBasePointer();
      ad_value.setGradient(derivates);
    }
    return ad_value;
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_AUDI_NAMESPACE
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

