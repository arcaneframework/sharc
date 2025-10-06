// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef LINEARSEARCH_H
#define LINEARSEARCH_H

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/Array.h>

using namespace Arcane;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename V>
struct LinearSearchT
{
  static Integer apply(const V & vec, const typename V::value_type & key)
  {
    Integer i=0;
    for(typename V::const_iterator it=vec.begin(); it!=vec.end(); it++, i++)
      if(*it==key) return i;
    return -1;
  }
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
