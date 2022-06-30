// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_PHYSICS_GUMP_PRIVATE_H
#define ARCGEOSIM_PHYSICS_GUMP_PRIVATE_H
/* Author : desrozis at Mon Jun 29 16:05:37 2015
 * Generated by createNew
 */

#include "ArcGeoSim/Physics/Gump/Dimension.h"

#include <arcane/utils/Array.h>
#include <arcane/utils/Array2.h>
#include <arcane/utils/String.h>
#include <arcane/utils/FatalErrorException.h>
#include <arcane/datatype/DataTypes.h>

#include <map>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Gump {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Builder;

namespace Private {

  class Entity;
  class Property;

}

typedef Arcane::UniqueArray<Private::Entity*> EntityVector;
typedef Arcane::UniqueArray<Private::Property*> PropertyVector;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Private {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Entity
{
public:

  typedef std::map<Arcane::String, Entity*>   EntityMap;
  typedef std::map<Arcane::String, Property*> PropertyMap;
  typedef std::map<Entity*,Arcane::Real> Values;

private:

  Entity(const Arcane::String& name,
         const Arcane::Integer kind,
         const Arcane::Integer tag,
         const Arcane::Integer number_of_entities,
         const Arcane::Integer number_of_entity_tags,
         const Arcane::Integer number_of_properties,
         const Arcane::Integer entity_unique_id);

public:

  ~Entity() {}

  friend class Gump::Builder;

private:

  Entity(const Entity&);
  Entity& operator=(const Entity&);

public:

  const Arcane::String& name() const
  {
    return m_name;
  }
  
  Arcane::Integer kind() const
  {
    return m_kind;
  }

  Arcane::Integer tag() const
  {
    return m_tag;
  }

  bool is(Arcane::Integer tag) const
  {
    return (m_tag & tag) == tag;
  }

  Arcane::Integer numberOf(Arcane::Integer K) const
  {
    return m_entities[K].size();
  }

  Entity* entity(Arcane::Integer i, Arcane::Integer K) const
  {
    return m_entities[K][i];
  }

  bool contains(Arcane::String name, Arcane::Integer K) const
  {
    return m_sorted_entities[K].find(name) != m_sorted_entities[K].end();
  }

  Entity* entity(Arcane::String name, Arcane::Integer K) const
  {
    return m_sorted_entities[K].find(name)->second;
  }
  
  Arcane::Integer numberOf(Arcane::Integer K, Arcane::Integer T) const
  {
    return m_tagged_entities[K][T].size();
  }

  Entity* entity(Arcane::Integer i, Arcane::Integer K, Arcane::Integer T) const
  {
    return m_tagged_entities[K][T][i];
  }

  void push(Entity* entity);

  void push(Property* property);

  const EntityVector& entities() const
  {
    return m_pushed_entities;
  }

  const EntityVector& entities(Arcane::Integer K) const
  {
    return m_entities[K];
  }

  const EntityVector& entities(Arcane::Integer K, Arcane::Integer T) const
  {
    return m_tagged_entities[K][T];
  }

  Arcane::Integer numberOfProperties() const
  {
    return m_defined_properties.size();
  }

  Arcane::Integer propertyKind(Arcane::Integer i) const;

  Property* property(Arcane::Integer kind) const
  {
    ARCANE_ASSERT((m_properties[kind] != nullptr),("Property pointer null"));
    return m_properties[kind];
  }

  Arcane::Integer uniqueId() const
  {
    return m_unique_id;
  }

  inline Arcane::Integer familyId() const
  {
    return m_family_id;
  }

  Entity* parent() const
  {
    return m_parent;
  }

  Entity* root() const;

  Values& values() { return m_values; }

  void kill();

private:

  void _insert(Entity* entity);

  void _updateFamilyId(Entity* entity);

private:

  Arcane::String m_name;
  Arcane::Integer m_kind;
  Arcane::Integer m_tag;

  Entity* m_parent;

  EntityVector m_pushed_entities;
  Arcane::UniqueArray<EntityVector> m_entities;
  Arcane::UniqueArray<EntityMap> m_sorted_entities;
  Arcane::UniqueArray2<EntityVector> m_tagged_entities;

  PropertyVector m_properties;
  PropertyVector m_defined_properties;
  PropertyMap m_sorted_properties;

  Arcane::Integer m_unique_id;
  Arcane::Integer m_family_id;

  Values m_values;
};

class Property
{
public:

  Property(Arcane::String name,
           Arcane::Integer property_unique_id,
           Dimension dimension,
           Arcane::eDataType data_type,
           Arcane::Integer size);

private:

  Property();

  Property(Arcane::String name,
           Arcane::Integer kind,
           Arcane::Integer property_unique_id,
           Entity* owner,
           Dimension dimension,
           Arcane::eDataType data_type,
           Arcane::Integer size);

  friend class Gump::Builder;

public:

  ~Property() {}

public:

  const Arcane::String& name() const
  {
    return m_name;
  }

  const Arcane::String& fullName() const
  {
    return m_full_name;
  }

  Arcane::Integer kind() const
  {
    return m_kind;
  }

  Arcane::Integer uniqueId() const
  {
    return m_index;
  }

  Entity* owner() const
  {
    return m_owner;
  }

  Dimension dimension() const
  {
    return m_dimension;
  }

  Arcane::eDataType dataType() const
  {
    return m_data_type;
  }

  Arcane::Integer size() const
  {
    return m_size;
  }

private:

  Arcane::Integer m_kind;
  Arcane::Integer m_index;
  Entity* m_owner;
  Arcane::String m_name;
  Arcane::String m_full_name;
  Dimension m_dimension;
  Arcane::eDataType m_data_type;
  Arcane::Integer m_size;
};

inline Arcane::Integer Entity::propertyKind(Arcane::Integer i) const
{
  return m_defined_properties[i]->kind();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_PHYSICS_GUMP_PRIVATE_H */
