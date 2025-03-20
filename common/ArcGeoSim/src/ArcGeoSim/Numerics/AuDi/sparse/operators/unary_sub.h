#ifndef __audi_sparse_operators_unary_sub_h__
#define __audi_sparse_operators_unary_sub_h__

#include "ArcGeoSim/Numerics/AuDi/sparse/functional/make_node.h"

template<class T, class R>
inline audi::sparse_expression<T> operator-(R u, const audi::scalar_wrapper<T>& v) {
  return audi::sparse_expression<T>(u - v.value(), audi::make_node(T(-1), v));
}

template<class T, class R>
inline audi::sparse_expression<T> operator-(const audi::scalar_wrapper<T>& u, R v) {
  return audi::sparse_expression<T>(u.value() - v, audi::make_node(T(1), u));
}

template<class T, class R>
inline audi::sparse_expression<T> operator-(R u, const audi::sparse_expression<T>& v) {
  if(u==0) return v;
  else return audi::sparse_expression<T>(u - v.first, audi::make_node(T(-1), v));
}

template<class T, class R>
inline audi::sparse_expression<T> operator-(const audi::sparse_expression<T>& u, R v) {
  if(v==0) return u;
  else return audi::sparse_expression<T>(u.first - v, audi::make_node(T(1), u));
}

#endif /* __audi_sparse_operators_unary_sub_h__ */
