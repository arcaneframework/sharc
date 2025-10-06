#ifndef __audi_dense_functional_unpack_h__
#define __audi_dense_functional_unpack_h__

#include "ArcGeoSim/Numerics/AuDi/dense/assign.h"
#include "ArcGeoSim/Numerics/AuDi/dense/expression.h"

namespace audi {

  template<typename T>
  struct Promotion
  {
	static /* constexpr */ const T* t /* = nullptr */;

    using type = decltype(t->value());
  };

  template<typename T>
  static scalar<typename Promotion<T>::type> unpack(const expression<T>& e)
  {
    scalar<typename Promotion<T>::type> n;
    assign(n, e.staticCast());
    return n;
  }

}

#endif /* __audi_dense_functional_unpack_h__ */
