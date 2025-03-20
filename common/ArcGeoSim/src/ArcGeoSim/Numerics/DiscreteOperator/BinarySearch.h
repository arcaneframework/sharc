// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef BINARYSEARCH_H
#define BINARYSEARCH_H

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/Array.h>

using namespace Arcane;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename V>
Integer binarySearch(const V & vec, Integer start, Integer end, const typename V::value_type & key)
{
  //   ARCANE_ASSERT(start<=end, ("start>end in binary search"));
  if(start>end) {
    return -1;
  }

  // Find the middle element of the vector and use that for splitting
  // the array into two pieces.
  unsigned middle = (start + ((end - start) / 2));

  if(vec[middle] == key) {
    return middle;
  } else if(vec[middle]>key) {
    return binarySearch(vec, start, middle-1, key);
  }

  return binarySearch(vec, middle+1, end, key);
}

/*----------------------------------------------------------------------------*/

template<typename V>
struct BinarySearchT
{
  static Integer apply(const V & vec, const typename V::value_type & key)
  {
    return binarySearch<V>(vec, 0, vec.size(), key);
  }
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
