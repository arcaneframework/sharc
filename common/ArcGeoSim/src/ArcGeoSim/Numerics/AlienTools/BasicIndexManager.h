// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_NUMERICS_UTILS_BASICINDEXMANAGER_H
#define ARCGEOSIM_ARCGEOSIM_NUMERICS_UTILS_BASICINDEXMANAGER_H


#include "ArcGeoSim/Numerics/AlienTools/IIndexManager.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*! \todo Il est possible d'optimiser les acc�s vectorielles en raprochant les
 *  structures internes des interfaces critiques
 *  (en particulier getIndex vectoriel)
 */
class BasicIndexManager :
    public IIndexManager
{
public:
  class ItemAbstractFamily : public IAbstractFamily {
  public:
    ItemAbstractFamily(const Arcane::IItemFamily * family);
    virtual ~ItemAbstractFamily() { }

  public:
    IIndexManager::IAbstractFamily * clone() const;

  public:
    Int32 maxLocalId() const;
    void uniqueIdToLocalId(Arcane::Int32ArrayView localIds, Arcane::Int64ConstArrayView uniqueIds) const;
    Item item(Int32 localId) const;

    SafeConstArrayView<Integer> owners(Arcane::Int32ConstArrayView localIds) const;
    SafeConstArrayView<Int64> uids(Arcane::Int32ConstArrayView localIds) const;
    SafeConstArrayView<Int32> allLocalIds() const;

    const Arcane::IItemFamily * family() const { return m_family; }

  private:
    const Arcane::IItemFamily * m_family;
    const Arcane::ItemInternalArrayView m_item_internals;
  };

  class MyEntryEnumeratorImpl;
  class MyEntryImpl;

  public:
  //! Constructeur de la classe
  BasicIndexManager(Arcane::IParallelMng * parallelMng);

  //! Destructeur de la classe
  virtual ~BasicIndexManager();

  //! Initialisation
  void init();

  //! Indique si la phase de pr�paration est achev�e
  bool isPrepared() const { return m_state == Prepared; }

  //! D�finit le gestionnaire de trace
  void setTraceMng(Arcane::ITraceMng * traceMng);

  //! Pr�paration : fixe l'indexation (fin des d�finitions)
  void prepare();

  //! Statistiques d'indexation
  /*! Uniquement valide apr�s \a prepare */
  void stats(Integer & globalSize,
      Integer & minLocalIndex,
      Integer & localSize) const;

  //! Retourne la taille globale
  /*! Uniquement valide apr�s \a prepare */
  Integer globalSize() const;

  //! Retourne l'indice minimum local
  /*! Uniquement valide apr�s \a prepare */
  Integer minLocalIndex() const;

  //! Retourne l'indice minimum local
  /*! Uniquement valide apr�s \a prepare */
  Integer localSize() const;

  //! Construction d'un enumerateur sur les \a Entry
  EntryEnumerator enumerateEntry() const;

  //! Construit une nouvelle entr�e scalaire sur des items du maillage
  ScalarIndexSet buildScalarIndexSet(const String name, Arcane::IItemFamily * item_family);
  void defineIndex(ScalarIndexSet& set, const Arcane::ItemGroup & itemGroup);

  ScalarIndexSet buildScalarIndexSet(const String name, const Arcane::ItemGroup & itemGroup);

  //! Construit une nouvelle entr�e scalaire sur un ensemble d'entit�s abstraites
  ScalarIndexSet buildScalarIndexSet(const String name, const Arcane::IntegerConstArrayView localIds, const IAbstractFamily & family);

  //! Construit une nouvelle entr�e scalaire sur l'ensemble des entit�s d'une familles abstraite
  ScalarIndexSet buildScalarIndexSet(const String name, const IAbstractFamily & family);

  //! Construit une nouvelle entr�e vectorielle sur des items du maillage
  /*! L'impl�mentation actuelle consid�re le multi-scalaire comme du vectoriel */
  VectorIndexSet buildVectorIndexSet(const String name, const Arcane::ItemGroup & itemGroup, const Integer n);

  //! Construit une nouvelle entr�e vectoriellesur un ensemble d'entit�s abstraites
  /*! L'impl�mentation actuelle consid�re le multi-scalaire comme du vectoriel */
  VectorIndexSet buildVectorIndexSet(const String name, const Arcane::IntegerConstArrayView localIds, const IAbstractFamily & family, const Integer n);

  //! Construit une nouvelle entr�e scalaire sur l'ensemble des entit�s d'une familles abstraite
  /*! L'impl�mentation actuelle consid�re le multi-scalaire comme du vectoriel */
  VectorIndexSet buildVectorIndexSet(const String name, const IAbstractFamily & family, const Integer n);

  //! Demande de d�-indexation d'une partie d'une entr�e
  /*! Utilisable uniquement avant prepare */
  void removeIndex(const ScalarIndexSet & entry, const Arcane::ItemGroup & itemGroup);

  Integer getIndex(const Entry & entry, const Arcane::Item & item) const ;

  //! Consultation vectorielle d'indexation d'une entr�e (apr�s prepare)
  void getIndex(const ScalarIndexSet & entry, const Arcane::ItemVectorView & items, ArrayView<Integer> indexes) const;

  //! Fournit une table de translation index� par les items
  Arcane::IntegerSharedArray getIndexes(const ScalarIndexSet & entry) const;

  //! Fournit une table de translation vectorielle index� par les items puis par les entr�es
  Arcane::SharedArray2<Integer> getIndexes(const VectorIndexSet & entries) const;

  //! Donne le gestionnaire parall�le ayant servi � l'indexation
  Arcane::IParallelMng * parallelMng() const { return m_parallel_mng; }

  //! define null index : default = -1, if true null_index = max_index+1
  void setMaxNullIndexOpt(bool flag) {
    m_max_null_index_opt = flag ;
  }

  Integer nullIndex() const {
    ARCANE_ASSERT((m_state==Prepared),("nullIndex is valid only in Prepared state")) ;
    if(m_max_null_index_opt)
      return m_global_entry_offset+m_local_entry_count ;
    else
      return -1 ;
  }


  public:
  void keepAlive(const IAbstractFamily * family);

  private:
  Arcane::IParallelMng * m_parallel_mng;
  Integer m_local_owner; //!< Identifiant du 'propri�taire' courant

  enum State { Undef, Initialized, Prepared } m_state;

  Arcane::ITraceMng * m_trace;

  struct InternalEntryIndex {
    InternalEntryIndex(MyEntryImpl * e, Integer lid, Integer kind, Int64 uid, Integer index, Integer creation_index, Integer owner) 
    : m_entry(e)
    , m_uid(uid)
    , m_localid(lid)
    , m_kind(kind)
    , m_index(index)
    // , m_creation_index(creation_index)
    , m_owner(owner)
    { }
    MyEntryImpl * m_entry;
    Int64 m_uid;
    Integer m_localid, m_kind, m_index;
    // Integer m_creation_index;
    Integer m_owner;
    bool operator==(const InternalEntryIndex & m) const { return m.m_entry == m_entry && m.m_localid == m_localid; }
  };

  typedef std::vector<InternalEntryIndex> EntryIndexMap;

  Integer m_local_entry_count;
  Integer m_global_entry_count;
  Integer m_global_entry_offset;
  Integer m_local_removed_entry_count ;
  Integer m_global_removed_entry_count ;

  bool    m_max_null_index_opt ;


  //! Table des Entry connues localement
  typedef std::map<String,MyEntryImpl*> EntrySet;
  EntrySet m_entry_set;

  //! Index de creation des entr�es
  Integer m_creation_index;

  //! Famille des familles abstraites associ�es aux familles du maillage
  static const Integer m_mesh_kind_shift;
  static const Integer m_family_kind_shift;
  Integer m_abstract_family_base_kind;
  std::map<Arcane::IMesh *, Integer> m_item_family_meshes; //!< Table des maillages connues (pour grouper en maillage)
  std::map<Integer, boost::shared_ptr<IAbstractFamily> > m_abstract_families; //!< Table des IAbstractFamily ici g�r�es
  std::map<const IAbstractFamily *, Integer> m_abstract_family_to_kind_map; //!< Permet la gestion de la survie des IAbstractFamily ext�rieures

  struct EntryIndexComparator
  {
    inline bool operator()(const BasicIndexManager::InternalEntryIndex & a,
        const BasicIndexManager::InternalEntryIndex & b) const;
  };

  //! Retourne l'entr�e associ�e � un nom
  Entry getEntry(const String name) const;

  protected:
  //! \internal Structure interne de communication dans prepare()
  struct EntrySendRequest;
  struct EntryRecvRequest;

  protected: // M�thodes prot�g�s en attendant une explicitation du besoin


  private:
  Entry buildEntry(const String name, const IAbstractFamily * itemFamily, const Integer kind);
  void defineIndex(const Entry & entry, const Arcane::IntegerConstArrayView localIds);
  void parallel_prepare(EntryIndexMap & entry_index);
  void sequential_prepare(EntryIndexMap & entry_index);
  inline bool isOwn(const IAbstractFamily::Item & item) const { return item.owner() == m_local_owner; }
  inline bool isOwn(const InternalEntryIndex & i) const { return i.m_owner == m_local_owner; }
  void reserveEntries(const EntryIndexMap & entry_index);
  Integer addNewAbstractFamily(const IAbstractFamily * family);
  Integer kindFromItemFamily(const Arcane::IItemFamily * family);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_INDEXMANAGER_BASICINDEXMANAGER_H */
