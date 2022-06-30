// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef TENSOR_ALGEBRA_H
#define TENSOR_ALGEBRA_H

#include <arcane/ArcaneTypes.h>
#include <arcane/MathUtils.h>
#include <arcane/VariableTypedef.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 106400
#include <boost/serialization/array_wrapper.hpp>
#endif
#include <boost/numeric/ublas/matrix.hpp>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace DiscreteOperator {

  using namespace Arcane;

  /*---------------------------------------------------------------------------*/

  struct i_tensor_vector_prod {
    struct Error {};    
  };

  template<typename T1>
  struct tensor_vector_prod 
    : public i_tensor_vector_prod {
    static Real3 eval (const T1& t, const Real3& v) {
      throw(i_tensor_vector_prod::Error());
    }
  };

  template<> struct tensor_vector_prod<Real> 
    : public i_tensor_vector_prod {
    static Real3 eval (const Real& t, const Real3& v) {
      return t * v;
    }
  };

  template<> struct tensor_vector_prod<Real3> 
    : public i_tensor_vector_prod {
    static Real3 eval (const Real3& t, const Real3& v) {
      return Real3(t.x * v.x,
                   t.y * v.y,
                   t.z * v.z);
    }
  };

  template<> struct tensor_vector_prod<Real3x3> 
    : public i_tensor_vector_prod {
    static Real3 eval (const Real3x3& t, const Real3& v) {
      return math::prodTensVec(t, v);
    }
  };

  /*---------------------------------------------------------------------------*/

  struct MinimumEigenvalue
  {
    static Real compute(const Real & A);
    static Real compute(const Real3 & A);
    static Real compute(const Real3x3 & A);
  };

  /*---------------------------------------------------------------------------*/

  template<Integer N>
  struct vector_outer_prod {
  };

  // By default we assume to be in the xz plane
  template<>
  struct vector_outer_prod<2> {
    static boost::numeric::ublas::matrix<Real> eval(const Real3 & v1, const Real3 & v2) {
      boost::numeric::ublas::matrix<Real> M = boost::numeric::ublas::zero_matrix<Real>(2, 2);
      M(0,0) = v1.y * v2.y;
      M(0,1) = v1.y * v2.z;
      M(1,0) = v1.z * v2.y;
      M(1,1) = v1.z * v2.z;
      return M;
    }
  };

  template<>
  struct vector_outer_prod<3> {
    static boost::numeric::ublas::matrix<Real> eval(const Real3 & v1, const Real3 & v2) {
      boost::numeric::ublas::matrix<Real> M = boost::numeric::ublas::zero_matrix<Real>(3, 3);
      M(0,0) = v1.x * v2.x;
      M(0,1) = v1.x * v2.y;
      M(0,2) = v1.x * v2.z;
      M(1,0) = v1.y * v2.x;
      M(1,1) = v1.y * v2.y;
      M(1,2) = v1.y * v2.z;
      M(2,0) = v1.z * v2.x;
      M(2,1) = v1.z * v2.y;
      M(2,2) = v1.z * v2.z;
      return M;
    }
  };

  /*---------------------------------------------------------------------------*/

  template<Integer N>
  struct realvector_to_real3 {
  };

  // By default we assume to be in the xz plane
  template<>
  struct realvector_to_real3<2> {
    static Real3 convert(const boost::numeric::ublas::vector<Real> & v)
    {
      Real3 _v;
      _v.x = 0.;
      _v.y = v(0);
      _v.z = v(1);
      return _v;
    }
  };

  template<>
  struct realvector_to_real3<3> {
    static Real3 convert(const boost::numeric::ublas::vector<Real> & v)
    {
      Real3 _v;
      _v.x = v(0);
      _v.y = v(1);
      _v.z = v(2);
      return _v;
    }
  };

  /*---------------------------------------------------------------------------*/

  template<Integer N>
  struct real3_to_realvector {
  };

  // By default we assume to be in the xz plane
  template<>
  struct real3_to_realvector<2> {
    static boost::numeric::ublas::vector<Real> convert(const Real3 & v) 
    {
      boost::numeric::ublas::vector<Real> _v(2);
      _v(0) = v.y;
      _v(1) = v.z;
      return _v;
    }
  };

  template<>
  struct real3_to_realvector<3> {
    static boost::numeric::ublas::vector<Real> convert(const Real3 & v) 
    {
      boost::numeric::ublas::vector<Real> _v(3);
      _v(0) = v.x;
      _v(1) = v.y;
      _v(2) = v.z;
      return _v;
    }
  };

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#endif
