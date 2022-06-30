// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ITEMTOOLS_H
#define ITEMTOOLS_H

#ifdef WIN32
#include <ciso646>
#endif

#include <arcane/IVariable.h>
#include <arcane/Item.h>
#include <arcane/ArcaneVersion.h>
#include <arcane/ItemVector.h>

using namespace Arcane;

/*! \brief retourne l'item associe a un localId pour une variable donnee 
 */ 
Item itemFromLocalId(IVariable * ivar, Integer localId);

/*!
  \struct BinaryGroupOperations
  \brief Binary operations between groups
*/
struct BinaryGroupOperations {
  static ItemVector And(ItemGroup a, ItemGroup b);
  static ItemVector Or(ItemGroup a, ItemGroup b);
  static ItemVector Substract(ItemGroup a, ItemGroup b);
  static ItemVector Concatenate(ItemGroup a, ItemGroup b);
};

#endif /* ITEMTOOLS_H */
