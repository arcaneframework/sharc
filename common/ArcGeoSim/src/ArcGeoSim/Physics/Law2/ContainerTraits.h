// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_PHYSICS_LAW_CONTAINERTRAITS_H
#define ARCGEOSIM_PHYSICS_LAW_CONTAINERTRAITS_H

#include "ArcGeoSim/Physics/Law2/Property.h"

/*
 * \ingroup Law
 * \brief Informations statiques des conteneurs de lois pour une propri�t�
 *
 * D�crit les conteneurs :
 *   - pour les states
 *   - pour les variables
 *   - pour les variables partielles
 *   - pour les tableaux
 *
 * L'id�e est d'utiliser au plus les conteneurs Arcane
 *
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Pour les states

template<typename T> struct StateContainer;

template<typename T>
struct StateContainer< PropertyT<eScalar,T> > 
{ 
  typedef T                      ValueType;
  typedef Arcane::SharedArray<T> DerivType;
  typedef Arcane::ArrayView<T>   DerivViewType;
};

template<typename T>
struct StateContainer< PropertyT<eVectorial,T> > 
{ 
  typedef Arcane::SharedArray<T>  ValueType;
  typedef Arcane::SharedArray2<T> DerivType;
  typedef Arcane::Array2View<T>   DerivViewType;
};

/*---------------------------------------------------------------------------*/

// Pour les tableaux

template<typename T> struct ArrayContainer;

template<typename T>
struct ArrayContainer< PropertyT<eScalar,T> > 
{ 
  typedef Arcane::VariableRefArrayT<T>  ValueType;
  typedef Arcane::VariableRefArray2T<T> DerivType;
};

template<typename T>
struct ArrayContainer< PropertyT<eVectorial,T> > 
{ 
  typedef Arcane::VariableRefArray2T<T> ValueType;
  typedef Arcane::VariableRefArray3T<T> DerivType;
};

/*---------------------------------------------------------------------------*/

// Pour les scalaires

template<typename T> struct ScalarContainer;

template<typename T>
struct ScalarContainer< PropertyT<eScalar,T> >
{
  typedef Arcane::VariableRefScalarT<T> ValueType;
  typedef Arcane::VariableRefArrayT<T>  DerivType;
};

template<typename T>
struct ScalarContainer< PropertyT<eVectorial,T> >
{
  typedef Arcane::VariableRefArrayT<T>  ValueType;
  typedef Arcane::VariableRefArray2T<T> DerivType;
};

/*---------------------------------------------------------------------------*/

// Pour les variables

template<typename T> struct GenericVariableContainer;

template<typename T>
struct GenericVariableContainer< PropertyT<eScalar,T> > 
{ 
  typedef Arcane::ItemVariableScalarRefT<T> ValueType;
  typedef Arcane::ItemVariableArrayRefT<T>  DerivType;
};

template<typename T>
struct GenericVariableContainer< PropertyT<eVectorial,T> > 
{ 
  typedef Arcane::ItemVariableArrayRefT<T>  ValueType;
  typedef Arcane::ItemVariableArray2RefT<T> DerivType; 
};

template<typename T, typename K> struct VariableContainer;

template<typename T, typename K>
struct VariableContainer< PropertyT<eScalar,T>, K > 
{ 
  typedef Arcane::MeshVariableScalarRefT<K,T> ValueType;
  typedef Arcane::MeshVariableArrayRefT<K,T>  DerivType;
};

template<typename T, typename K>
struct VariableContainer< PropertyT<eVectorial,T>, K > 
{ 
  typedef Arcane::MeshVariableArrayRefT<K,T>  ValueType;
  typedef Arcane::MeshVariableArray2RefT<K,T> DerivType;
};

/*---------------------------------------------------------------------------*/

// Pour les variables partielles

template<typename T> struct GenericPartialVariableContainer;

template<typename T>
struct GenericPartialVariableContainer< PropertyT<eScalar,T> > 
{ 
  typedef Arcane::ItemPartialVariableScalarRefT<T> ValueType;
  typedef Arcane::ItemPartialVariableArrayRefT<T>  DerivType;
};

template<typename T>
struct GenericPartialVariableContainer< PropertyT<eVectorial,T> > 
{ 
  typedef Arcane::ItemPartialVariableArrayRefT<T>  ValueType;
  typedef Arcane::ItemPartialVariableArray2RefT<T> DerivType; 
};

template<typename T, typename K> struct PartialVariableContainer;

template<typename T, typename K>
struct PartialVariableContainer< PropertyT<eScalar,T>, K > 
{ 
  typedef Arcane::MeshPartialVariableScalarRefT<K,T> ValueType;
  typedef Arcane::MeshPartialVariableArrayRefT<K,T>  DerivType;
};

template<typename T, typename K>
struct PartialVariableContainer< PropertyT<eVectorial,T>, K > 
{ 
  typedef Arcane::MeshPartialVariableArrayRefT<K,T>  ValueType;
  typedef Arcane::MeshPartialVariableArray2RefT<K,T> DerivType;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_PHYSICS_LAW_PROPERTYTRAITS_H */
