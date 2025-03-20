#ifndef __audi_dense_node_inverse_h__
#define __audi_dense_node_inverse_h__

namespace audi {

  template<typename T>
  struct inverse
      : public expression<inverse<T>>
      , private node
  {
    using Operand = typename std::conditional<is_node<T>::value, const T, const T&>::type;

    Operand m_type;

    using data_type = decltype(m_type.value());

    const decltype(m_type.value()) m_factor;
    const decltype(m_type.value()) m_v;

    inverse(const expression<T>& t, decltype(m_type.value()) factor)
      : m_type(t.staticCast())
      , m_factor(factor)
      , m_v(m_type.value()) {}

    inline auto value() const
    {
      return m_factor / m_v;
    }

    inline auto gradient(std::size_t i) const
    {
      return - m_factor * m_type.gradient(i) / (m_v*m_v);
    }

    inline auto size() const { return m_type.size(); }
  };

}

#endif /* __audi_dense_node_inverse_h__ */
