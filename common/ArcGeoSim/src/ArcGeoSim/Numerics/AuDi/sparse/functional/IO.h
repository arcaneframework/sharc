#ifndef __audi_sparse_functional_io_h__
#define __audi_sparse_functional_io_h__

#include <iostream>

#include "ArcGeoSim/Numerics/AuDi/sparse/scalar_wrapper.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/expression.h"

namespace audi {
template<typename T>
std::ostream& operator<<(std::ostream& nout, const audi::scalar_wrapper<T>& s)
{
  auto grad = s.gradient();
  auto idx = s.indexes();
  nout << "value={ " << s.value() << " }, derivatives={ ";
  for (auto i = 0; i < grad.size(); ++i) {
    nout << grad[i] << " ";
  }
  nout << "}, indexes={ ";
  for (auto i = 0; i < idx.size(); ++i) {
    nout << idx[i] << " ";
  }
  nout << "}";
  return nout;
}

template<typename T>
std::ostream& operator<<(std::ostream& nout, const audi::details::node<T>& s)
{
  nout << "coef={ " << s.coef << " },";
  //if (s.size) {
    nout << "\n[scalar] derivatives={ ";
    for (auto i = 0; i < s.size; ++i) {
      nout << s.gradient[i] << " ";
    }
    nout << "}, indexes={ ";
    for (auto i = 0; i < s.size; ++i) {
      nout << s.indexes[i] << " ";
    }
    nout << "}";
  //}
  if (s.nested) {
    nout << "\n[nested] " << *(s.nested);
  }
  if (s.next) {
    nout << "\n[next] " << *(s.next);
  }
  return nout;
}

template<typename T>
std::ostream& operator<<(std::ostream& nout, const audi::sparse_expression<T>&e )
{
  nout << "value={ " << e.first << " }, node={ ";
  if (e.second) {
    nout << *e.second;
  }
  nout << "\n}";
  return nout;
}
}

#endif /* __audi_sparse_functional_io_h__ */
