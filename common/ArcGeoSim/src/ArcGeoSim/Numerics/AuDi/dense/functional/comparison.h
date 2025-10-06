#ifndef __audi_dense_functional_comparison_h__
#define __audi_dense_functional_comparison_h__

#include "ArcGeoSim/Numerics/AuDi/dense/expression.h"

namespace audi {

  template<typename U, typename V>
  inline bool operator<=(const expression<U>& u, const expression<V>& v)
  {
    return u.staticCast().value() <= v.staticCast().value();
  }

  template<typename U, typename V>
  inline bool operator<(const expression<U>& u, const expression<V>& v)
  {
    return u.staticCast().value() < v.staticCast().value();
  }

  template<typename U, typename V>
  inline bool operator<=(const expression<U>& u, V v)
  {
    return u.staticCast().value() <= v;
  }

  template<typename U, typename V>
  inline bool operator<(const expression<U>& u, V v)
  {
    return u.staticCast().value() < v;
  }

  template<typename U, typename V>
  inline bool operator>=(const expression<U>& u, const expression<V>& v)
  {
    return u.staticCast().value() >= v.staticCast().value();
  }

  template<typename U, typename V>
  inline bool operator>(const expression<U>& u, const expression<V>& v)
  {
    return u.staticCast().value() > v.staticCast().value();
  }

  template<typename U, typename V>
  inline bool operator>=(const expression<U>& u, V v)
  {
    return u.staticCast().value() >= v;
  }

  template<typename U, typename V>
  inline bool operator>(const expression<U>& u, V v)
  {
    return u.staticCast().value() > v;
  }
}

#endif /* __audi_dense_functional_comparison_h__ */
