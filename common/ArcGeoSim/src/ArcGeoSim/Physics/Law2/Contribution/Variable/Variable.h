// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef LAW_CONTRIBUTION_VARIABLE_H
#define LAW_CONTRIBUTION_VARIABLE_H
/* Author : desrozis at Tue Nov 13 08:40:09 2012
 * Generated by createNew
 */

#include "ArcGeoSim/Physics/Law2/Variable.h"
#include "ArcGeoSim/Physics/Law2/VariableRef.h"

#include "ArcGeoSim/Physics/Law2/Contribution/Variable/BaseVariable.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE
BEGIN_AUDI_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#ifdef SPARSE_AUDI
template<typename K>
class Variable
    : public Law::VariableT< Law::VariableRefT<Law::ScalarRealProperty,K> >
    , private BaseVariable
{
public:

  typedef typename Law::VariableT< Law::VariableRefT<Law::ScalarRealProperty,K> > Base;
  typedef typename Base::DerivativesReturnType DerivType;

  Variable() {}

  Variable(const Variable& rhs)
  : Base(rhs)
  , BaseVariable(rhs) {}

  Variable(const Law::BuildInfo<Law::ScalarRealProperty>& bi)
  : Base(bi.property(), bi.offsets(), bi.variableMng().template variables<K>())
  , BaseVariable(bi.offsets()) {}

  virtual ~Variable() {}

public:

  inline const RootContribution& operator[](const K& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::Item"));
    return _contribution(item,0);
  }

  inline const RootContribution& operator[](const Arcane::ItemEnumeratorT<K>& iter) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::Item"));
    return _contribution(iter,0);
  }

  inline const RootContribution& operator[](const ItemT<K>& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == item.size()),("error, Law::Item size not suitable"));
    const Arcane::Integer index = item.index();
    return _contribution(item,index);
  }

private:
  template<typename ItemT>
  inline const RootContribution& _contribution(const ItemT& item, const Arcane::Integer& index) const
  {
    Arcane::ConstArrayView<Arcane::Integer> unknowns = m_unknowns_sparse[index].constView();
    // sparse ad wrapper affectation
    RootContribution& ad_value = m_ad_value[index];
    ad_value.value = Base::operator[](item);
    ad_value.size = unknowns.size();
    ad_value.indexes = unknowns.unguardedBasePointer();
    ad_value.gradient = (m_unknowns_identity_derivatives_sparse[index].empty()) ?
            Base::storedDerivatives(item).unguardedBasePointer() :
            m_unknowns_identity_derivatives_sparse[index].unguardedBasePointer() ;
    //
    return ad_value;
  }

};
#else
template<typename K>
class Variable
    : public Law::VariableT< Law::VariableRefT<Law::ScalarRealProperty,K> >
    , private BaseVariable
{
public:

  typedef Law::VariableT< Law::VariableRefT<Law::ScalarRealProperty,K> > Base;

  Variable() {}

  Variable(const Variable& rhs)
  : Base(rhs)
  , BaseVariable(rhs) {}

  Variable(const Law::BuildInfo<Law::ScalarRealProperty>& bi)
  : Base(bi.property(), bi.offsets(), bi.variableMng().template variables<K>())
  , BaseVariable(bi.offsets()) {}

  virtual ~Variable() {}

public:

  inline const Contribution& operator[](const K& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::Item"));
    return _contribution(item);
  }

  inline const Contribution& operator[](const Arcane::ItemEnumeratorT<K>& iter) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::Item"));
    return _contribution(iter);
  }

  inline const Contribution& operator[](const ItemT<K>& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == item.size()),("error, Law::Item size not suitable"));
    Arcane::Integer index = item.index();
    Contribution& ad_value = m_ad_value[index];
    ad_value = 0.;
    ad_value.value() = Base::operator[](item);
    const Arcane::Integer offset = m_unknowns_offsets[index];
    const Arcane::Integer size = m_unknowns_offsets[index+1] - offset;
    Arcane::RealArrayView d(size, ad_value.derivatives()+offset);
    Base::derivatives(item,index,d);
    return ad_value;
  }

private:
  template<typename ItemT>
  inline const Contribution& _contribution(const ItemT& item) const
  {
    Contribution& ad_value = m_ad_value[0];
    ad_value = 0.;
    ad_value.value() = Base::operator[](item);
    Arcane::RealArrayView d(ad_value.size(), ad_value.derivatives());
    Base::derivatives(item,0,d);
    return ad_value;
  }

};

#endif


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE
END_AUDI_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* LAW_CONTRIBUTION_VARIABLE_H */
