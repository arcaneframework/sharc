// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_MPL_H
#define ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_MPL_H

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include <boost/mpl/vector.hpp>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace mpl {
  
  namespace details {
  
    // For statique
    
    template<int Begin, int End>
    struct static_for
    {
      template<typename Op>
      void operator()(const Op& oper)
      {
        oper.template apply<Begin>();
        static_for<Begin + 1, End>()(oper);
      }
      
      template<typename Op, typename T>
      void operator()(const Op& oper, T& arg)
      {
        oper.template apply<Begin>(arg);
        static_for<Begin + 1, End>()(oper,arg);
      }
      
      template<typename Op, typename T1, typename T2>
      void operator()(const Op& oper, T1& arg1, T2& arg2)
      {
        oper.template apply<Begin>(arg1,arg2);
        static_for<Begin + 1, End>()(oper,arg1,arg2);
      }
      
      template<typename Op, typename T1, typename T2, typename T3>
      void operator()(const Op& oper, T1& arg1, T2& arg2, T3& arg3)
      {
        oper.template apply<Begin>(arg1,arg2,arg3);
        static_for<Begin + 1, End>()(oper,arg1,arg2,arg3);
      }

      template<typename Op, typename T1, typename T2, typename T3, typename T4>
      void operator()(const Op& oper, T1& arg1, T2& arg2, T3& arg3, T4& arg4)
      {
        oper.template apply<Begin>(arg1,arg2,arg3,arg4);
        static_for<Begin + 1, End>()(oper,arg1,arg2,arg3,arg4);
      }

      template<typename Op, typename T1, typename T2, typename T3, typename T4, typename T5>
      void operator()(const Op& oper, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5)
      {
        oper.template apply<Begin>(arg1,arg2,arg3,arg4,arg5);
        static_for<Begin + 1, End>()(oper,arg1,arg2,arg3,arg4,arg5);
      }

      template<typename Op, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
      void operator()(const Op& oper, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6)
      {
    	  oper.template apply<Begin>(arg1,arg2,arg3,arg4,arg5,arg6);
    	  static_for<Begin + 1, End>()(oper,arg1,arg2,arg3,arg4,arg5,arg6);
      }
    };
    
    template<int N>
    struct static_for<N, N> 
    {
      template<typename Op>
      void operator()(const Op&) {}
      
      template<typename Op, typename T>
      void operator()(const Op&, T&) {}
      
      template<typename Op, typename T1, typename T2>
      void operator()(const Op&, T1&, T2&) {}
      
      template<typename Op, typename T1, typename T2, typename T3>
      void operator()(const Op&, T1&, T2&, T3&) {}

      template<typename Op, typename T1, typename T2, typename T3, typename T4>
      void operator()(const Op&, T1&, T2&, T3&, T4&) {}

      template<typename Op, typename T1, typename T2, typename T3, typename T4, typename T5>
      void operator()(const Op&, T1&, T2&, T3&, T4&, T5&) {}

      template<typename Op, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
      void operator()(const Op&, T1&, T2&, T3&, T4&, T5&, T6&) {}
   };
    
  } // details
  
  template<int Debut, int Fin, class Op>
  inline void static_for(const Op& oper) { return details::static_for<Debut, Fin>()(oper); }
  
  template<int Debut, int Fin, class Op, typename T>
  inline void static_for(const Op& oper, T& t) { return details::static_for<Debut, Fin>()(oper,t); }
  
  template<int Debut, int Fin, class Op, typename T1, typename T2>
  inline void static_for(const Op& oper, T1& t1, T2& t2) { return details::static_for<Debut, Fin>()(oper,t1,t2); }
  
  template<int Debut, int Fin, class Op, typename T1, typename T2, typename T3>
  inline void static_for(const Op& oper, T1& t1, T2& t2, T3& t3) { return details::static_for<Debut, Fin>()(oper,t1,t2,t3); }
  
  template<int Debut, int Fin, class Op, typename T1, typename T2, typename T3, typename T4>
  inline void static_for(const Op& oper, T1& t1, T2& t2, T3& t3, T4& t4) { return details::static_for<Debut, Fin>()(oper,t1,t2,t3,t4); }
  
  template<int Debut, int Fin, class Op, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline void static_for(const Op& oper, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5) { return details::static_for<Debut, Fin>()(oper,t1,t2,t3,t4,t5); }

  template<int Debut, int Fin, class Op, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  inline void static_for(const Op& oper, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6) { return details::static_for<Debut, Fin>()(oper,t1,t2,t3,t4,t5,t6); }

  namespace details {
    
    // Merge altern�
    
    template<typename T, typename V, typename State, int Current, int Size>
    struct merge
    {
      typedef boost::mpl::int_<Current> _Current;
      typedef typename boost::mpl::at<T,_Current>::type type_1;
      typedef typename boost::mpl::at<V,_Current>::type type_2;
      
      typedef typename boost::mpl::push_back<State,         type_1>::type partial_insert;
      typedef typename boost::mpl::push_back<partial_insert,type_2>::type insert;
      
      typedef typename merge<T,V,insert,Current+1,Size>::type type;
    };
    
    template<typename T, typename V, typename State, int Size>
    struct merge<T,V,State,Size,Size>
    {
      typedef State type;
    };
    
  } // details
  
  template<typename T, typename V>
  struct merge
  {
    typedef typename details::merge< T, V, boost::mpl::vector<>, 0, boost::mpl::size<T>::value >::type type;
  };
  
} // mpl

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_MPL_H */
