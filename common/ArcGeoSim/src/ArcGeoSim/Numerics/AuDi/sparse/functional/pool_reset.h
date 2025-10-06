#ifndef __audi_sparse_functional_pool_reset_h__
#define __audi_sparse_functional_pool_reset_h__

#include "ArcGeoSim/Numerics/AuDi/memory/pool.h"

namespace audi {

  template<typename T>
  static void pool_reset()
  {
    using node_type = details::node<T>;
    auto& node_pool = pool<node_type>::get();
    node_pool.reset();
  }

}

#endif /* __audi_sparse_functional_pool_reset_h__ */
