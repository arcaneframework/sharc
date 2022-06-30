// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_UTILS_MESHPARTIALVARIABLEARRAY2PROXY_H
#define ARCGEOSIM_ARCGEOSIM_UTILS_MESHPARTIALVARIABLEARRAY2PROXY_H
/* Author : desrozis at Wed Dec 19 12:20:34 2012
 * Generated by createNew
 */

#include <arcane/ArcaneTypes.h>
#include <arcane/MeshVariable.h>
#include <arcane/ItemTypes.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Pour avoir une API similaire et a priori bientot dans Arcane
ARCANE_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T>
class ItemPartialVariableArray2RefT : public ItemPartialVariableArrayRefT<T>
{
  typedef ItemPartialVariableArrayRefT<T> Base;

public:
  
  typedef T DataType;
  typedef Array2<T> ValueType; // Pas encore coherent, il faudrait Array3
  typedef ConstArray2View<T> ConstReturnReferenceType;
  typedef Array2View<T> ReturnReferenceType;
 
public:
  
  ItemPartialVariableArray2RefT(VariableBuildInfo vbi, 
                                eItemKind kind) 
    : Base(vbi,kind) 
    , m_size_x(VariableBuildInfo(vbi.mesh(), vbi.name() + "_Size_X"))
    , m_size_y(VariableBuildInfo(vbi.mesh(), vbi.name() + "_Size_Y")) {}
  
public:
  
  void resize(Integer size_x, Integer size_y) {
    m_size_x = size_x;
    m_size_y = size_y;
    Base::resize(m_size_x()*m_size_y());
  }
  
  Integer dim1Size() const { return m_size_x(); }
  Integer dim2Size() const { return m_size_y(); }

  Array2View<T> operator[](const Item& item) {
    return Array2View<T>(Base::operator[](item).unguardedBasePointer(), m_size_x(), m_size_y());
  }
 
  const Array2View<T> operator[](const Item& item) const {
    return Array2View<T>(Base::operator[](item).unguardedBasePointer(), m_size_x(), m_size_y());
  }
  
  Array2View<T> operator[](const ItemEnumerator& iter) {
    return Array2View<T>(Base::operator[](iter).unguardedBasePointer(), m_size_x(), m_size_y());
  }
 
  const Array2View<T> operator[](const ItemEnumerator& iter) const {
    return Array2View<T>(Base::operator[](iter).unguardedBasePointer(), m_size_x(), m_size_y());
  }

private:
  VariableScalarInteger m_size_x;
  VariableScalarInteger m_size_y;
};

/*---------------------------------------------------------------------------*/

template<typename K, typename T>
class MeshPartialVariableArray2RefT : public ItemPartialVariableArray2RefT<T>
{
public:
  
  MeshPartialVariableArray2RefT(VariableBuildInfo vbi) 
    : ItemPartialVariableArray2RefT<T>(vbi,ItemTraitsT<K>::kind()) {}
  
public:

  Array2View<T> operator[](const K& item) {
    return ItemPartialVariableArray2RefT<T>::operator[](item);
  }
 
  const Array2View<T> operator[](const K& item) const {
    return ItemPartialVariableArray2RefT<T>::operator[](item);
  }
 
  Array2View<T> operator[](const ItemEnumeratorT<K>& iter) {
    return ItemPartialVariableArray2RefT<T>::operator[](iter);
  }
 
  const Array2View<T> operator[](const ItemEnumeratorT<K>& iter) const {
    return ItemPartialVariableArray2RefT<T>::operator[](iter);
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef USE_ARCANE_V3
#define VARIABLE_WITH_TYPE(type)                                        \
  typedef MeshPartialVariableArray2RefT<Cell,type> PartialVariableCellArray2##type; \
  typedef MeshPartialVariableArray2RefT<Face,type> PartialVariableFaceArray2##type; \
  typedef MeshPartialVariableArray2RefT<Node,type> PartialVariableNodeArray2##type; \
  typedef MeshPartialVariableArray2RefT<Edge,type> PartialVariableEdgeArray2##type; \
  typedef ItemPartialVariableArray2RefT<type> PartialVariableItemArray2##type; 
#else
#define VARIABLE_WITH_TYPE(type)                                        \
  typedef MeshPartialVariableArray2RefT<Cell,type> PartialVariableCellArray2##type; \
  typedef MeshPartialVariableArray2RefT<Face,type> PartialVariableFaceArray2##type; \
  typedef MeshPartialVariableArray2RefT<Node,type> PartialVariableNodeArray2##type; \
  typedef MeshPartialVariableArray2RefT<Edge,type> PartialVariableEdgeArray2##type; \
  typedef MeshPartialVariableArray2RefT<DualNode,type> PartialVariableDualNodeArray2##type; \
  typedef MeshPartialVariableArray2RefT<Link,type> PartialVariableLinkArray2##type; \
  typedef ItemPartialVariableArray2RefT<type> PartialVariableItemArray2##type; 
#endif

VARIABLE_WITH_TYPE(Real);
VARIABLE_WITH_TYPE(Real2);
VARIABLE_WITH_TYPE(Real2x2);
VARIABLE_WITH_TYPE(Real3);
VARIABLE_WITH_TYPE(Real3x3);
VARIABLE_WITH_TYPE(Integer);
VARIABLE_WITH_TYPE(Int32);
VARIABLE_WITH_TYPE(Int64);

#undef VARIABLE_WITH_TYPE

#ifdef USE_ARCANE_V3
typedef MeshPartialVariableArray2RefT<Cell,Byte> PartialVariableCellArray2Bool;
typedef MeshPartialVariableArray2RefT<Face,Byte> PartialVariableFaceArray2Bool;
typedef MeshPartialVariableArray2RefT<Node,Byte> PartialVariableNodeArray2Bool;
typedef MeshPartialVariableArray2RefT<Edge,Byte> PartialVariableEdgeArray2Bool;
typedef ItemPartialVariableArray2RefT<Byte> PartialVariableItemArray2Bool;
#else
typedef MeshPartialVariableArray2RefT<Cell,Byte> PartialVariableCellArray2Bool;
typedef MeshPartialVariableArray2RefT<Face,Byte> PartialVariableFaceArray2Bool;
typedef MeshPartialVariableArray2RefT<Node,Byte> PartialVariableNodeArray2Bool;
typedef MeshPartialVariableArray2RefT<Edge,Byte> PartialVariableEdgeArray2Bool;
typedef MeshPartialVariableArray2RefT<DualNode,Byte> PartialVariableDualNodeArray2Bool;
typedef MeshPartialVariableArray2RefT<Link,Byte> PartialVariableLinkArray2Bool;
typedef ItemPartialVariableArray2RefT<Byte> PartialVariableItemArray2Bool;
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_UTILS_MESHPARTIALVARIABLEARRAY2PROXY_H */
