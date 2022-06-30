// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_UTILS_ITEMGROUPMAP_H
#define ARCGEOSIM_ARCGEOSIM_UTILS_ITEMGROUPMAP_H

/* Sp�cialement pour certains tests de CooresArcane */
// #define ITEMGROUPMAP_FILLINIT

#ifdef WIN32
#include <ciso646>
#endif

#include <arcane/ArcaneVersion.h>
#include <arcane/Item.h>
#include <arcane/utils/HashTableMap.h>
#include <arcane/ItemGroup.h>
#include <arcane/ItemGroupRangeIterator.h>
#include <arcane/ItemEnumerator.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/Trace.h>
#include <arcane/utils/TraceInfo.h>
#include <arcane/ItemPrinter.h>

#include <arcane/ItemVector.h>
#include <arcane/ItemVectorView.h>
#include <arcane/mesh/ItemFamily.h>

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#ifndef ITEMGROUP_USE_OBSERVERS
#error "This implementation of ItemGroupMap is only available with new ItemGroupImpl Observers"
#endif /* ITEMGROUP_USE_OBSERVERS */

//! Classe fondamentale technique des ItemGroupMap's
/*! \todo optimiser la table de hachage vu que l'on connait l'ensemble
 *  des indices � la construction (les collisions sont consultables via stats())
 */
class ItemGroupMapAbstract
  : protected Arcane::HashTableBase
{
protected:
  typedef Integer KeyTypeValue;
  typedef Arcane::HashTraitsT<KeyTypeValue> KeyTraitsType;
  typedef KeyTraitsType::KeyTypeConstRef KeyTypeConstRef;

public:
  enum Property { };

public:
  //! Constructeur
  ItemGroupMapAbstract();
    
  //! Destructeur
  virtual ~ItemGroupMapAbstract();

  //! Initialisation sur un nouveau groupe
  virtual void init(const Arcane::ItemGroup & group) = 0;

  //! Indique si l'objet a �t� invalid�
  bool isValid() const { return m_is_valid ; }

  //! Acc�s aux stats
  /*! Inclus la description des collisions */
  void stats(std::ostream & o);

  //! Acc�s au groupe associ�
  Arcane::ItemGroup group() const { return m_group; }

  //! Controle la coh�rence du groupe avec le groupe de r�f�rence
  virtual bool checkSameGroup(const Arcane::ItemGroup & group) const;

  //! Controle la coh�rence du groupe avec le groupe de r�f�rence
  virtual bool checkSameGroup(const Arcane::ItemVectorView & group) const;
  
  //! Acc�s aux propri�t�s (lecture seule)
  virtual Integer properties() const { return m_properties; }

  //! Modification des propri�t�s (activation)
  virtual void setProperties(const Integer property);

  //! Modification des propri�t�s (d�sactivation)
  virtual void unsetProperties(const Integer property);

  //! Nom de l'objet
  String name() const;

protected:
  //! Initialisation de bas niveau
  void _init(const Arcane::ItemGroup & group);

  //! Installe les observers adapt�s aux propri�t�s
  void _updateObservers(Arcane::ItemGroupImpl * group = NULL);

  //! Invalide la relation avec son groupe
  void _executeInvalidate();

  //! Test l'int�grit� de l'ItemGroupMap relativement � son groupe
  bool _checkGroupIntegrity() const;

  //! Fonction de hachage
  /*! Utilise la fonction de hachage de Arcane m�me si quelques
   *  collisions sont constat�es avec les petites valeurs */
  Integer _hash(KeyTypeConstRef id) const;

  //! \a true si une valeur avec la cl� \a id est pr�sente
  bool _hasKey(KeyTypeConstRef id) const;

  //! Recherche d'une clef dans un bucket
  Integer _lookupBucket(Integer bucket, KeyTypeConstRef id) const;

  //! Recherche d'une clef dans toute la table
  Integer _lookup(KeyTypeConstRef id) const;

  //! Teste l'initialisation de l'objet
  bool _initialized() const;

  //! G�n�re une erreur de recherche
  void _throwItemNotFound(const Arcane::TraceInfo & info, const Item & item) const throw();

  //! Acc�s aux traces
  Arcane::ITraceMng * traceMng() const;

protected:
  Arcane::ItemGroupImpl * m_group; //!< Groupe associ�
  Integer m_properties; //! Propri�t�s
  Arcane::UniqueArray<KeyTypeValue> m_key_buffer; //! Table des cl�s associ�es
  Arcane::UniqueArray<Integer> m_next_buffer; //! Table des index suivant associ�s
  Arcane::UniqueArray<Integer> m_buckets; //! Tableau des buckets
  String m_stacktrace;
  bool  m_is_valid ;
};

