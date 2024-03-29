// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCANEDEMO_SCHEMEAPI2016_DISCRETIZATION_H
#define ARCANEDEMO_SCHEMEAPI2016_DISCRETIZATION_H
/* Author : encheryg at Mon Jan  4 15:35:58 2016
 * Generated by createNew
 */
#include <arcane/ArcaneVersion.h>
#include <arcane/Item.h>
#include <arcane/ItemInternal.h>
#if (ARCANE_VERSION >= 22200)
  #include <arcane/mesh/DoFFamily.h>
  #include <arcane/ConnectivityItemVector.h>
#else
  #include <arcane/dof/DoFFamily.h>
  #if (ARCANE_VERSION > 20305)
    #include <arcane/ConnectivityItemVector.h>
  #endif
#endif

#include <utility>
#include <vector>
#include <type_traits>
#include <set>

namespace Discretization {

  enum class ItemKind : int {Cell=0, Face, Node} ;

  enum class FaceCellInd : int {Back=0, Front} ;

  enum class Pair : int {First=0, Second} ;

  using ItemKindPair = std::pair<ItemKind,ItemKind> ;

  using ItemKindSet = std::set<ItemKind> ;

  using ItemKindPairList = std::vector<ItemKindPair> ;

  template<ItemKind IK> class Item : public Arcane::DoF {
  public:
    Item() : Arcane::DoF() {}
    Item(Arcane::ItemInternal* internal) : Arcane::DoF(internal) {}
    Item(const ItemInternalPtr* internals, Arcane::Integer local_id) : Arcane::DoF(internals, local_id) {}
    Item(const Arcane::Item aItem) : Arcane::DoF(aItem.internal()) {}
  } ;

  using Cell = Item<ItemKind::Cell> ;

  using Face = Item<ItemKind::Face> ;

  using Node = Item<ItemKind::Node> ;

  using Family = Arcane::mesh::DoFFamily ;

  using ItemGroup = Arcane::DoFGroup  ;

//  using ItemVectorBuilder = Arcane::ItemVectorBuilder ;

  using ItemVector = Arcane::ItemVector ;

  // ConnectedItems type introduce to avoid writing tens of #if in the code. This def is less readable and must be temporary
  // Todo when only one API supported, change Discretization::ConnectedItems to Arcane::ItemVectorView everywhere in the code for readability
#if (ARCANE_VERSION < 20305)
  using ConnectedItems = Arcane::ItemVector;
#else
//  using ConnectedItems = Arcane::ItemVectorView; // SDC
  using ConnectedItems = Arcane::ConnectivityItemVector;
#endif


  using CellEnumerator = Arcane::ItemEnumeratorT<Cell> ;
  using FaceEnumerator = Arcane::ItemEnumeratorT<Face> ;
  using NodeEnumerator = Arcane::ItemEnumeratorT<Node> ;

  template<typename T, bool faultCore = false> struct ConvertA2D {} ;
  template<> struct ConvertA2D<Arcane::Cell> {using Type = Cell ;} ;
  template<> struct ConvertA2D<Arcane::Face> {using Type = Face ;} ;
  template<> struct ConvertA2D<Arcane::Node> {using Type = Node ;} ;
  template<> struct ConvertA2D<Contact> {using Type = Face ;} ;
  template<> struct ConvertA2D<Contact, true> {using Type = Cell ;} ;

  template<typename T> struct ConvertD2A {} ;
  template<> struct ConvertD2A<Cell> {using Type = Arcane::Cell ;} ;
  template<> struct ConvertD2A<Face> {using Type = Arcane::Face ;} ;
  template<> struct ConvertD2A<Node> {using Type = Arcane::Node ;} ;

  template<typename T> struct ConvertD2DoF {} ;
  template<> struct ConvertD2DoF<Cell> {using Type = Arcane::DoF ;} ;
  template<> struct ConvertD2DoF<Face> {using Type = Arcane::DoF ;} ;
  template<> struct ConvertD2DoF<Node> {using Type = Arcane::DoF ;} ;

  template<typename T> struct Group {} ;
  template<> struct Group<Arcane::Cell> {using Type = Arcane::CellGroup ;} ;
  template<> struct Group<Arcane::Face> {using Type = Arcane::FaceGroup ;} ;
  template<> struct Group<Arcane::Node> {using Type = Arcane::NodeGroup ;} ;
  template<> struct Group<Contact> {using Type = ContactGroup ;} ;

  template<typename T> struct Enum {} ;
  template<> struct Enum<Arcane::Cell> {using Type = Arcane::ItemEnumeratorT<Arcane::Cell> ;} ;
  template<> struct Enum<Arcane::Face> {using Type = Arcane::ItemEnumeratorT<Arcane::Face> ;} ;
  template<> struct Enum<Arcane::Node> {using Type = Arcane::ItemEnumeratorT<Arcane::Node> ;} ;
  template<> struct Enum<Contact> {using Type = ContactEnumerator ;} ;
}

#define ENUMERATE_DISCRETIZATION_CELL(name, group) \
for( ::Arcane::ItemEnumeratorT< Discretization::Cell > name((group).enumerator()); name.hasNext(); ++name)

#define ENUMERATE_DISCRETIZATION_FACE(name, group) \
for( ::Arcane::ItemEnumeratorT< Discretization::Face > name((group).enumerator()); name.hasNext(); ++name)

#define ENUMERATE_DISCRETIZATION_NODE(name, group) \
for( ::Arcane::ItemEnumeratorT< Discretization::Node > name((group).enumerator()); name.hasNext(); ++name)

#endif /* ARCANEDEMO_SCHEMEAPI2016_DISCRETIZATION_H */
