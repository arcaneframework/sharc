// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_PHYSICS_GUMP_USERPROPERTY_H
#define ARCGEOSIM_PHYSICS_GUMP_USERPROPERTY_H

#include "Entity.h"
#include "UniqueIdFactory.h"

#include "ArcGeoSim/Physics/Law2/Property.h"

#include <arcane/utils/ArcaneGlobal.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Gump {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename E>
class UserProperty
{
public:

  UserProperty()
    : m_id(-1) {}

  UserProperty(const UserProperty<E>& property)
    : m_id(property.m_id)
    , m_name(property.m_name)
    , m_full_name(property.m_full_name)
    , m_owner(property.m_owner) {}

  UserProperty(Arcane::Integer id, const Arcane::String name, const E& owner)
    : m_id(id)
    , m_name(name)
    , m_full_name(Arcane::String::format("{0}_{1}", name, owner.name()))
    , m_owner(owner) {}

public:

  const Arcane::String& name() const
  {
    return m_name;
  }

  const Arcane::String& fullName() const
  {
    return m_full_name;
  }

  const E& owner() const
  {
    return m_owner;
  }

  Arcane::Integer uniqueId() const
  {
    return m_id;
  }

  bool operator==(const UserProperty& p) const
  {
    return uniqueId() == p.uniqueId();
  }

  bool operator!=(const UserProperty& p) const
  {
    return not this->operator==(p);
  }

  bool operator<(const UserProperty& p) const
  {
    return uniqueId() < p.uniqueId();
  }

  UserProperty<E>& operator=(const UserProperty<E>& property)
  {
    m_id = property.m_id;
    m_name = property.m_name;
	m_full_name = property.m_full_name;
    m_owner = property.m_owner;
    return *this;
  }

  operator Law::ScalarRealProperty() const
  {
    return cast();
  }

  Law::ScalarRealProperty cast() const
  {
    return Law::ScalarRealProperty(uniqueId(),fullName());
  }

private:

  Arcane::Integer m_id;
  Arcane::String m_name;
  Arcane::String m_full_name;
  E m_owner;
};

/*---------------------------------------------------------------------------*/

template<typename E>
inline UserProperty<E> makeUserProperty(Arcane::String name, const E& owner)
{
  auto id = UniqueIdFactory::instance().retainPropertyUniqueId(owner);
 
  return UserProperty<E>(
      id,
      name,
      owner);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_PHYSICS_GUMP_USERPROPERTY_H */
