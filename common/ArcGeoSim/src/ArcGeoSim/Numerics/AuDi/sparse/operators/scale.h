#ifndef __audi_sparse_operators_scale_h__
#define __audi_sparse_operators_scale_h__

#include "ArcGeoSim/Numerics/AuDi/sparse/functional/make_node.h"

/*template<class T>
inline audi::sparse_expression<T> operator=(const audi::scalar_wrapper<T>& v) {
  return audi::sparse_expression<T>(v.value(), audi::make_node(T(1), v),v.to());
}*/

template<class T, class R>
inline audi::sparse_expression <T> operator*(R u, const audi::scalar_wrapper <T> &v) {
  return audi::sparse_expression<T>(u * v.value(), audi::make_node(T(u), v));
}

template<class T, class R>
inline audi::sparse_expression <T> operator*(const audi::scalar_wrapper <T> &u, R v) {
  return audi::sparse_expression<T>(u.value() * v, audi::make_node(T(v), u));
}

template<class T, class R>
inline audi::sparse_expression<T> operator*(R u, const audi::sparse_expression<T>& v) {
  if(u==1) return v;
  else return audi::sparse_expression<T>(u * v.first, audi::make_node(T(u), v));
}

template<class T, class R>
inline audi::sparse_expression<T> operator*(const audi::sparse_expression<T>& u, R v) {
  if(v==1) return u;
  else return audi::sparse_expression<T>(u.first * v, audi::make_node(T(v), u));
}

#endif /* __audi_sparse_operators_scale_h__ */
