// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_EXPRESSIONS_IDFUNCTIONR1VR1_H
#define ARCGEOSIM_EXPRESSIONS_IDFUNCTIONR1VR1_H

#include <arcane/ArcaneVersion.h>
#include "FunctionVars.h"

BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_NUMERICS_NAMESPACE
BEGIN_EXPRESSIONS_NAMESPACE

using namespace Arcane ;

class IDFunctionR1vR1 : public IFunctionR1vR1
{
public:
  /** Constructeur de la classe */
  IDFunctionR1vR1()
  {
    ;
  }

  /** Destructeur de la classe */
  virtual ~IDFunctionR1vR1() { }

public:

  //! Point-wise evaluation
  virtual DVar<Real> evalD(const Real & var0) = 0;

  //! Vector evaluation
  virtual void evalD(const Array<Real> & var0, Array< DVar<Real> >& res0) = 0;
};

END_EXPRESSIONS_NAMESPACE
END_NUMERICS_NAMESPACE
END_ARCGEOSIM_NAMESPACE
#endif /* ARCGEOSIM_EXPRESSIONS_IDFUNCTIONR1VR1_H */
