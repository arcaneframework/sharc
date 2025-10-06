#ifndef __audi_dense_node_math_h__
#define __audi_dense_node_math_h__

#include <cmath>

namespace audi {

  template<typename T, typename M>
  struct math
    : public expression<math<T,M>>
    , private node
  {
    using Type = typename std::conditional<is_node<T>::value, const T, const T&>::type;

    Type m_type;

    const M m_math;

    using data_type = decltype(m_type.value());

    const decltype(m_type.value()) m_v;

    math(const expression<T>& type, M math)
      : m_type(type.staticCast())
      , m_math(math)
      , m_v(m_type.value()) {}

    inline auto value() const
    {
      return m_math(m_v);
    }

    inline auto gradient(std::size_t i) const
    {
      return m_math(m_v, m_type.gradient(i));
    }

    inline auto size() const { return m_type.size(); }
  };

  struct Sqrt
  {
    template<typename T>
    inline T operator()(T value) const
    {
      return std::sqrt(value);
    }

    template<typename T>
    inline T operator()(T value, T gradient) const
    {
      return gradient / (2 * std::sqrt(value));
    }
  };

}

#endif /* __audi_dense_node_math_h__ */
