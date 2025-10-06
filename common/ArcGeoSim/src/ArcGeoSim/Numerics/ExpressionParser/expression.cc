// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "expression.h"

namespace expression_parser {

  ////////////////////////////////////////////////////////////
  // Operators

  REGISTER_BINARY_OPERATOR(operator+, Addition);
  REGISTER_BINARY_OPERATOR(operator-, Subtraction);
  REGISTER_BINARY_OPERATOR(operator*, Multiplication);
  REGISTER_BINARY_OPERATOR(operator/, Division);
  REGISTER_BINARY_OPERATOR(_power, Power);

  ////////////////////////////////////////////////////////////
  // Functions

  REGISTER_FUNCTION(fabs, AbsoluteValue);
  REGISTER_FUNCTION(sqrt, SquareRoot);
  REGISTER_FUNCTION(sgn, Sign);
  REGISTER_FUNCTION(h, Heavyside);
#ifndef WIN32
  REGISTER_FUNCTION(erf, ErrorFunction);
  REGISTER_FUNCTION(erfc, ComplementaryErrorFunction);
#endif
  REGISTER_FUNCTION(sin, Sinus);
  REGISTER_FUNCTION(cos, Cosinus);
  REGISTER_FUNCTION(log, Logarithm);
  REGISTER_FUNCTION(exp, Exponential);
  
}
