// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_EXPRESSIONS_IFUNCTIONR2VR1_H
#define ARCGEOSIM_EXPRESSIONS_IFUNCTIONR2VR1_H

/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------- 
 <autogenerated>
   GENERATED FILE : DO NOT MODIFY THIS FILE
   This code was generated by a tool. 
   Changes to this file may cause incorrect behavior and will be lost if 
   the code is regenerated.
   Date of generation : 
   Generator : common/ArcanInfra/tools/ArcaneInfra.Tools/ExpressionBuilderGenerator
   With options : --accumulation 4 --interface --n=4 --m=1 --path .
</autogenerated>
  ---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/ArcaneVersion.h>

#include "IIFunction.h"

#include <arcane/utils/UtilsTypes.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IFunctionR2vR1 : public IIFunction
{
public:
  
  IFunctionR2vR1() {}
  
  virtual ~IFunctionR2vR1() {}
  
public:

  //! Getting dimension of in-space
  Integer getInDimension() const { return 2; }
  
  //! Getting dimension of out-space
  Integer getOutDimension() const { return 1; }

  //! Point-wise evaluation
  /*! An optimized syntax will be introduce for single return evaluation 
   */
  virtual void eval(const Arcane::Real& var0,
                    const Arcane::Real& var1,
                          Arcane::Real& res0) = 0;

  //! Vector evaluation
  virtual void eval(const Arcane::Array<Arcane::Real>& var0,
                    const Arcane::Array<Arcane::Real>& var1,
                          Arcane::Array<Arcane::Real>& res0) = 0;
  
  //! Vector evaluation
  virtual void eval(Arcane::RealConstArrayView var0,
                    Arcane::RealConstArrayView var1,
                         Arcane::RealArrayView res0) = 0;

  //! Coupled function and derivative scalar evaluation (temporary: derivation handling in IFunction still to be defined)
  virtual void eval(const Arcane::Real& var0,
                    const Arcane::Real& var1,
                          Arcane::Real& res0,
                          Arcane::Real& diffres0,
                          Arcane::Real& diffres1) = 0;
  
  //! Scalar return for point-wise evaluation
  virtual Arcane::Real eval(const Arcane::Real& var0,
                            const Arcane::Real& var1) = 0;

  //! Eval vectorized data service function
  virtual void eval() = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_EXPRESSIONS_IFUNCTIONR2VR1_H */

