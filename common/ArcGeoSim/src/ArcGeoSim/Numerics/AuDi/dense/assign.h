#ifndef __audi_dense_assign_h__
#define __audi_dense_assign_h__

namespace audi {

  template<typename U, typename V>
  inline void assign(U& u, const V& v)
  {
    u.resize(v.size());
    for(auto i = 0u; i < u.size(); ++i) {
      u.gradient(i) = v.gradient(i);
    }
    u.value() = v.value();
  }

  template<typename U, typename V>
  inline void addAssign(U& u, const V& v)
  {
    assert(u.size() == v.size());
    for(auto i = 0u; i < u.size(); ++i) {
      u.gradient(i) += v.gradient(i);
    }
    u.value() += v.value();
  }

  template<typename U, typename V>
  inline void subAssign(U& u, const V& v)
  {
    assert(u.size() == v.size());
    for(auto i = 0u; i < u.size(); ++i) {
      u.gradient(i) -= v.gradient(i);
    }
    u.value() -= v.value();
  }

  template<typename U, typename V>
  inline void multAssign(U& u, const V& v)
  {
    assert(u.size() == v.size());
    const auto _v = v.value();
    const auto _u = u.value();
    for(auto i = 0u; i < u.size(); ++i) {
      u.gradient(i) = u.gradient(i) * _v + _u * v.gradient(i);
    }
    u.value() = _u * _v;
  }

}

#endif /* __audi_dense_assign_h__ */
