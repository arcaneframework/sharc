// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ITEMGROUPBUILDER_H
#define ITEMGROUPBUILDER_H


#include <set>
#include <cstring>
#include <cctype>

#include <arcane/ArcaneVersion.h>
#include <arcane/ItemGroup.h>
#include <arcane/IMesh.h>
#include <arcane/utils/String.h>
#include <arcane/utils/StringBuilder.h>
#include <arcane/IItemFamily.h>
#include <arcane/ItemGroupRangeIterator.h>
#include <arcane/utils/FatalErrorException.h>

using namespace Arcane;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

//! Macro de construction d'un nom d'objet
/*! Sert generalement a nommer des groupes pour ItemGroupBuilder */
#define IMPLICIT_NAME ItemGroupBuilder_cleanString(__FILE__ "__" TOSTRING(__LINE__),false)
#define IMPLICIT_UNIQ_NAME ItemGroupBuilder_cleanString(__FILE__ "__" TOSTRING(__LINE__),true)


inline String ItemGroupBuilder_cleanString(const char * origStr, const bool isUniq) {
  static int cmpt = 0;
  const int len = std::strlen(origStr);
  char * str = new char[len+1];
  for(int i=0;i<len;++i)
    {
      if (std::isalnum(origStr[i]))
        str[i] = origStr[i];
      else
        str[i] = '_';
    }
  StringBuilder newString(str,len) ;
  newString += "_";
  if (isUniq) newString += cmpt++;
  delete[] str;
  return newString.toString() ;
}


/* \brief Outil de construction assistee de groupe
 *
 * L'unicite des elements du groupe est garantie par construction. Il
 * est possible d'utiliser la macro IMPLICIT_NAME pour nommer nom de
 * groupe.
 */
template<typename T>
class ItemGroupBuilder {
 private:
  IMesh * m_mesh;
  std::set<Integer> m_ids;
  String m_group_name;
  IItemFamily* m_item_family;
  
 public:
  //! Constructeur pour les familles pr�sentes par d�faut dans le maillage (Cell, Face, Edge, Node)
  ItemGroupBuilder(IMesh * mesh, const String& groupName)
    : m_mesh(mesh)
    , m_group_name(groupName)
    {
      m_item_family = _getFamily(ItemTraitsT<T>::defaultFamilyName());
    }

  //! Constructeur pour les familles non pr�sentes par d�faut, le nom de famille doit �tre pass�
  ItemGroupBuilder(IMesh* mesh, const String& groupName, const String& familyName)
    : m_mesh(mesh)
    , m_group_name(groupName)
  {
    m_item_family = _getFamily(familyName);
  }

 private:
  IItemFamily* _getFamily(const String& family_name){
    IItemFamily* item_family = m_mesh->findItemFamily(ItemTraitsT<T>::kind(),family_name,false);
    // Check use for non default family
    if (! item_family) throw Arcane::FatalErrorException(Arcane::String::format("Cannot create an ItemGroup on the unexisting ItemFamily {0} (kind {1}). To use ItemGroupBuilder for DoF group, give the family name in the constructor."
                                                          , family_name
                                                          , itemKindName(ItemTraitsT<T>::kind())));
    return item_family;
  }

 public:

  //! Destructeur
  virtual ~ItemGroupBuilder()
    {
      ;
    }

 public:
  //! Ajout d'un ensemble d'item fourni par un enumerateur
  void add(ItemEnumeratorT<T> enumerator) 
    { 
      while(enumerator.hasNext()) 
        {
          m_ids.insert(enumerator.localId());
          ++enumerator;
        }
    }

  //! Ajout d'un ensemble d'item fourni par un enumerateur
  void add(ItemGroupRangeIteratorT<T> enumerator) 
    { 
      while(enumerator.hasNext())
        {
          m_ids.insert(enumerator.itemLocalId());
          ++enumerator;
        }
    }

  //! Ajout d'un item unique
  void add(const T & item) 
    { 
      m_ids.insert(item.localId());
    }

  //! Constructeur du nouveau group
  ItemGroupT<T> buildGroup() 
    {
      Int32SharedArray localIds(m_ids.size());

      std::set<Integer>::const_iterator is = m_ids.begin();
      Integer i = 0;

      while(is != m_ids.end())
        {
          localIds[i] = *is;
          ++is; ++i;
        }
    
      ItemGroup newGroup = m_item_family->findGroup(m_group_name,true);

      newGroup.setItems(localIds);
      // newGroup.setLocalToSubDomain(true); // Force le nouveau a etre local : non transfere en cas de reequilibrage

      return newGroup;
    }

  //! Nom du groupe
  String getName() const 
    { 
      return m_group_name; 
    }
};

#endif /* ITEMGROUPBUILDER_H */
