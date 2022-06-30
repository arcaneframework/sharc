// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef __audi_dense_node_unary_sub_h__
#define __audi_dense_node_unary_sub_h__

namespace audi {

  template<typename T>
  struct unary_sub_by_right
    : public expression<unary_sub_by_right<T>>
    , private node
  {
    using Operand = typename std::conditional<is_node<T>::value, const T, const T&>::type;

    Operand m_type;

    using data_type = decltype(m_type.value());

    decltype(m_type.value()) m_factor;
    decltype(m_type.value()) m_v;

    unary_sub_by_right(const expression<T>& t, decltype(m_type.value()) factor)
      : m_type(t.staticCast())
      , m_factor(factor)
      , m_v(m_type.value()) {}

    inline auto value() const
    {
      return m_v - m_factor;
    }

    inline auto gradient(std::size_t i) const
    {
      return m_type.gradient(i);
    }

    inline auto size() const { return m_type.size(); }
  };

  template<typename T>
  struct unary_sub_by_left
    : public expression<unary_sub_by_left<T>>
    , private node
  {
    using Operand = typename std::conditional<is_node<T>::value, const T, const T&>::type;

    Operand m_type;

    using data_type = decltype(m_type.value());

    decltype(m_type.value()) m_factor;
    decltype(m_type.value()) m_v;

    unary_sub_by_left(const expression<T>& t, decltype(m_type.value()) factor)
      : m_type(t.staticCast())
      , m_factor(factor)
      , m_v(m_type.value()) {}

    inline auto value() const
    {
      return m_factor - m_v;
    }

    inline auto gradient(std::size_t i) const
    {
      return - m_type.gradient(i);
    }

    inline auto size() const { return m_type.size(); }
  };

}

#endif /* __audi_dense_node_unary_sub_h__ */
