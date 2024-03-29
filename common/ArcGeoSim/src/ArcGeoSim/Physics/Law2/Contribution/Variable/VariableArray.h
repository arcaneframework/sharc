// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef LAW_CONTRIBUTION_VARIABLEARRAY_H
#define LAW_CONTRIBUTION_VARIABLEARRAY_H
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
class VariableArray
    : public Law::VariableT< Law::ArrayRefT<Law::ScalarRealProperty> >
    , private BaseVariable
{
public:

  typedef Law::VariableT< Law::ArrayRefT<Law::ScalarRealProperty> > Base;

  VariableArray() {}

  VariableArray(const VariableArray& rhs)
  : Base(rhs)
  , BaseVariable(rhs) {}

  VariableArray(const Law::BuildInfo<Law::ScalarRealProperty>& bi)
  : Base(bi.property(), bi.offsets(), bi.variableMng().arrays())
  , BaseVariable(bi.offsets()) {}

  virtual ~VariableArray() {}

public:

  inline const RootContribution& operator[](Arcane::Integer item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::Item instead of Geoxim::Item"));
    RootContribution& ad_value = m_ad_value[0];
    /*ad_value = 0.;
    ad_value.value() = Base::operator[](item);
    Arcane::RealArrayView d(ad_value.size(), ad_value.derivatives());
    Base::derivatives(item,0,d);*/
    return ad_value;
  }

  inline const RootContribution& operator[](const IntegerIndex& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == item.size()),("error, Geoxim::Item size not suitable"));
    Arcane::Integer index = item.index();
    RootContribution& ad_value = m_ad_value[index];
    /*ad_value = 0.;
    ad_value.value() = Base::operator[](item);
    const Arcane::Integer offset = m_unknowns_offsets[index];
    const Arcane::Integer size = m_unknowns_offsets[index+1] - offset;
    Arcane::RealArrayView d(size, ad_value.derivatives() + offset);// = ad_value.derivatives().subView(offset, size);
    Base::derivatives(item,index,d);*/
    return ad_value;
  }

};
#else
class VariableArray
    : public Law::VariableT< Law::ArrayRefT<Law::ScalarRealProperty> >
    , private BaseVariable
{
public:

  typedef Law::VariableT< Law::ArrayRefT<Law::ScalarRealProperty> > Base;

  VariableArray() {}

  VariableArray(const VariableArray& rhs)
  : Base(rhs)
  , BaseVariable(rhs) {}

  VariableArray(const Law::BuildInfo<Law::ScalarRealProperty>& bi)
  : Base(bi.property(), bi.offsets(), bi.variableMng().arrays())
  , BaseVariable(bi.offsets()) {}

  virtual ~VariableArray() {}

public:

  inline const Contribution& operator[](Arcane::Integer item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == 1),("error, using Arcane::Item instead of Geoxim::Item"));
    Contribution& ad_value = m_ad_value[0];
    ad_value = 0.;
    ad_value.value() = Base::operator[](item);
    Arcane::RealArrayView d(ad_value.size(), ad_value.derivatives());
    Base::derivatives(item,0,d);
    return ad_value;
  }

  inline const Contribution& operator[](const IntegerIndex& item) const
  {
    ARCANE_ASSERT((m_ad_value.size() == item.size()),("error, Geoxim::Item size not suitable"));
    Arcane::Integer index = item.index();
    Contribution& ad_value = m_ad_value[index];
    ad_value = 0.;
    ad_value.value() = Base::operator[](item);
    const Arcane::Integer offset = m_unknowns_offsets[index];
    const Arcane::Integer size = m_unknowns_offsets[index+1] - offset;
    Arcane::RealArrayView d(size, ad_value.derivatives() + offset);// = ad_value.derivatives().subView(offset, size);
    Base::derivatives(item,index,d);
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

#endif /* LAW_CONTRIBUTION_VARIABLEARRAY_H */
