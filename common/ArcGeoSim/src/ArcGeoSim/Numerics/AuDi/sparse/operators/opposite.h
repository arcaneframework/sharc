#ifndef __audi_sparse_operators_opposite_h__
#define __audi_sparse_operators_opposite_h__

#include "ArcGeoSim/Numerics/AuDi/sparse/functional/make_node.h"

template<class T>
inline audi::sparse_expression<T> operator-(const audi::scalar_wrapper<T>& u) {
  return audi::sparse_expression<T>(- u.value(), audi::make_node(T(-1), u));
}

template<class T>
inline audi::sparse_expression<T> operator-(const audi::sparse_expression<T>& u) {
  return audi::sparse_expression<T>(- u.first, audi::make_node(T(-1), u));
}

#endif /* __audi_sparse_operators_opposite_h__ */
