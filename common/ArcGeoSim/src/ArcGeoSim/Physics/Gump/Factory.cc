// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "Factory.h"
#include "UniqueIdFactory.h"
#include "IO.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Gump {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Factory::
~Factory() noexcept(false)
{
  if(m_entities.size() == 0) return;

  auto root = m_entities[0].root();

  for(auto e : m_entities) {
    if(e.root() != root) {
      throw Arcane::FatalErrorException("Entities don't have same root");
    }
  }

  UniqueIdFactory::instance().initialize(root, m_property_unique_id);
}

/*---------------------------------------------------------------------------*/

Builder
Factory::
create(Arcane::String name,
       Arcane::Integer kind,
       Arcane::Integer tag)
{
  auto builder = Builder(name, kind, tag,
      m_number_of_entities,
      m_number_of_entity_tags,
      m_number_of_properties,
      m_entity_unique_id++,
      m_property_unique_id);
  // On garde toutes les entites
  m_entities.add(builder.entity());
  return builder;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
