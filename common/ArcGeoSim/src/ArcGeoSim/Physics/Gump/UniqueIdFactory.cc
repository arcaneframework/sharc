// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "UniqueIdFactory.h"
#include "IO.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Gump {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

UniqueIdFactory* UniqueIdFactory::m_instance = nullptr;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
UniqueIdFactory::
initialize(const Gump::Entity& root, Arcane::Integer nb_properties)
{
  auto it = m_next_property_uid.find(root);

  if(it != m_next_property_uid.end()) {
    throw Arcane::FatalErrorException("Entity i already registered");
  }

  m_next_property_uid[root] = nb_properties;
}

/*---------------------------------------------------------------------------*/

Arcane::Integer
UniqueIdFactory::
retainPropertyUniqueId(const Gump::Entity& owner)
{
  auto root = owner.root();

  auto it = m_next_property_uid.find(root);

  if(it == m_next_property_uid.end()) {
    throw Arcane::FatalErrorException("Entity has no root registered");
  }
  it->second++;
  return it->second;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
