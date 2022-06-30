// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_PHYSICS_GUMP_UNIQUEIDFACTORY_H
#define ARCGEOSIM_PHYSICS_GUMP_UNIQUEIDFACTORY_H

#include "ArcGeoSim/Physics/Gump/Entity.h"

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Physics/ArcGeoSim_physicsExport.h"
#else
#define ARCGEOSIM_PHYSICS_EXPORT
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Gump {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class UniqueIdFactory
{
public:

  static UniqueIdFactory& instance()
  {
    if (m_instance == nullptr)
      m_instance = new UniqueIdFactory;
    return *m_instance;
  }

  static void release()
  {
    delete m_instance;
    m_instance = nullptr;
  }

private:

  static ARCGEOSIM_PHYSICS_EXPORT UniqueIdFactory* m_instance;

private:

  UniqueIdFactory() {}

public:

  Arcane::Integer retainPropertyUniqueId(const Gump::Entity& owner);

private:

  void initialize(const Gump::Entity& root, Arcane::Integer nb_properties);

private:

  std::map<Gump::Entity, Arcane::Integer> m_next_property_uid;

  friend class Factory;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_PHYSICS_GUMP_UNIQUEIDFACTORY_H */
