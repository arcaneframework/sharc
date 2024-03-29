// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef NUMERICS_CONDITION_FOLDERENUMERATOR_H
#define NUMERICS_CONDITION_FOLDERENUMERATOR_H
/* Author : desrozis at Fri Apr 24 15:13:48 2015
 * Generated by createNew
 */

#include "ArcGeoSim/Physics/Law2/Contribution/VariableFolder.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<Law::ContainerKind::eType CK, Law::ItemKind::eType IK>
class FolderEnumerator
{
public:
  
  typedef Law::VariableFolder<CK,IK> Folder;
  
public:

  FolderEnumerator(const Arcane::Array<Folder*>& folders,
		  	  	       const Arcane::StringArray& names)
    : m_folders(folders)
	  , m_names(names)
    , m_index(0)
  {
    ARCANE_ASSERT((folders.size() == names.size()),("folder and names size different"));
  }

  FolderEnumerator(const FolderEnumerator& e)
    : m_folders(e.m_folders)
  	, m_names(e.m_names)
    , m_index(0) {}

  //! Vrai si la l'enumeration n'est pas terminee
  bool hasNext() const { return m_index != size(); }

  //! Incrementation
  void operator++() { m_index ++; }

  //! Acces aux entites
  const Folder& folder() const { return *(m_folders[m_index]); }

  Folder& folder() { return *(m_folders[m_index]); }

  Arcane::String name() const { return m_names[m_index]; }

  //! Nombre d'elements a enumerer
  Arcane::Integer size() const { return m_folders.size(); }

  //! Index courant
  Arcane::Integer index() const { return m_index; }

private:

  //! Liste des entites a enumerer
  const Arcane::Array<Folder*>& m_folders;

  //! Liste des noms
  const Arcane::StringArray& m_names;

  //! Index courant
  Arcane::Integer m_index;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* NUMERICS_CONDITION_FOLDERENUMERATOR_H */
