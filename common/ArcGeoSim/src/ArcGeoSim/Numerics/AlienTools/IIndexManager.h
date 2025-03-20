// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_NUMERICS_UTILS_IINDEX_MANAGER_H
#define ARCGEOSIM_ARCGEOSIM_NUMERICS_UTILS_IINDEX_MANAGER_H


#include <arcane/Item.h>
#include <arcane/utils/ObjectImpl.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/AutoRef.h>
#include <arcane/IItemFamily.h>
#include <arcane/utils/FatalErrorException.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/ArcaneVersion.h>
#include <arcane/ItemTypes.h>

#ifdef USE_ARCANE_V3
#include <alien/utils/SafeConstArrayView.h>
#else
#include <ALIEN/Utils/SafeConstArrayView.h>
#endif

#include <vector>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#ifndef BEGIN_ALIEN_NAMESPACE
#define BEGIN_ALIEN_NAMESPACE namespace Alien {
#define END_ALIEN_NAMESPACE }
#endif

BEGIN_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/**
 * Voir \ref secLinearSolver "Description des services de solveurs
 * lin�aires" pour plus de d�tails
 *
 * \todo Pour g�rer les inconnues vectoriel, mettre un argument additionnel qui
 * est le sous-indice (vectoriel) avec une valeur par d�faut de 0 (ou
 * bien un proto diff�rent)
 */
class IIndexManager {
public:
  //! Interface des familles abstraites pour l'indexation de items
  class IAbstractFamily {
  public:
    class Item {
    public:
      Item(Int64 uniqueId, Integer owner) : m_unique_id(uniqueId), m_owner(owner) { }
    public:
      Int64 uniqueId() const { return m_unique_id; }
      Integer owner() const { return m_owner; }
    private:
      Int64 m_unique_id;
      Integer m_owner;
    };

  public:
    virtual ~IAbstractFamily() { }

  public:
    //! Construit un clone de cet objet
    virtual IAbstractFamily * clone() const = 0;

  public:
    //! Identifiant maximal des localIds pour cette famille
    virtual Int32 maxLocalId() const = 0;
    //! Convertit des uniqueIds en localIds. Erreur fatale si un item n'est pas retrouv�
    virtual void uniqueIdToLocalId(Arcane::Int32ArrayView localIds, Arcane::Int64ConstArrayView uniqueIds) const = 0;
    //! Retourne un objet Item � partir de son localId
    virtual Item item(Int32 localId) const = 0;
    //! Retourne l'ensemble des owners (propri�taires) d'un ensemble d'item d�crits par leur localIds
    virtual SafeConstArrayView<Integer> owners(Arcane::Int32ConstArrayView localIds) const = 0;
    //! Retourne l'ensemble des uniqueIds d'un ensemble d'item d�crits par leur localIds
    virtual SafeConstArrayView<Int64> uids(Arcane::Int32ConstArrayView localIds) const = 0;
    //! Retourne l'ensemble des identifiants locaux de la famille
    virtual SafeConstArrayView<Int32> allLocalIds() const = 0;
  };

protected:
  //! Interface d'impl�mentation de \a Entry
  class EntryImpl
  {
  public:
    //! Destructeur
    virtual ~EntryImpl() { }
    //! Retourne la liste des Index de l'Entry
    virtual Arcane::ConstArrayView<Integer> getOwnIndexes() const = 0;
    //! Retourne la liste des Index de l'Entry (own + ghost)
    virtual Arcane::ConstArrayView<Integer> getAllIndexes() const = 0;
    //! Retourne la liste des Items de l'Entry
    virtual Arcane::ConstArrayView<Integer> getOwnLocalIds() const = 0;
    //! Retourne la liste des Items de l'Entry (own + ghost)
    virtual Arcane::ConstArrayView<Integer> getAllLocalIds() const = 0;
    //! Retourne le nom de l'entr�e
    virtual String getName() const = 0;
    //! Retourne le type de support de l'Entry
    virtual Integer getKind() const = 0;
    //! Retourne la famille abstraite de l'Entry
    virtual const IAbstractFamily & getFamily() const = 0;
    //! Ajout d'un tag
    virtual void addTag(const String &tagname, const String &tagvalue) = 0;
    //! Suppression d'un tag
    virtual void removeTag(const String &tagname) = 0;
    //! Test d'existance d'un tag
    virtual bool hasTag(const String &tagname) = 0;
    //! Lecture d'un tag
    virtual String tagValue(const String & tagname) = 0;
    //! R�f�rentiel du manager associ�
    virtual IIndexManager * manager() const = 0;
  };

public:
  //! Classe de repr�sentation des Entry
  /*! Cette classe est un proxy; sa copie est donc peu couteuse et son
   *  impl�mentation variable suivant le contexte
   */
  class Entry
  {
  protected:
    //! Impl�mentation de ce type d'entr�e
    EntryImpl * m_impl;
  public:
    //! Constructeur par d�faut
    Entry() : m_impl(NULL) { }

