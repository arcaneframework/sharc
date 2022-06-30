// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef VARIABLEBUILDINFO_H
#define VARIABLEBUILDINFO_H
/* Author : desrozis at Mon May  2 17:39:07 2011
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Law {

template<typename SupportT, typename ValueT, ArcGeoSim::var::eDim dim>
class VariableBuildInfo
{
private:
  typedef typename ArcGeoSim::Var<SupportT,ValueT,dim>::var_type  T ;
  typedef typename ArcGeoSim::Var<SupportT,ValueT,dim>::dvar_type DT;
  typedef typename ArcGeoSim::Var<SupportT,ValueT,dim>::key_type  key_type ;
public:

  VariableBuildInfo(Arcane::Integer property_id,
                       T& property)
    : m_property_id(property_id)
    , m_property(&property)
    , m_derivative_property(NULL)
    , m_with_derivated(false){}

  VariableBuildInfo(Arcane::Integer property_id,
                       T& property,
                       DT& derivative_property)
    : m_property_id(property_id)
    , m_property(&property)
    , m_derivative_property(&derivative_property)
    , m_with_derivated(true){}

  VariableBuildInfo(Arcane::Integer property_id,
                    key_type        property_key,
                    bool with_derivated = false )
    : m_property_id(property_id)
    , m_property_key(property_key)
    , m_property(NULL)
    , m_derivative_property(NULL)
    , m_with_derivated(with_derivated){}

public:
  const Arcane::Integer& getPropertyId() const {
    return m_property_id;
  }

  const key_type& getPropertyKey() const {
    return m_property_key;
  }

  T* getProperty() {
    return m_property;
  }

  DT* getDerivativeProperty() {
    return m_derivative_property;
  }

  bool withDerivated() const {
    return m_with_derivated ;
  }

  void setProperty(T* var) {
    m_property = var ;
  }

  void setDerivativeProperty(DT* dvar) {
    m_derivative_property = dvar ;
  }

private:
  const Arcane::Integer m_property_id;
  key_type              m_property_key;
  T* m_property;
  DT* m_derivative_property;
  bool m_with_derivated ;
};
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* LAW_LAWVARIABLE_H */
