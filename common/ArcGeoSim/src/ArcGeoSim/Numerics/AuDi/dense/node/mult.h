#ifndef __audi_dense_node_mult_h__
#define __audi_dense_node_mult_h__

namespace audi {

  template<typename L, typename R>
  struct mult
      : public expression<mult<L,R>>
      , private node
  {
    using Left  = typename std::conditional<is_node<L>::value, const L, const L&>::type;
    using Right = typename std::conditional<is_node<R>::value, const R, const R&>::type;

    Left  m_l;
    Right m_r;

    const decltype(m_l.value()) m_lv;
    const decltype(m_r.value()) m_rv;

    mult(const expression<L>& l, const expression<R>& r)
      : m_l(l.staticCast())
      , m_r(r.staticCast())
      , m_lv(m_l.value())
      , m_rv(m_r.value())
    {
      assert(m_l.size() == m_r.size());
    }

    inline auto value() const
    {
      return m_lv * m_rv;
    }

    inline auto gradient(std::size_t i) const
    {
      return m_l.gradient(i) * m_rv + m_lv * m_r.gradient(i);
    }

    inline auto size() const { return m_l.size(); }
  };

}

#endif /* __audi_dense_node_mult_h__ */
