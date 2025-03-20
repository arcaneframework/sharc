// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_ENUMERATOR_H
#define ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_ENUMERATOR_H

#include "ArcGeoSim/Utils/ArcGeoSim.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Outil d'�numeration d'un invoker pour un groupe
struct GroupEnumerator
{
  GroupEnumerator(Arcane::ItemGroup group) 
    : m_group(group) {}
  
  template<typename V> // V est un �valuateur
  void operator()(V& v) const
  {
    ENUMERATE_ITEM(iitem, m_group)
    {
      //int lid = iitem->localId() ; // line for totalview debug
      v(iitem);
    }
  }
  
  Arcane::ItemGroup m_group;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Outil d'�numeration d'un invoker pour un range
struct RangeEnumerator
{
  RangeEnumerator(Integer begin, Integer end) 
    : m_begin(begin)
    , m_end(end) {}
  
  template<typename V> // V est un �valuateur
  void operator()(V& v) const
  {
    for(Integer i = m_begin; i < m_end; ++i) v(i);
  }

  Integer m_begin, m_end;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Outil d'�numeration d'un invoker pour un scalaire
struct NoneEnumerator
{
  NoneEnumerator() {}

  template<typename V> // V est un �valuateur
  void operator()(V& v) const
  {
    //throw Arcane::FatalErrorException("Evaluator for Scalar TODO");
    v();
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_ENUMERATOR_H */
