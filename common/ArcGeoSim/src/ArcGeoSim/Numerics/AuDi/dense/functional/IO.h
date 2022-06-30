// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef __audi_dense_functional_io_h__
#define __audi_dense_functional_io_h__

#include "ArcGeoSim/Numerics/AuDi/dense/expression.h"

#include <iostream>

namespace audi {

  template<typename T>
  inline std::ostream& operator<<(std::ostream& out, const expression<T>& n)
  {
    const auto& t = n.staticCast();
    out << "value={ " << t.value() << " }, derivatives["
        << t.size() << "]={ ";
    for(auto i = 0u; i < t.size(); ++i) {
      out << t.gradient(i) << " ";
    }
    out << "}";
    return out;
  }

}

#endif /* __audi_dense_functional_io_h__ */
