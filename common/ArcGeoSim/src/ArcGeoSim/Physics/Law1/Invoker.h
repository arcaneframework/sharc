// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_INVOKER_H
#define ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_INVOKER_H

/*
 * \ingroup Law
 * \brief Outil interne pour l'invocation de la signature d'une fonction
 *
 */

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/preprocessor/iteration/local.hpp>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace details {

  // Les VariableRefScalarT n'ont pas d'accesseur aux donn�es!!!
  // mais simplement l'op�rateur =
  template<typename T>
  inline T& ScalarInvokerTraits(Arcane::VariableRefScalarT<T>& v)
  {
    const T& t = v();
    return const_cast<T&>(t);
  };
  template<typename T>
  inline const T& ScalarInvokerTraits(const Arcane::VariableRefScalarT<T>& v)
  {
    return v();
  };
  template<typename T>
  inline Arcane::ArrayView<T>& ScalarInvokerTraits(Arcane::VariableRefArrayT<T>& v)
  {
    return v;
  };
  template<typename T>
  inline Arcane::ConstArrayView<T>& ScalarInvokerTraits(const Arcane::VariableRefArrayT<T>& v)
  {
    return v;
  };
  // Pour les states
  template<typename T>
  inline T& ScalarInvokerTraits(T& v)
  {
    return v;
  };
  template<typename T>
  inline T& ScalarInvokerTraits(const T& v)
  {
    return v;
  };

#define INVOKER_MAX_SIZE 30
#define BUILD(z,n,x) ScalarInvokerTraits(*boost::fusion::at_c<n>(x))
#define BUILDK(z,n,x) boost::fusion::at_c<n>(x)->operator[](k)

  template<typename T, typename V, Integer>
  struct Invoker;

#define INVOKER(z,n,V)                          \
  template<typename T, typename V>              \
  struct Invoker<T,V,n>                         \
  {                                             \
    Invoker(const T& algo, V& v)                \
      : m_algo(algo)                            \
      , m_v(v) {}                               \
    inline void operator()() {                  \
      m_algo.eval(BOOST_PP_ENUM(n,BUILD,m_v));  \
    }                                           \
    template<typename E>                        \
    inline void operator()(const E& k) {        \
      m_algo.eval(BOOST_PP_ENUM(n,BUILDK,m_v)); \
    }                                           \
    const T& m_algo;                            \
    V& m_v;                                     \
  };

  BOOST_PP_REPEAT(INVOKER_MAX_SIZE,INVOKER,V)
#undef INVOKER

  template<typename T, typename V, Integer>
  struct DerivativeInvoker;

#define INVOKER(z,n,V)                                  \
  template<typename T, typename V>                      \
  struct DerivativeInvoker<T,V,n>                       \
  {                                                     \
    DerivativeInvoker(const T& algo, V& v)              \
      : m_algo(algo)                                    \
      , m_v(v) {}                                       \
    inline void operator()() {                          \
      m_algo.eval(BOOST_PP_ENUM(n,BUILD,m_v));   \
    }                                                   \
    template<typename E>                                \
    inline void operator()(const E& k) {                \
      m_algo.eval(BOOST_PP_ENUM(n,BUILDK,m_v)); \
    }                                                   \
    const T& m_algo;                                    \
    V& m_v;                                             \
  };

  BOOST_PP_REPEAT(INVOKER_MAX_SIZE,INVOKER,V)

#undef INVOKER

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
 * \ingroup Law
 * \brief Invoker de la signature d'une fonction
 *
 * Contrat par polymorphisme statique : l'objet algoritme doit avoir une m�thode
 * evaluate() avec la signature des �l�ments du vecteur boost V
 *
 */
template<typename T, typename V>
struct Invoker
{  
  Invoker(const T& algo, V& v) 
    : m_invoker(algo,v) {}
  
  inline void operator()() {
    m_invoker();
  }
  
  template<typename E>
  inline void operator()(const E& i) {
    m_invoker(i);
  }
  
  details::Invoker<T,V,boost::fusion::result_of::size<V>::type::value> m_invoker;
};

/*
 * \ingroup Law
 * \brief Invoker de la signature d'une fonction
 *
 * Contrat par polymorphisme statique : l'objet algoritme doit avoir une m�thode
 * eval() avec la signature des �l�ments du vecteur boost V
 *
 */
template<typename T, typename V>
struct DerivativeInvoker
{  
  DerivativeInvoker(const T& algo, V& v) 
    : m_invoker(algo,v) {}
  
  inline void operator()() {
    m_invoker();
  }
  
  template<typename E>
  inline void operator()(const E& i) {
    m_invoker(i);
  }
  
  details::DerivativeInvoker<T,V,boost::fusion::result_of::size<V>::type::value> m_invoker;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_INVOKER_H */
