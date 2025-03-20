// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTGROUPIMPL_H
#define ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTGROUPIMPL_H

/*---------------------------------------------------------------------------*/

#include <arcane/utils/String.h>
#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/Contact.h"

// Pr�d�claration
class IContactFamily;

/*---------------------------------------------------------------------------*/

class ContactGroupImpl
{
public:
  //! Constructeur de la classe
  /*! Conserve la r�f�rence sur le tableau des lids */
  ContactGroupImpl(IContactFamily * family, 
                   Arcane::Int32SharedArray lids,
                   const String & name, 
                   const bool protect);

//   //! Constructeur de la classe
//   /*! Recopie le tableau des lids */
//   ContactGroupImpl(FamilyType * family, 
//                 const Int32ConstArrayView & lids, 
//                 const String & name,
//                 const bool protect)
//   : m_family(family)
//   , m_local_ids(lids)
//   , m_name(name)
//   , m_protect(protect)
//   {}
  
  /** Destructeur de la classe */
  virtual ~ContactGroupImpl();

private:
  ContactGroupImpl(const ContactGroupImpl &); // undefined

public:
  //! Enumerateur du groupe
  ContactEnumerator enumerator() const;

  //! D�finition du groupe
  void setItems(ConstArrayView<Int32> lids);
  
  //! Ajout d'items
  void addItems(ConstArrayView<Int32> lids);

  //! Taille du groupe
  Integer size() const { return m_local_ids.size(); }

  //! Teste si le groupe est vide
  bool empty() const { return m_local_ids.empty(); }
  
  //! Nom du groupe
  String name() const { return m_name; }

  //! Famille du groupe
  IContactFamily * family() const { return m_family; }

  //! LocalIds du groupe
  Int32ConstArrayView itemsLocalId() { return m_local_ids; }

protected:
  IContactFamily * m_family;
  Arcane::Int32SharedArray m_local_ids;
  String m_name;
  bool m_protect;
};

/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTGROUPIMPL_H */
