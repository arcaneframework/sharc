#ifndef __audi_dense_functional_operator_h__
#define __audi_dense_functional_operator_h__

#include "ArcGeoSim/Numerics/AuDi/dense/expression.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/is_node.h"

#include "ArcGeoSim/Numerics/AuDi/dense/node/add.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/unary_add.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/div.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/mult.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/sub.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/unary_sub.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/scale.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/inv_scale.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/inverse.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/opposite.h"
#include "ArcGeoSim/Numerics/AuDi/dense/node/math.h"

namespace audi {

  template<typename L, typename R>
  inline add<L,R> operator+(const expression<L>& l, const expression<R>& r)
  {
    return add<L,R>(l,r);
  }

  template<typename T>
  inline unary_add_by_left<T> operator+(typename unary_add_by_left<T>::data_type r, const expression<T>& l)
  {
    return unary_add_by_left<T>(l,r);
  }

  template<typename T>
  inline unary_add_by_right<T> operator+(const expression<T>& l, typename unary_add_by_right<T>::data_type r)
  {
    return unary_add_by_right<T>(l,r);
  }

  template<typename L, typename R>
  inline div<L,R> operator/(const expression<L>& l, const expression<R>& r)
  {
    return div<L,R>(l,r);
  }

  template<typename T>
  inline inverse<T> operator/(typename inverse<T>::data_type v, const expression<T>& t)
  {
    return inverse<T>(t, v);
  }

  template<typename T>
  inline inv_scale<T> operator/(const expression<T>& t, typename inv_scale<T>::data_type v)
  {
    return inv_scale<T>(t, v);
  }

  template<typename L, typename R>
  inline mult<L,R> operator*(const expression<L>& l, const expression<R>& r)
  {
    return mult<L,R>(l,r);
  }

  template<typename T>
  inline scale_by_right<T> operator*(const expression<T>& t, typename scale_by_right<T>::data_type v)
  {
    return scale_by_right<T>(t, v);
  }

  template<typename T>
  inline scale_by_left<T> operator*(typename scale_by_left<T>::data_type v, const expression<T>& t)
  {
    return scale_by_left<T>(t, v);
  }

  template<typename L, typename R>
  inline sub<L,R> operator-(const expression<L>& l, const expression<R>& r)
  {
    return sub<L,R>(l,r);
  }

  template<typename T>
  inline unary_sub_by_left<T> operator-(typename unary_sub_by_left<T>::data_type r, const expression<T>& l)
  {
    return unary_sub_by_left<T>(l,r);
  }

  template<typename T>
  inline unary_sub_by_right<T> operator-(const expression<T>& l, typename unary_sub_by_right<T>::data_type r)
  {
    return unary_sub_by_right<T>(l,r);
  }

  template<typename T>
  inline opposite<T> operator-(const expression<T>& t)
  {
    return opposite<T>(t);
  }

  template<typename T>
  inline math<T,Sqrt> sqrt(const expression<T>& t)
  {
    return math<T,Sqrt>(t, Sqrt());
  }

}

#endif /* __audi_dense_functional_operator_h__ */
