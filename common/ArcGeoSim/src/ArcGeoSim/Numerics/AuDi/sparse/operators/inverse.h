#ifndef __audi_sparse_operators_inverse_h__
#define __audi_sparse_operators_inverse_h__

#include "ArcGeoSim/Numerics/AuDi/sparse/functional/make_node.h"

template<class T, class R>
inline audi::sparse_expression <T> operator/(R u, const audi::scalar_wrapper <T> &v) {
  return audi::sparse_expression<T>(u / v.value(), audi::make_node( - u / (v.value()*v.value()), v));
}

template<class T, class R>
inline audi::sparse_expression <T> operator/(const audi::scalar_wrapper <T> &u, R v) {
  return audi::sparse_expression<T>(u.value() / v, audi::make_node( T(1) / v, u));
}

template<class T, class R>
inline audi::sparse_expression<T> operator/(R u, const audi::sparse_expression<T>& v) {
  return audi::sparse_expression<T>(u / v.first, audi::make_node( - u / (v.first*v.first), v));
}

template<class T, class R>
inline audi::sparse_expression<T> operator/(const audi::sparse_expression<T>& u, R v) {
  return audi::sparse_expression<T>(u.first / v, audi::make_node( T(1) / v, u));
}

#endif /* __audi_sparse_operators_inverse_h__ */
