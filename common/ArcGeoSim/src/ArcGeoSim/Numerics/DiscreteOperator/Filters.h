// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_TESTS_TYPEDGRAPHTESTMNG_FILTERS_H
#define ARCGEOSIM_TESTS_TYPEDGRAPHTESTMNG_FILTERS_H

#include <arcane/Item.h>
#include <arcane/ItemEnumerator.h>

using namespace Arcane;

#define ADD_FILTER(name) \
name FILTER_##name;

struct CellCell {
  bool operator()(const Link& link) const;
};

struct CellFace {
  bool operator()(const Link& link) const;
};

////////////////////////////////////////////////////////////

bool CellCell::operator()(const Link& link) const {
  if(link.dualNodes().count() != 2) return false;

  bool is_cell_to_cell = true;
  for(DualNodeEnumerator inode(link.dualNodes()); inode.hasNext(); ++inode) {
    const DualNode& node = *inode;
    if(!node.isCell()) {
      is_cell_to_cell = false;
      break;
    }
  }
  return is_cell_to_cell;
}

bool CellFace::operator()(const Link& link) const {
  if(link.dualNodes().count() != 2) return false;
  
  bool cell_found = false;
  bool face_found = false;
  for(DualNodeEnumerator inode(link.dualNodes()); inode.hasNext(); ++inode) {
    const DualNode& node = *inode;
    if(node.isCell()) cell_found = true;
    if(node.isFace()) face_found = true;
  }
  return cell_found && face_found;
}

////////////////////////////////////////////////////////////

ADD_FILTER(CellCell);
ADD_FILTER(CellFace);

#endif /* ARCGEOSIM_TESTS_TYPEDGRAPHTESTMNG_FILTERS_H */
