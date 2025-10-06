#ifndef __audi_sparse_operators_mult_h__
#define __audi_sparse_operators_mult_h__

#include "ArcGeoSim/Numerics/AuDi/sparse/functional/make_node.h"

template<class T>
inline audi::sparse_expression<T> operator*(const audi::scalar_wrapper<T>& u, const audi::scalar_wrapper<T>& v) {
  return audi::sparse_expression<T>(u.value() * v.value(), audi::make_node(v.value(), u, audi::make_node(u.value(), v)));
}

template<class T>
inline audi::sparse_expression<T> operator*(const audi::sparse_expression<T>& u, const audi::scalar_wrapper<T>& v) {
  return audi::sparse_expression<T>(u.first * v.value(), audi::make_node(v.value(), u, audi::make_node(u.first, v)));
}

template<class T>
inline audi::sparse_expression<T> operator*(const audi::scalar_wrapper<T>& u, const audi::sparse_expression<T>& v) {
  return audi::sparse_expression<T>(u.value() * v.first, audi::make_node(u.value(), v, audi::make_node(v.first, u)));
}

template<class T>
inline audi::sparse_expression<T> operator*(const audi::sparse_expression<T>& u, const audi::sparse_expression<T>& v) {
  return audi::sparse_expression<T>(u.first * v.first, audi::make_node(v.first, u, audi::make_node(u.first, v)));
}

#endif /* __audi_sparse_operators_add_h__ */