/*---------------------------------------------------------------------------*/

//! Classe d'impl�mentation des parties communes aux ItemGroupMap
template<typename _ValueType>
class ItemGroupMapAbstractT
  : public ItemGroupMapAbstract
{
public:
  typedef _ValueType ValueType;

protected:
  typedef ItemGroupMapAbstractT<ValueType> ThatClass;
  typedef typename Arcane::UniqueArray<ValueType>::ConstReferenceType  ConstReferenceType;

public:

  //! \brief Constructeur d'une table vide
  ItemGroupMapAbstractT()
    : ItemGroupMapAbstract()
  {
    ;
  }

  //! \brief Constructeur d'une table adapt�e � un groupe
  ItemGroupMapAbstractT(const Arcane::ItemGroup & group)
    : ItemGroupMapAbstract()
  {
    init(group);
  }
  
  //! Destructeur
  virtual ~ItemGroupMapAbstractT()
  {
    ;
  }

private:
  //! Constructeur par copie
  /*! Zone priv� + non d�fini => usage interdit */
  ItemGroupMapAbstractT(const ItemGroupMapAbstractT &);

  //! Op�rateur de recopie
  /*! Zone priv� + non d�fini => usage interdit */
  const ThatClass& operator=(const ThatClass& from);

public:
  //! Initialise la structure sur un groupe
  void init(const Arcane::ItemGroup & group)
  {
    _init(group);
    m_data_buffer.resize(group.size());
#ifdef ITEMGROUPMAP_FILLINIT
    fill(ValueType());
#endif /* ITEMGROUPMAP_FILLINIT */
    ARCANE_ASSERT( (_checkGroupIntegrity()), ("ItemGroupMap integrity failed") );
  }

  //! Remplit la structure avec une valeur constante
  void fill(const ValueType & v) 
  {
    m_data_buffer.fill(v);
  }

public:
  /*! \brief Recherche la valeur correspondant � l'item \a item
   *
   * Une exception est g�n�r�e si la valeur n'est pas trouv�.
   */
  inline ConstReferenceType operator[](const Item & item) const throw()
  {
    ARCANE_ASSERT((m_is_valid),("Inconsistent ItemGroupMap since invalidation"));
    const Integer i = _lookup(item.localId());
    if (i<0) this->_throwItemNotFound(A_FUNCINFO,item);
    return m_data_buffer[i];
  }

  /*! \brief Recherche la valeur correspondant � l'item \a item
   *
   * Une exception est g�n�r�e si la valeur n'est pas trouv�.
   */
  inline ValueType& operator[](const Item & item) throw()
  {
    ARCANE_ASSERT((m_is_valid),("Inconsistent ItemGroupMap since invalidation"));
    const Integer i = _lookup(item.localId());
    if (i<0) this->_throwItemNotFound(A_FUNCINFO,item);
    return m_data_buffer[i];
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un iterateur
   */
  inline ConstReferenceType operator[](const Arcane::ItemGroupRangeIterator & iter) const
  {
    ARCANE_ASSERT((m_is_valid),("Inconsistent ItemGroupMap since invalidation"));
    ARCANE_ASSERT((_lookup(iter.itemLocalId()) == iter.index()),("Inconsistency detected on item id"));
    return m_data_buffer[iter.index()];
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un iterateur
   */
  inline ValueType& operator[](const Arcane::ItemGroupRangeIterator & iter)
  {
    ARCANE_ASSERT((m_is_valid),("Inconsistent ItemGroupMap since invalidation"));
    ARCANE_ASSERT((_lookup(iter.itemLocalId()) == iter.index()),("Inconsistency detected on item id"));
    return m_data_buffer[iter.index()];
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  inline ConstReferenceType operator[](const Arcane::ItemEnumeratorT<Item> & iter) const
  {
    ARCANE_ASSERT((m_is_valid),("Inconsistent ItemGroupMap since invalidation"));
    ARCANE_ASSERT((_lookup(iter.itemLocalId()) == iter.index()),("Inconsistency detected on item id"));
    return m_data_buffer[iter.index()];
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  inline ValueType& operator[](const Arcane::ItemEnumeratorT<Item> & iter)
  {
    ARCANE_ASSERT((m_is_valid),("Inconsistent ItemGroupSet since invalidation"));
    ARCANE_ASSERT((_lookup(iter.itemLocalId()) == iter.index()),("Inconsistency detected on item id"));
    return m_data_buffer[iter.index()];
  }

  //! Test l'existence d'une cl�
  inline bool hasKey(const Item & item) const
  {
    ARCANE_ASSERT((m_is_valid),("Inconsistent ItemGroupMap since invalidation"));
    return _hasKey(item.localId());
  }

protected:
  Arcane::UniqueArray<ValueType> m_data_buffer;
};

/*---------------------------------------------------------------------------*/

//! \brief Classe de base pour les ItemGroupMap
/*  Contient la majorit� des fonctionnalit�s mais sans distinction du
 *  type d'item/
 *
 *  Permet d'ind�xer un tableau par les items d'un groupe. Ceci �vite
 *  des erreurs principalement en parall�le ou certains items ne sont
 *  pas contigus en localId (et �vite le bug d'index� un Array via les
 *  localId des items).
 *
 *  Il n'est possible d'acc�der qu'� des items du groupe
 *  original. Retourne une exception si l'item demand� n'est pas
 *  r�f�renc�. L'acc�s est optimiser pour les ItemGroupRangeIterator
 *
 *  Cette impl�mentation stocke les valeurs dans la structure Data de
 *  la table de hachage. Elle ne sert qu'� typer les Items.
 *
 * \see ItemGroupMapT et ItemGroupMapArrayT
 */
template <typename ValueTypeT>
class ItemGroupMapBaseT : 
  public ItemGroupMapAbstractT<ValueTypeT>
{
protected:
  typedef ItemGroupMapAbstractT<ValueTypeT> BaseClass;

public:
  typedef typename BaseClass::ConstReferenceType ConstReferenceType ;

public:
  //! Constructeur par d�faut
  ItemGroupMapBaseT() 
    : BaseClass()
  {
    ;
  }
  
  //! Constructeur � partir d'un groupe
  ItemGroupMapBaseT(const Arcane::ItemGroup & group) 
    : BaseClass()
  {
    init(group);
  }

  //! Destructeur
  virtual ~ItemGroupMapBaseT()
  {
    ;
  }
  
  //! Initialisation sur un nouveau groupe
  void init(const Arcane::ItemGroup & group) 
  {
    // Met la valeur par d�faut en association aux clefs
    BaseClass::init(group);
  }

  //! Op�rateur d'acc�s � partir d'un item
  ValueTypeT & operator[](const Item & item) 
  {
    return BaseClass::operator[](item);
  }

  //! Op�rateur d'acc�s � partir d'un ItemGroupRangeIteratorT
  ValueTypeT & operator[](const Arcane::ItemGroupRangeIterator & iter)
  {
    return BaseClass::operator[](iter);
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un item
  ConstReferenceType operator[](const Item & item) const
  {
    return BaseClass::operator[](item);
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un ItemGroupRangeIteratorT
  ConstReferenceType operator[](const Arcane::ItemGroupRangeIterator & iter) const
  {
    return BaseClass::operator[](iter);
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  inline ConstReferenceType operator[](const Arcane::ItemEnumerator & iter) const
  {
    return BaseClass::operator[](iter);    
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  inline ValueTypeT& operator[](const Arcane::ItemEnumerator & iter)
  {
    return BaseClass::operator[](iter);
  }

  //! Teste l'existence d'un item en temps que clef
  bool hasKey(const Item & item) const
  {
    return BaseClass::hasKey(item);
  }
};

/*---------------------------------------------------------------------------*/

/*! \brief Forme de tableau ind�x� sur un groupe d'items de type ItemKind
 *
 *  Permet d'ind�xer un tableau par les items d'un groupe. Ceci �vite
 *  des erreurs principalement en parall�le ou certains items ne sont
 *  pas contigus en localId (et �vite le bug d'index� un Array via les
 *  localId des items).
 *
 *  Il n'est possible d'acc�der qu'� des items du groupe
 *  original. Retourne une exception si l'item demand� n'est pas
 *  r�f�renc�. L'acc�s est optimiser pour les ItemGroupRangeIterator
 *
 *  Cette impl�mentation stocke les valeurs dans la structure Data de
 *  la table de hachage. Elle ne sert qu'� typer les Items.
 *
 * \see ItemGroupMapArrayT
 */
template <typename ItemKind, typename ValueTypeT>
class ItemGroupMapT : 
  public ItemGroupMapAbstractT<ValueTypeT>
{
protected:
  typedef ItemGroupMapAbstractT<ValueTypeT> BaseClass;

public:
  typedef typename BaseClass::ConstReferenceType ConstReferenceType ;

public:
  //! Constructeur par d�faut
  ItemGroupMapT()
    : BaseClass()
  {
    ;
  }
  
  //! Constructeur � partir d'un groupe
  ItemGroupMapT(const Arcane::ItemGroupT<ItemKind> & group) 
    : BaseClass()
  {
    init(group);
  }
  
  ItemGroupMapT(const ItemGroupMapT& rhs)
  : BaseClass()
  {
    BaseClass::init(Arcane::ItemGroup(rhs.m_group));
    this->m_data_buffer.copy(rhs.m_data_buffer) ;
  }

  //! Destructeur
  virtual ~ItemGroupMapT()
  {
    ;
  }

  //! Initialise la structure sur un groupe
  /*! C++ requieres explicit redefinition since init method is overloaded here */
  void init(const Arcane::ItemGroup & group) { return ItemGroupMapAbstractT<ValueTypeT>::init(group); }

  //! Initialisation sur un nouveau groupe
  void init(const Arcane::ItemGroupT<ItemKind> & group) 
  {
    // Met la valeur par d�faut en association aux clefs
    BaseClass::init(group);
  }

  //! Op�rateur d'acc�s � partir d'un item
  ValueTypeT & operator[](const ItemKind & item) 
  {
    return BaseClass::operator[](item);
  }

  //! Op�rateur d'acc�s � partir d'un ItemGroupRangeIteratorT
  ValueTypeT & operator[](const Arcane::ItemGroupRangeIteratorT<ItemKind> & iter)
  {
    return BaseClass::operator[](iter);
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un item
  ConstReferenceType operator[](const ItemKind & item) const
  {
    return BaseClass::operator[](item);
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un ItemGroupRangeIteratorT
  ConstReferenceType operator[](const Arcane::ItemGroupRangeIteratorT<ItemKind> & iter) const
  {
    return BaseClass::operator[](iter);
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  inline ConstReferenceType operator[](const Arcane::ItemEnumeratorT<ItemKind> & iter) const
  {
    return BaseClass::operator[](iter);    
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  inline ValueTypeT& operator[](const Arcane::ItemEnumeratorT<ItemKind> & iter)
  {
    return BaseClass::operator[](iter);    
  }

  bool hasKey(const ItemKind & item) const
  {
    return BaseClass::hasKey(item);
  }
};

/*---------------------------------------------------------------------------*/

//! \brief Classe de base pour les ItemGroupMapArray
/*  Contient la majorit� des fonctionnalit�s mais sans distinction du
 *  type d'item/
 *
 *  Permet d'ind�x� un tableau par les items d'un groupe. Ceci �vite
 *  des erreurs principalement en parall�le ou certains items ne sont
 *  pas contigus en localId (et �vite le bug d'index� un Array via les
 *  localId des items).
 *
 *  Il n'est possible d'acc�der qu'� des items du groupe
 *  original. Retourne une exception si l'item demand� n'est pas
 *  r�f�renc�. L'acc�s en optimiser pour les ItemGroupRangeIterator
 *
 * \see ItemGroupMapT
 */
template <typename ValueTypeT>
class ItemGroupMapArrayBaseT : 
  public ItemGroupMapAbstractT<ValueTypeT*>
{
public:
  typedef ArrayView<ValueTypeT> ArrayType;
  typedef ConstArrayView<ValueTypeT> ConstArrayType;

protected:
  typedef ItemGroupMapAbstractT<ValueTypeT*> BaseClass;
  Integer m_array_size;
  SharedArray<ValueTypeT> m_array_data;

public:
  //! Constructeur par d�faut
  ItemGroupMapArrayBaseT() 
    : BaseClass()
    , m_array_size(0)
  {
    ;
  }

  //! Constructeur � partir d'un groupe
  ItemGroupMapArrayBaseT(const Arcane::ItemGroup & group, 
                         const Integer array_size)
    : BaseClass()
  {
    init(group,array_size);
  }

  //! Destructeur
  virtual ~ItemGroupMapArrayBaseT()
  {
    ;
  }
  
  //! Initialisation sur un nouveau groupe
  void init(const Arcane::ItemGroup & group, const Integer array_size) 
  {
    BaseClass::init(group);
    m_array_size = array_size;
    const Integer global_size = group.size()*m_array_size;
    m_array_data.resize(global_size);
    ValueTypeT * base_data = m_array_data.unguardedBasePointer();
    for(Arcane::ItemEnumerator i(group.enumerator()) ; i.hasNext(); ++i)
      {
        ValueTypeT * currentData = base_data + m_array_size * i.index();
        BaseClass::operator[](i) = currentData;
      }
#ifdef ITEMGROUPMAP_FILLINIT
    fill(ValueTypeT());
#endif /* ITEMGROUPMAP_FILLINIT */
  }

  //! Initialisation sans changement de taille des tableaux
  void init(const Arcane::ItemGroup & group)
  {
    init(group,m_array_size);
  }

  //! Remplit la structure avec une valeur constante
  void fill(const ValueTypeT & v) 
  {
    m_array_data.fill(v);
  }

  //! Op�rateur d'acc�s � partir d'un item
  /*! Retourne une exception si l'item n'est pas r�f�renc� */
  ArrayType operator[](const Item & item) 
  {
    return ArrayType(m_array_size,BaseClass::operator[](item));
  }

  //! Op�rateur d'acc�s � partir d'un ItemGroupRangeIteratorT
  /*! Retourne une exception si l'item n'est pas r�f�renc� */
  ArrayType operator[](const Arcane::ItemGroupRangeIterator & iter)
  {
    return ArrayType(m_array_size,BaseClass::operator[](iter));
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  ArrayType operator[](const Arcane::ItemEnumerator & iter)
  {
    return ArrayType(m_array_size,BaseClass::operator[](iter));
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un item
  /*! Retourne une exception si l'item n'est pas r�f�renc� */
  ConstArrayType operator[](const Item & item) const
  {
    return ConstArrayType(m_array_size,BaseClass::operator[](item));
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un ItemGroupRangeIteratorT
  /*! Retourne une exception si l'item n'est pas r�f�renc� */
  ConstArrayType operator[](const Arcane::ItemGroupRangeIterator & iter) const
  {
    return ConstArrayType(m_array_size,BaseClass::operator[](iter));
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  inline const ConstArrayType operator[](const Arcane::ItemEnumerator & iter) const
  {
    return ConstArrayType(m_array_size,BaseClass::operator[](iter));
  }

  bool hasKey(const Item & item) const
  {
    return BaseClass::hasKey(item);
  }
};

/*---------------------------------------------------------------------------*/

/*! \brief Forme de tableau ind�x� sur un groupe d'items � valeurs de type tableau
 *
 *  Permet d'ind�x� un tableau par les items d'un groupe. Ceci �vite
 *  des erreurs principalement en parall�le ou certains items ne sont
 *  pas contigus en localId (et �vite le bug d'index� un Array via les
 *  localId des items).
 *
 *  Il n'est possible d'acc�der qu'� des items du groupe
 *  original. Retourne une exception si l'item demand� n'est pas
 *  r�f�renc�. L'acc�s en optimiser pour les ItemGroupRangeIterator
 *
 * \see ItemGroupMapT
 */
template <typename ItemKind, typename ValueTypeT>
class ItemGroupMapArrayT : 
  public ItemGroupMapArrayBaseT<ValueTypeT>
{
protected:
  typedef ItemGroupMapArrayBaseT<ValueTypeT> BaseClass;
  typedef typename BaseClass::ArrayType ArrayType;
  typedef typename BaseClass::ConstArrayType ConstArrayType;
  
public:
  //! Constructeur par d�faut
  ItemGroupMapArrayT()
    : BaseClass()
  {
    ;
  }
  
  //! Constructeur � partir d'un groupe
  ItemGroupMapArrayT(const Arcane::ItemGroupT<ItemKind> & group, 
                     const Integer array_size)
    : BaseClass(group,array_size)
  {
    ;
  }

  //! Destructeur
  virtual ~ItemGroupMapArrayT()
  {
    ;
  }
  
  //! Initialisation sur un nouveau groupe
  void init(const Arcane::ItemGroupT<ItemKind> & group, const Integer array_size)
  {
    // Met la valeur par d�faut en association aux clefs
    BaseClass::init(group,array_size);
  }

  //! Initialisation sur un nouveau groupe
  void init(const Arcane::ItemGroupT<ItemKind> & group) 
  {
    // Met la valeur par d�faut en association aux clefs
    BaseClass::init(group);
  }

  //! Op�rateur d'acc�s � partir d'un item
  ArrayType operator[](const ItemKind & item) 
  {
    return BaseClass::operator[](item);
  }

  //! Op�rateur d'acc�s � partir d'un ItemGroupRangeIteratorT
  ArrayType operator[](const Arcane::ItemGroupRangeIteratorT<ItemKind> & iter)
  {
    return BaseClass::operator[](iter);
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un item
  ConstArrayType operator[](const ItemKind & item) const
  {
    return BaseClass::operator[](item);
  }

  //! Op�rateur d'acc�s en mode constant � partir d'un ItemGroupRangeIteratorT
  ConstArrayType operator[](const Arcane::ItemGroupRangeIteratorT<ItemKind> & iter) const
  {
    return BaseClass::operator[](iter);
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  ConstArrayType operator[](const Arcane::ItemEnumeratorT<ItemKind> & iter) const
  {
    return BaseClass::operator[](iter);    
  }

  /*! \brief Recherche la valeur correspondant � l'item associ� � un �num�rateur
   */
  ArrayType operator[](const Arcane::ItemEnumeratorT<ItemKind> & iter)
  {
    return BaseClass::operator[](iter);    
  }

  bool hasKey(const ItemKind & item) const
  {
    return BaseClass::hasKey(item);
  }
};

/*---------------------------------------------------------------------------*/

//! Classe vide
/*! Coute un octet en m�moire */
class Void { };
// inline std::ostream & operator<<(std::ostream & o, const Void & v) { return o; }

/*---------------------------------------------------------------------------*/

/*! ItemGroupSet s'utilise essentiellement avec hasKey vu que les donn�es port�es sont Void */
class ItemGroupSet : protected ItemGroupMapAbstractT<Void>
{
public:
  ItemGroupSet() : BaseClass() { }

  ItemGroupSet(const Arcane::ItemGroup & group) : BaseClass(group) { }

  void init(const Arcane::ItemGroup & group)
  {
    BaseClass::init(group);
  }

  bool hasKey(const Item & item) const
  {
    return BaseClass::hasKey(item);
  }

  bool isValid() const 
  {
     return BaseClass::isValid() ;
  }

protected:
  typedef ItemGroupMapAbstractT<Void> BaseClass;
};

#endif /* ARCGEOSIM_ARCGEOSIM_UTILS_ITEMGROUPMAP_H */
