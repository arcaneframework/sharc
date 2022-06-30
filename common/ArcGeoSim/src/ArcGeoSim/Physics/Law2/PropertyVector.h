// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_PROPERTYVECTOR_H
#define ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_PROPERTYVECTOR_H
/* Author : desrozis at Tue Nov  6 15:11:02 2012
 * Generated by createNew
 */

#ifdef WIN32
#include <ciso646>
#endif

#include "ArcGeoSim/Physics/Law2/Property.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
 * \ingroup Law
 * \brief Vecteur de proprietes
 *
 * Decrit un vecteur de proprietes :
 *   - tableau des proprietes
 *   - mapping inverse propriete => index dans la liste
 *   - somme des tailles des proprietes
 *   
 * \code
 * ScalarRealProperty pressure(0,"Pressure");
 * VectorialRealProperty saturation(1,"Saturation",2); // taille 2
 * VectorialRealProperty composition(2,"Composition",4);
 * 
 * PropertyVector v;
 * v << pressure << saturation << composition;
 * 
 * \endcode
 *
 * ATTENTION : les vecteurs ne sont plus valides si la taille d'une propriete vectorielle change
 *
 */
class PropertyVector
{
public:

  PropertyVector()  
    : m_offset(1) 
    , m_data_size(0) 
  {
    m_offset[0] = 0;
  }
  
  PropertyVector(const PropertyVector& p)
    : m_properties(p.m_properties) 
    , m_index(p.m_index)
    , m_offset(p.m_offset)
    , m_size(p.m_size)
    , m_data_size(p.m_data_size)
  {}

  PropertyVector& operator=(const PropertyVector& p)
  {
    m_properties = p.m_properties ;
    m_index = p.m_index ;
    m_offset = p.m_offset ;
    m_size = p.m_size ;
    m_data_size = p.m_data_size ;
    return *this ;
  }

  PropertyVector(Arcane::ConstArrayView<Property> properties)
    : m_properties(properties)
    , m_offset(m_properties.size()+1)
    , m_size(m_properties.size())
    , m_data_size(0)
  {
    if(m_properties.size() == 0) return;

    auto max_id = std::max_element(m_properties.begin(), m_properties.end());

    m_index.resize(max_id->id()+1);
    m_size.resize(max_id->id()+1);
    m_index.fill(-1);
    m_size.fill(-1);

    m_offset[0] = 0;
    for(Arcane::Integer i = 0; i < m_properties.size(); ++i) {
      const Property& p = m_properties[i];
      ARCANE_ASSERT((p.isInitialized()),(Arcane::String::format("Property '{0}' is not initialized",p.name()).localstr()));
      ARCANE_ASSERT((m_index[p.id()] == -1),("Multiple property in array"));
      m_index[p.id()] = i;
      m_data_size += p.size();
      m_size[p.id()] = p.size();
      m_offset[i+1] = m_data_size;
    }
  }

  ~PropertyVector() {}

  //! Comparaison
  bool operator==(const PropertyVector& v) const
  {
    if(m_index.size() != v.m_index.size()) {
      return false;
    }
    for(auto i=0; i<m_index.size(); ++i) {
      if(m_index[i] != v.m_index[i])
        return false;
    }
    return true;
  }

  bool operator!=(const PropertyVector& v) const
  {
    return not operator==(v);
  }

  //! Nombre de proprietes du vecteur
  inline Arcane::Integer size() const { return m_properties.size(); }

  //! Reservation de memoire
  inline void reserve(Arcane::Integer size) { m_properties.reserve(size); }

  //! Vide le vecteur
  inline void reset() { 
    m_properties.resize(0);
    m_index.resize(0);
    m_offset.resize(1);
    m_offset[0] = 0;
    m_size.resize(0);
    m_data_size = 0;
  }

  //! Acces a la ieme propriete
  inline const Property& operator[](Integer i) const 
  {
    return m_properties[i];
  }

  //! Acces a la ieme propriete
  inline Property& get(Arcane::Integer i)
  {
	return m_properties[i];
  }

  //! Position de la propriete
  inline Arcane::Integer operator[](const Property& p) const 
  {
    ARCANE_ASSERT((p.isInitialized()),(Arcane::String::format("Property '{0}' is not initialized",p.name()).localstr()));
    ARCANE_ASSERT((p.id() < m_index.size()),(Arcane::String::format("Property '{0}' is not in vector",p.name()).localstr()));
    ARCANE_ASSERT((m_index[p.id()] != -1),("Property is not in vector"));
    return m_index[p.id()];
  }
  
  //! Test si appartient au vecteur
  inline bool contains(const Property& p) const
  {
    ARCANE_ASSERT((p.isInitialized()),(Arcane::String::format("Property '{0}' is not initialized",p.name()).localstr()));
    return not( p.id() >= m_index.size() || m_index[p.id()] == -1 );
  }
  
