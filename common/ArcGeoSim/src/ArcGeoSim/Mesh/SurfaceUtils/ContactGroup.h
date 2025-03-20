// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTGROUP_H
#define ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTGROUP_H

/*---------------------------------------------------------------------------*/

#include <arcane/utils/Array.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/Item.h>

#include "ArcGeoSim/Mesh/SurfaceUtils/Contact.h"
#include "ArcGeoSim/Utils/ArcGeoSim.h"
class IContactFamily;
class ContactGroupImpl;

/*---------------------------------------------------------------------------*/

/*! Groupe d'Item g�n�rique mimant les fonctionnements d'Arcane
 *  L'organisation de cette classe suit celle d'arcane, la g�n�ricit� sur le type d'item en plus
 */
class ContactGroup
{
public:
  typedef ContactEnumerator Enumerator;

public:
  //! Constructeur de la classe
  ContactGroup();
  
  //! Constructeur par copie
  ContactGroup(const ContactGroup & rhs);

  //! Constructeur
  ContactGroup(ContactGroupImpl * impl);

  //! Destructeur de la classe
  virtual ~ContactGroup();

  //! Op�rateur de copie
  ContactGroup & operator=(const ContactGroup & rhs);

public:
  //! Enumerateur du groupe
  ContactEnumerator enumerator() const;

  //! D�finition du groupe
  void setItems(const Int32ConstArrayView & lids);

  //! Remplissage du groupe
  void addItems(const Int32ConstArrayView & lids);

  //! Taille du groupe
  Integer size() const;

  //! Teste si le groupe est vide
  bool empty() const;
  
  //! Teste si le groupe est null
  bool null() const;

  //! Nom du groupe
  String name() const;

  //! Famille du groupe
  IContactFamily * family() const;

  //! Impl�mentation interne
  ContactGroupImpl * impl() const;

protected:
  ContactGroupImpl * m_impl;
};

/*---------------------------------------------------------------------------*/

/*! Groupe d'Item g�n�rique mimant les fonctionnements d'Arcane
 *  L'organisation de cette classe suit celle d'arcane, la g�n�ricit� sur le type d'item en plus
 */
template<typename ContactType>
class ContactGroupT : public ContactGroup
{
public:
  typedef ContactEnumeratorT<ContactType> Enumerator;

public:
  //! Constructeur de la classe
  ContactGroupT() : ContactGroup() { }
  
  //! Constructeur de la classe
  ContactGroupT(const ContactGroup & group) : ContactGroup(group) { /* CHECK TYPE */ }

  //! Constructeur de la classe
  ContactGroupT(const ContactGroupT<ContactType> & group) : ContactGroup(group) { }
  
  //! Destructeur de la classe
  virtual ~ContactGroupT() {}
};

/*---------------------------------------------------------------------------*/

typedef ContactGroupT<FaceFaceContact> FaceFaceContactGroup;
typedef ContactGroupT<NodeFaceContact> NodeFaceContactGroup;

/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTGROUP_H */
