#ifndef __audi_sparse_functional_comparison_h__
#define __audi_sparse_functional_comparison_h__

#include "ArcGeoSim/Numerics/AuDi/sparse/scalar_wrapper.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/expression.h"

namespace audi {

  // TODO use template to do it with scalar wrapper
  template<typename U, typename V>
  inline bool operator<=(const sparse_expression<U>& u, const sparse_expression<V>& v)
  {
    return u.value() <= v.value();
  }

  template<typename U, typename V>
  inline bool operator<(const sparse_expression<U>& u, const sparse_expression<V>& v)
  {
    return u.value() < v.value();
  }

  template<typename U, typename V>
  inline bool operator<=(const sparse_expression<U>& u, V v)
  {
    return u.value() <= v;
  }

  template<typename U, typename V>
  inline bool operator<(const sparse_expression<U>& u, V v)
  {
    return u.value() < v;
  }

  template<typename U, typename V>
  inline bool operator>=(const sparse_expression<U>& u, const sparse_expression<V>& v)
  {
    return u.value() >= v.value();
  }

  template<typename U, typename V>
  inline bool operator>(const sparse_expression<U>& u, const sparse_expression<V>& v)
  {
    return u.value() > v.value();
  }

  template<typename U, typename V>
  inline bool operator>=(const sparse_expression<U>& u, V v)
  {
    return u.value() >= v;
  }

  // attention uniquement cet operateur implemente
  template<typename U, typename V>
  inline bool operator>=(const scalar_wrapper<U>& u, V v)
  {
    return u.value() >= v;
  }

  template<typename U, typename V>
  inline bool operator>(const sparse_expression<U>& u, V v)
  {
    return u.value() > v;
  }
}

#endif /* __audi_dense_functional_comparison_h__ */
