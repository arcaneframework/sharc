// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef __audi_dense_expression_h__
#define __audi_dense_expression_h__

namespace audi {

  template<typename E>
  struct expression
  {
    inline const E& staticCast() const
    {
      return static_cast<const E&>(*this);
    }

    inline E& staticCast()
    {
      return static_cast<E&>(*this);
    }
  };

}

#endif /* __audi_dense_expression_h__ */