  //! Ajout incremental d'une propriete
  PropertyVector& operator<<(const Property& p) 
  {
    ARCANE_ASSERT((p.isInitialized()),(Arcane::String::format("Property '{0}' is not initialized",p.name()).localstr()));
   
    if(contains(p)) return *this;
    
    m_properties.add(p);

    const Integer id = p.id();

    if(id >= m_index.size()) {
      const Integer old_size = m_index.size();
      m_index.resize(id+1);
      m_size.resize(id+1);
      for(Integer i = old_size; i < id+1; ++i) {
        m_index[i] = -1;
        m_size[i] = 0;
      }
    }
    
    ARCANE_ASSERT((m_index[id] == -1),(Arcane::String::format("Property '{0}' is not in vector",p.name()).localstr()));

    m_index[id] = m_properties.size() - 1;
    
    m_size[id] = p.size();

    m_data_size += p.size();

    m_offset.add(m_data_size);
    
    return *this;
  }
  
  //! Ajout incremental d'un tableau de proprietes
  PropertyVector& operator<<(Arcane::ConstArrayView<Property> properties)
  {
    for(Arcane::Integer i = 0; i < properties.size(); ++i)
      *this << properties[i];
    return *this;
  }
  
  //! Ajout incremental d'un tableau de proprietes
  template<Dimension D, typename T>
  PropertyVector& operator<<(const Arcane::Array< PropertyT<D,T> >& properties)
  {
    for(Arcane::Integer i = 0; i < properties.size(); ++i) {
      *this << properties[i];
    }
    return *this;
  }
  
  //! Test si le vecteur est vide
  inline bool empty() const { return m_properties.size() == 0; }

  //! Liste des proprietes
  inline Arcane::ConstArrayView<Property> properties() const { return m_properties; }

  //! Index des proprietes
  inline Arcane::IntegerConstArrayView index() const { return m_index; }

  //! Taille des proprietes
  inline Arcane::IntegerConstArrayView propertySize() const { return m_size; }
  
  //! Tableau des offset
  inline Arcane::IntegerConstArrayView offset() const { return m_offset; }
  
  //! Offset d'une propriete
  inline Arcane::Integer offset(const Property& p) const 
  {
    ARCANE_ASSERT((contains(p)),(Arcane::String::format("Property '{0}' is not in dependencies",p.name()).localstr()));
    return m_offset[m_index[p.id()]];
  }

  //! Taille d'une propriete
  inline Arcane::Integer size(const Property& p) const 
  {
    ARCANE_ASSERT((contains(p)),(Arcane::String::format("Property '{0}' is not in dependencies",p.name()).localstr()));
    return m_size[p.id()];
  }

  //! Retaille une propriete
  inline void resize(const Property& p, Arcane::Integer size) 
  {
    ARCANE_ASSERT((contains(p)),(Arcane::String::format("Property '{0}' is not in dependencies",p.name()).localstr()));
    ARCANE_ASSERT((size <= p.size()),(Arcane::String::format("New size of Property '{0}' should be lesser",p.name()).localstr()));
    
    if(size == m_size[p.id()]) return;
    
    const Arcane::Integer d = m_size[p.id()] - size;
    
    m_size[p.id()] = size;
    
    m_data_size -= d;
    
    // On doit decaler les offsets suivant
    for(Arcane::Integer i = m_index[p.id()] + 1; i < m_offset.size(); ++i)
      m_offset[i] -= d;
  }
  
  //! Taille des donnees
  inline Arcane::Integer dataSize() const { return m_data_size; }

  //! Recopie
  inline void copy(const PropertyVector& properties) 
  {
    m_properties.copy(properties.m_properties);
    m_index.copy(properties.m_index);
    m_offset.copy(properties.m_offset);
    m_size.copy(properties.m_size);
    m_data_size = properties.m_data_size;
  }
  
  operator Arcane::ConstArrayView<Property>() const { return m_properties; }

private:

  //! Liste des proprietes
  Arcane::SharedArray<Property> m_properties;

  //! Tableau des index
  Arcane::IntegerSharedArray m_index;

  //! Tableau des offsets
  Arcane::IntegerSharedArray m_offset;

  //! Tableau des tailles
  Arcane::IntegerSharedArray m_size;

  //! Somme des tailles des proprietes
  Arcane::Integer m_data_size;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline std::ostream& operator<<(std::ostream& nout, const PropertyVector& v) 
{
  for(Integer i = 0; i < v.size(); ++i) {
    nout << "Property{'" << v[i].name() << "'}[" << v[i].size() << "] ";
  }
  return nout;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_PROPERTYVECTOR_H */
