// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_PHYSICS_GUMP_BUILDER_H
#define ARCGEOSIM_PHYSICS_GUMP_BUILDER_H

#include "ArcGeoSim/Physics/Gump/Entity.h"

#include <arcane/utils/ValueConvert.h>
#include <arcane/VariableDataTypeTraits.h>

namespace Gump {
    
  class Builder
  {
  protected:
        
    Builder(Arcane::String name, Arcane::Integer kind, Arcane::Integer tag,
            Arcane::Integer number_of_entities,
            Arcane::Integer number_of_entity_tags,
            Arcane::Integer number_of_properties,
            Arcane::Integer entity_unique_id,
            Arcane::Integer& property_unique_id)
      : m_entity(new Private::Entity(name, kind, tag,
                                     number_of_entities,
                                     number_of_entity_tags,
                                     number_of_properties,
                                     entity_unique_id))
      , m_property_unique_id(&property_unique_id) {}
        
    friend class Factory;

  public:
        
    Builder()
      : m_entity(nullptr)
      , m_property_unique_id(nullptr) {}

    Builder& operator=(const Builder& builder)
    {
      m_entity = builder.m_entity;
      m_property_unique_id = builder.m_property_unique_id;
      return *this;
    }
        
    Builder(const Builder& builder)
      : m_entity(builder.m_entity)
      , m_property_unique_id(builder.m_property_unique_id) {}
        
    void push(const Builder& t)
    {
      m_entity->push(t.m_entity);
    }
    
    void push(const Builder& t, Arcane::Real value)
    {
      m_entity->push(t.m_entity);
      auto& values = m_entity->values();
      values[t.m_entity] = value;
    }

    template<Dimension D, typename T>
    void push(Arcane::String name, Arcane::Integer kind)
    {
      (*m_property_unique_id)++;
      auto dt = Arcane::VariableDataTypeTraitsT<T>::type();
      auto* ptr = new Private::Property(name, kind,
                                        *m_property_unique_id,
                                        m_entity, D, dt, 1);
      m_entity->push(ptr);
    }

    template<Dimension D, typename T>
    void push(Arcane::String name, Arcane::Integer kind, Arcane::Integer size)
    {
      (*m_property_unique_id)++;
      auto dt = Arcane::VariableDataTypeTraitsT<T>::type();
      auto* ptr = new Private::Property(name, kind,
                                        *m_property_unique_id,
                                        m_entity, D, dt, size);
      m_entity->push(ptr);
    }

    Entity entity() const
    {
      return m_entity;
    }
        
  protected:
        
    Private::Entity* m_entity;
        
    Arcane::Integer* m_property_unique_id;
  };
    
}

#endif /* ARCGEOSIM_PHYSICS_GUMP_BUILDER_H */