    //! Constructeur par copie
    Entry(const Entry & en) : m_impl(en.m_impl) { }

    //! Constructeur
    Entry(EntryImpl * impl) : m_impl(impl) { }

    //! Op�rateur de copie
    Entry & operator=(const Entry & en) {
      if (this != &en)
        m_impl = en.m_impl;
      return *this;
    }

    //! Acc�s interne � l'implementation
    EntryImpl * internal() const { return m_impl; }

    //! Indique si l'entr�e est d�finie
    bool null() const { return m_impl == NULL; }

    void nullify() {
      m_impl = NULL ;
    }

    //! Ensemble des indices 'own' g�r�s par cette entr�e
    ConstArrayView<Integer> getOwnIndexes() const { return m_impl->getOwnIndexes(); }

    //! Ensemble des indices 'own + ghost' g�r�s par cette entr�e (
    ConstArrayView<Integer> getAllIndexes() const { return m_impl->getAllIndexes(); }

    //! Ensemble des items 'own' g�r�s par cette entr�e
    ConstArrayView<Integer> getOwnLocalIds() const { return m_impl->getOwnLocalIds(); }

    //! Ensemble des items 'own + ghost' g�r�s par cette entr�e
    ConstArrayView<Integer> getAllLocalIds() const { return m_impl->getAllLocalIds(); }

    //! Nom de l'entr�e
    String getName() const { return m_impl->getName(); }

    //! Support de l'entr�e (en terme d'item)
    Integer getKind() const { return m_impl->getKind(); }

    //! Retourne la famille abstraite de l'Entry
    const IAbstractFamily & getFamily() const { return m_impl->getFamily(); }

    //@{ @name Gestion des tags
    //! Ajout d'un tag
    void addTag(const String &tagname, const String &tagvalue) { return m_impl->addTag(tagname,tagvalue); }

    //! Suppression d'un tag
    void removeTag(const String &tagname) { return m_impl->removeTag(tagname); }

    //! Test d'existance d'un tag
    bool hasTag(const String &tagname) { return m_impl->hasTag(tagname); }

    //! Acces en lecture � un tag
    String tagValue(const String & tagname) { return m_impl->tagValue(tagname); }
    //@}

    //! R�f�rentiel du manager associ�
    IIndexManager * manager() const { return m_impl->manager(); }
  };


  //! Interface d'implementation de \a EntryEnumerator
  class EntryEnumeratorImpl :
    public Arcane::ObjectImpl {
  public:
    virtual void moveNext() = 0;
    virtual bool hasNext() const = 0;
    virtual EntryImpl * get() const = 0;
  };


  //! Classe d'�num�ration des \a Entry connues
  /*! Classe de type proxy; la copie est peu couteuse et l'impl�mentation variable */
  class EntryEnumerator {
  protected:
    Arcane::AutoRefT<EntryEnumeratorImpl> m_impl;
  public:
    //! Constructeur par copie
    EntryEnumerator(const EntryEnumerator & e) : m_impl(e.m_impl) { }
    //! Constructeur par consultation de l'\a IndexManager
    EntryEnumerator(IIndexManager * manager) : m_impl(manager->enumerateEntry().m_impl) { }
    //! Constructeur par impl�mentation
    EntryEnumerator(EntryEnumeratorImpl * impl) : m_impl(impl) { }
    //! Avance l'�num�rateur
    void operator++() { m_impl->moveNext(); }
    //! Teste l'existence d'un �l�ment suivant
    bool hasNext() const { return m_impl->hasNext(); }
    //! D�r�f�rencement
    Entry operator*() const { return m_impl->get(); }
    //! D�r�f�rencement indirect
    EntryImpl * operator->() const { return m_impl->get(); }
    //! Nombre d'�l�ment dans l'�num�rateur
    Integer count() const 
    { 
      Integer my_size = 0;
      for(EntryEnumerator i = *this; i.hasNext(); ++i) ++my_size;
      return my_size;
    }
  };


 public:
  //! Constructeur par d�faut
  IIndexManager() { }

  //! Destructeur
  virtual ~IIndexManager() { }

  //! Indique si la phase de pr�paration est achev�e
  virtual bool isPrepared() const = 0 ;

  //! Initialisation les structures
  /*! Implicitement appel� par le constructeur */
  virtual void init() = 0;

  //! Pr�paration : fixe l'indexation (fin des d�finitions)
  virtual void prepare() = 0;

  //! D�finit le gestionnaire de trace
  virtual void setTraceMng(Arcane::ITraceMng * traceMng) = 0;

