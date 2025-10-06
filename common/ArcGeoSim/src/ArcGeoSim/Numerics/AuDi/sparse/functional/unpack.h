#ifndef __audi_sparse_functional_unpack_h__
#define __audi_sparse_functional_unpack_h__

#include "ArcGeoSim/Numerics/AuDi/dense/scalar.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/functional/IO.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/scalar_wrapper.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/expression.h"

#include <vector>

namespace audi {

  namespace details {

    template<typename T>
    static void unpack(T* s, const std::vector<int>& index_redir, T c, const node<T>& e)
    {
      T coef = c * e.coef;
      for(auto i = 0; i < e.size; ++i) {
        if (e.indexes[i] != -1){
          int redir = index_redir[e.indexes[i]];
          if(redir!=-1)
            s[redir] += coef * e.gradient[i];
        }
      }
      if(e.next) {
        unpack(s, index_redir, c, *(e.next));
      }
      if(e.nested) {
        unpack(s, index_redir, coef, *(e.nested));
      }
    }

    template<typename T>
    static void unpack(T* s, T c, const node<T>& e)
    {
      T coef = c * e.coef;
      for(auto i = 0; i < e.size; ++i) {
        if (e.indexes[i] != -1){
          s[e.indexes[i]] += coef * e.gradient[i];
        }
      }
      if(e.next) {
        unpack(s, c, *(e.next));
      }
      if(e.nested) {
        unpack(s, coef, *(e.nested));
      }
    }

  } // namespace details

  template<typename T>
  static void unpack(Arcane::UniqueArray<T>& unpacked_derivatives, const sparse_expression<T>& e, const bool force=true)
  {
    if(!e.is_unpack() || force){
      unpacked_derivatives.fill(T(0));
      if(e.second){
        T* gradient = unpacked_derivatives.unguardedBasePointer();
        if(e.index_redir().empty())
          details::unpack(gradient, T(1), *(e.second));
        else
          details::unpack(gradient, e.index_redir(), T(1), *(e.second));
      }
      e.is_unpack(true);
    }
  }


  template<typename T>
  static scalar<T> unpack(const int size, const sparse_expression<T>& e)
  {
    scalar<T> s(size);
    s.value() = e.first;
    T* gradient = s.derivatives();
    if(e.second){
      if(e.index_redir().empty())
        details::unpack(gradient, T(1), *(e.second));
      else
        details::unpack(gradient, e.index_redir(), T(1), *(e.second));
    }
    return s;
  }

  // attention pas de correction
  // to delete
  template<typename T>
  static scalar<T> unpack(const int size, const scalar_wrapper<T>& v)
  {
    const sparse_expression<T>& e = v;
    return unpack(size, e);
  }

}

#endif /* __audi_sparse_functional_unpack_h__ */
