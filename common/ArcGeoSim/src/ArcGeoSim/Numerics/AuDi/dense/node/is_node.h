// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef __audi_dense_node_is_node_h__
#define __audi_dense_node_is_node_h__

#include "ArcGeoSim/Numerics/AuDi/dense/node/node.h"

#include <type_traits>

namespace audi {

  template<typename E>
  struct is_node
  {
    enum { value = std::is_base_of<node,E>::value };
  };

}

#endif /* __audi_dense_node_is_node_h__ */