  //! Statistiques d'indexation
  /*! Uniquement valide apr�s \a prepare */
  virtual void stats(Integer & globalSize,
                     Integer & minLocalIndex,
                     Integer & localSize) const = 0;
  
  //! Retourne la taille globale
  /*! Uniquement valide apr�s \a prepare */
  virtual Integer globalSize() const = 0;
  
  //! Retourne l'indice minimum local
  /*! Uniquement valide apr�s \a prepare */
  virtual Integer minLocalIndex() const = 0;
  
  //! Retourne l'indice minimum local
  /*! Uniquement valide apr�s \a prepare */
  virtual Integer localSize() const = 0;
  
  //! Construction d'un enumerateur sur les \a Entry
  virtual EntryEnumerator enumerateEntry() const = 0;

  //! Retourne l'entr�e associ�e � un nom
  virtual Entry getEntry(const String name) const = 0;

  typedef Entry ScalarIndexSet;
  typedef SharedArray<ScalarIndexSet> VectorIndexSet;

  //! Construit une nouvelle entr�e scalaire sur des items du maillage
  virtual ScalarIndexSet buildScalarIndexSet(const String name,Arcane::IItemFamily * item_family) = 0;
  virtual void defineIndex(ScalarIndexSet& set , const Arcane::ItemGroup & itemGroup) = 0;
  virtual ScalarIndexSet buildScalarIndexSet(const String name, const Arcane::ItemGroup & itemGroup) = 0;

  //! Construit une nouvelle entr�e scalaire sur un ensemble d'entit�s abstraites
  virtual ScalarIndexSet buildScalarIndexSet(const String name, const Arcane::IntegerConstArrayView localIds, const IAbstractFamily & family) = 0;

  //! Construit une nouvelle entr�e scalaire sur l'ensemble des entit�s d'une familles abstraite
  virtual ScalarIndexSet buildScalarIndexSet(const String name, const IAbstractFamily & family) = 0;

  //! Construit une nouvelle entr�e vectorielle sur des items du maillage
  /*! L'impl�mentation actuelle consid�re le multi-scalaire comme du vectoriel */
  virtual VectorIndexSet buildVectorIndexSet(const String name, const Arcane::ItemGroup & itemGroup, const Integer n) = 0;

  //! Construit une nouvelle entr�e vectoriellesur un ensemble d'entit�s abstraites
  /*! L'impl�mentation actuelle consid�re le multi-scalaire comme du vectoriel */
  virtual VectorIndexSet buildVectorIndexSet(const String name, const Arcane::IntegerConstArrayView localIds, const IAbstractFamily & family, const Integer n) = 0;

  //! Construit une nouvelle entr�e scalaire sur l'ensemble des entit�s d'une familles abstraite
  /*! L'impl�mentation actuelle consid�re le multi-scalaire comme du vectoriel */
  virtual VectorIndexSet buildVectorIndexSet(const String name, const IAbstractFamily & family, const Integer n) = 0;

  //! Demande de d�-indexation d'une partie d'une entr�e
  /*! Utilisable uniquement avant prepare */
  virtual void removeIndex(const ScalarIndexSet & entry, const Arcane::ItemGroup & itemGroup) = 0;
  virtual void removeIndex(const VectorIndexSet & entry, Integer i, const Arcane::ItemGroup & itemGroup)
  {
    removeIndex(entry[i],itemGroup) ;
  }

  virtual Integer getIndex(const Entry & entry, const Arcane::Item & item) const = 0 ;

  //! Consultation vectorielle d'indexation d'une entr�e (apr�s prepare)
  virtual void getIndex(const ScalarIndexSet & entry, const Arcane::ItemVectorView & items, ArrayView<Integer> indexes) const = 0;

  //! Fournit une table de translation index� par les items
  virtual Arcane::IntegerSharedArray getIndexes(const ScalarIndexSet & entry) const = 0;

  //! Fournit une table de translation index� par les items
  virtual Arcane::IntegerSharedArray2 getIndexes(const VectorIndexSet & entry) const = 0;

  //! Donne le gestionnaire parall�le ayant servi � l'indexation
  virtual Arcane::IParallelMng * parallelMng() const = 0;

  //! define null index : default == -1, if true nullIndex() == max index of current indexation
  virtual void setMaxNullIndexOpt(bool flag) = 0 ;

  //! return value of null index
  virtual Integer nullIndex() const = 0 ;


public:
  //! Permet de g�rer la mort d'une famille associ�e � l'index-manager
  /*! M�thode de bas niveau pour les impl�mentationsde IAbstractFamily, 
   *  usuellement dans le desctructeur des impl�mentations ext�rieures de IAbstractFamily
   */
  virtual void keepAlive(const IAbstractFamily * family) = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_IINDEX_MANAGER_H */
