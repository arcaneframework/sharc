// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACT_H
#define ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACT_H

#include <arcane/ArcaneTypes.h>
#include <arcane/Item.h>

#include "ArcGeoSim/Mesh/Utils/MeshTypes.h"

#define NULL_CONTACT_ID -1

class ContactSharedInfo;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ContactInternal 
{
public:
  //! Constructeur
  ContactInternal() 
    : m_local_id(NULL_CONTACT_ID)
    , m_lid1(Arcane::NULL_ITEM_LOCAL_ID)
    , m_lid2(Arcane::NULL_ITEM_LOCAL_ID)
    , m_shared_info(NULL) { }

  //! Destructeur
  ~ContactInternal() { }

  //! Test de contact vide
  inline bool null() { return m_local_id == NULL_CONTACT_ID; }

  //! Identifiant local
  inline Int32 localId() const { return m_local_id; }

  //! Identifiant local du premier membre du contact
  inline Int32 item1LocalId() const { return m_lid1; }

  //! Identifiant local du premier membre du contact
  Arcane::ItemInternal * item1Internal() const;

  //! Identifiant local du second membre du contact
  inline Int32 item2LocalId() const { return m_lid2; }

  //! Identifiant local du premier membre du contact
  Arcane::ItemInternal * item2Internal() const;

  //! Propri�taire du contact
  Integer owner() const;

  //! Donn�es partag�es du contact
  inline ContactSharedInfo * sharedInfo() { return m_shared_info; }

public: 
  //!@{ Donn�es publiques � usage interne UNIQUEMENT 
  Int32 m_local_id;
  Int32 m_lid1; 
  Int32 m_lid2;
  ContactSharedInfo * m_shared_info;
  //!@}
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Contact
{
public:
  //! Constructeur
  Contact() : m_internal(NULL), m_local_id(NULL_CONTACT_ID) { }

  //! Constructeur
  Contact(ContactInternal * internal) : m_internal(internal), m_local_id(internal->localId()) { }

  //! Destructeur
  virtual ~Contact() {}

public:
  //! Test de contact vide
  bool null() { return m_local_id != NULL_CONTACT_ID; }

  //! Identifiant local
  inline Int32 localId() const { return m_local_id; }

  //! Premier item du contact
  Item item1() const { return Item(m_internal->item1Internal()); }

  //! Deuxi�me item du contact
  Item item2() const { return Item(m_internal->item2Internal()); }

  //! Acc�s � l'internal du contact
  ContactInternal * internal() const { return m_internal; }

  //! Teste le type FaceFace du contact
  bool isFaceFace() const;

  //! Teste le type NodeFace du contact
  bool isNodeFace() const;

  //! Teste la localit� du contact
  bool isOwn() const;

  //! Propri�taire du contact
  Integer owner() const { return m_internal->owner(); }

protected:
  ContactInternal * m_internal;
  Int32 m_local_id;  
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class FaceFaceContact
  : public Contact 
{ 
public:
  //! Constructeur
  FaceFaceContact() : Contact() { }

  //! Constructeur
  FaceFaceContact(ContactInternal * internal) : Contact(internal) 
  { 
#ifdef ARCANE_CHECK
    ARCANE_ASSERT((isFaceFace()),("Invalid ContactInternal"));
#endif /* ARCANE_CHECK */
  }

  //! Constructeur
  FaceFaceContact(const Contact & internal) : Contact(internal) 
  { 
#ifdef ARCANE_CHECK
    ARCANE_ASSERT((isFaceFace()),("Invalid ContactInternal"));
#endif /* ARCANE_CHECK */
  }

public:
  //! Premi�re face du contact
  Face face1() const { return Face(m_internal->item1Internal()); }

  //! Deuxi�me face du contact
  Face face2() const { return Face(m_internal->item2Internal()); }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class NodeFaceContact
  : public Contact 
{ 
public:
  //! Constructeur
  NodeFaceContact() : Contact() { }

  //! Constructeur
  NodeFaceContact(ContactInternal * internal) : Contact(internal) 
  { 
#ifdef ARCANE_CHECK
    ARCANE_ASSERT((isNodeFace()),("Invalid ContactInternal"));
#endif /* ARCANE_CHECK */
  }

  //! Constructeur
  NodeFaceContact(const Contact & internal) : Contact(internal) 
  { 
#ifdef ARCANE_CHECK
    ARCANE_ASSERT((isNodeFace()),("Invalid ContactInternal"));
#endif /* ARCANE_CHECK */
  }

public:
  //! Premi�re face du contact
  Node node() const { return Node(m_internal->item1Internal()); }

  //! Deuxi�me face du contact
  Face face() const { return Face(m_internal->item2Internal()); }
};

bool operator==(const Contact & a_c1, const Contact & a_c2);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ContactEnumerator
{
public:
  //! Constructeur
  ContactEnumerator() : m_internals(0), m_local_ids(), m_index(0) { }
  //! Constructeur
  ContactEnumerator(ContactInternal * internals, const Int32ConstArrayView & lids) : m_internals(internals), m_local_ids(lids), m_index(0) { }
  //! Constructeur
  ContactEnumerator(const ContactEnumerator & rhs) : m_internals(rhs.m_internals), m_local_ids(rhs.m_local_ids), m_index(rhs.m_index) { }
  //! Destructeur
  ~ContactEnumerator() { }
  //! Test d'un successeur
  inline bool hasNext() const { return m_index<m_local_ids.size(); }
  //! Operateur d'avancement
  inline void operator++() { ++m_index; }
  //! D�r�f�rencement
  inline Contact operator*() const { return &m_internals[m_local_ids[m_index]]; }
  //! D�r�f�rencement
  inline ContactInternal * operator->() const { return &m_internals[m_local_ids[m_index]]; }
  //! Identifiant du contact 
  inline Int32 localId() const { return m_local_ids[m_index]; }
  //! Position dans l'�num�rateur
  inline Integer index() const { return m_index; }
  //! Taille de l'�num�rateur
  inline Integer count() const { return m_local_ids.size(); }

protected:
  ContactInternal * m_internals;
  Int32ConstArrayView m_local_ids;
  Integer m_index;
};

/*---------------------------------------------------------------------------*/

template<typename ContactType>
class ContactEnumeratorT 
  : public ContactEnumerator
{
public:
  ContactEnumeratorT() { }
  ContactEnumeratorT(ContactInternal * internals, const Int32ConstArrayView & lids) : ContactEnumerator(internals,lids) { }
  ContactEnumeratorT(const ContactEnumerator & rhs) : ContactEnumerator(rhs) { }
  ~ContactEnumeratorT() { }
  ContactType operator*() const { return ContactType(&this->m_internals[m_local_ids[m_index]]); }
};

/*---------------------------------------------------------------------------*/

typedef ContactEnumeratorT<FaceFaceContact> FaceFaceContactEnumerator;
typedef ContactEnumeratorT<NodeFaceContact> NodeFaceContactEnumerator;

/*---------------------------------------------------------------------------*/

#define ENUMERATE_CONTACT(i,contacts) for(ContactEnumerator i((contacts).enumerator()); i.hasNext(); ++i)
#define ENUMERATE_FACEFACECONTACT(i,contacts) for(FaceFaceContactEnumerator i((contacts).enumerator()); i.hasNext(); ++i)
#define ENUMERATE_NODEFACECONTACT(i,contacts) for(NodeFaceContactEnumerator i((contacts).enumerator()); i.hasNext(); ++i)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACT_H */
