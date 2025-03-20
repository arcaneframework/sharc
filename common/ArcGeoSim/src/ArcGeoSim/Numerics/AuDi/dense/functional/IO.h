#ifndef __audi_dense_functional_io_h__
#define __audi_dense_functional_io_h__

#include "ArcGeoSim/Numerics/AuDi/dense/expression.h"

#include <iostream>

namespace audi {

  template<typename T>
  inline std::ostream& operator<<(std::ostream& out, const expression<T>& n)
  {
    const auto& t = n.staticCast();
    out << "value={ " << t.value() << " }, derivatives["
        << t.size() << "]={ ";
    for(auto i = 0u; i < t.size(); ++i) {
      out << t.gradient(i) << " ";
    }
    out << "}";
    return out;
  }

}

#endif /* __audi_dense_functional_io_h__ */
