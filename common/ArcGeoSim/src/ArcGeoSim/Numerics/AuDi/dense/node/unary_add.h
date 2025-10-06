#ifndef __audi_dense_node_unary_add_h__
#define __audi_dense_node_unary_add_h__

namespace audi {

  template<typename T>
  struct unary_add_by_right
    : public expression<unary_add_by_right<T>>
    , private node
  {
    using Operand = typename std::conditional<is_node<T>::value, const T, const T&>::type;

    Operand m_type;

    using data_type = decltype(m_type.value());

    decltype(m_type.value()) m_factor;
    decltype(m_type.value()) m_v;

    unary_add_by_right(const expression<T>& t, decltype(m_type.value()) factor)
      : m_type(t.staticCast())
      , m_factor(factor)
      , m_v(m_type.value()) {}

    inline auto value() const
    {
      return m_v + m_factor;
    }

    inline auto gradient(std::size_t i) const
    {
      return m_type.gradient(i);
    }

    inline auto size() const { return m_type.size(); }
  };

  template<typename T>
  struct unary_add_by_left
    : public expression<unary_add_by_left<T>>
    , private node
  {
    using Operand = typename std::conditional<is_node<T>::value, const T, const T&>::type;

    Operand m_type;

    using data_type = decltype(m_type.value());

    decltype(m_type.value()) m_factor;
    decltype(m_type.value()) m_v;

    unary_add_by_left(const expression<T>& t, decltype(m_type.value()) factor)
    : m_type(t.staticCast())
    , m_factor(factor)
    , m_v(m_type.value()) {}

    inline auto value() const
    {
      return m_factor + m_v;
    }

    inline auto gradient(std::size_t i) const
    {
      return m_type.gradient(i);
    }

    inline auto size() const { return m_type.size(); }
  };

}

#endif /* __audi_dense_node_unary_add_h__ */
