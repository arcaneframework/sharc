#ifndef __audi_sparse_functional_make_node_h__
#define __audi_sparse_functional_make_node_h__

#include "ArcGeoSim/Numerics/AuDi/memory/pool.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/scalar_wrapper.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/expression.h"

namespace audi {

  template<typename T>
  static details::node<T>* make_node(T coef, const scalar_wrapper<T>& s, details::node<T>* next = nullptr)
  {
    if(coef == T(0)) return next;
    if(s.indexes().size() == 0) return next;
    using node_type = details::node<T>;
    auto& node_pool = pool<node_type>::get();
    auto* n = node_pool();
    n->coef = coef;
    n->size = s.size();
    n->gradient = s.gradient().data();
    n->indexes = s.indexes().data();
    n->nested = nullptr;
    n->next = next;
    return n;
  }

  template<typename T>
  static details::node<T>* make_node(T coef, const sparse_expression<T>& s, details::node<T>* next = nullptr)
  {
    if(coef == T(0)) return next;
    if(s.second == nullptr) return next;
    using node_type = details::node<T>;
    auto& node_pool = pool<node_type>::get();
    auto* n = node_pool();
    n->coef = coef;
    n->size = 0;
    n->gradient = nullptr;
    n->indexes = nullptr;
    n->nested = s.second;
    n->next = next;
    return n;
  }

}

#endif /* __audi_sparse_functional_make_node_h__ */
