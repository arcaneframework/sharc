// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef COMBINEDSEARCH_H
#define COMBINEDSEARCH_H

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/Array.h>

#include "BinarySearch.h"
#include "LinearSearch.h"

using namespace Arcane;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename V>
struct CombinedSearchT
{
  static Integer apply(const V & vec, const typename V::value_type & key)
  {
    Integer ileft = 0;
    Integer iright = vec.size(); 
    if (n>32) {
      Integer vleft = vec[ileft];
      Integer vright = vec[n-1]+1;
      if (v<vleft || v>=vright) return -1;
      do {
        const Integer imid = (iright+ileft)/2;
        const Integer vmid = vec[imid];
        if (v < vmid) { 
          iright = imid; vright = vmid; 
        } else if (v >= vmid) { 
          ileft = imid; vleft = vmid; 
        }
      } while (iright-ileft>32);
    }
    for(Integer i=ileft;i<iright;++i)
      if (v==vec[i])
        return i;
    return -1;
  }
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
