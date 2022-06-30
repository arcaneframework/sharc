// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_EXPRESSIONS_IWEXPRESSIONMNG_H
#define ARCGEOSIM_EXPRESSIONS_IWEXPRESSIONMNG_H
/* Author : faneyt at Wed Apr 02 15:00:00 2014
 * Generated by createNew
 */

#include "IExpressionMng.h"

using namespace Arcane;

class IWExpressionMng :
		public IExpressionMng
{
public:
  /** Constructeur de la classe */
  IWExpressionMng()
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~IWExpressionMng() { }
  
public:

  //! Initialisation
  virtual void init() = 0;

  //! Access to constante
  virtual Real constant(const String & name) const = 0;

  //! teste l'existence d'une constante de nom donne
  virtual const Real * constantLookup(const String & name) const = 0;

  //! Access to function
  virtual IFunction * function(const String & name) const = 0;

  //! Add constant
  virtual void setConstant(const String & name, const Real value) = 0;

  //! Add function
  virtual void setFunction(const String & name, IFunction * value) = 0;

  // Methodes a ajouter

};

#endif /* ARCGEOSIM_EXPRESSIONS_IWEXPRESSIONMNG_H */
