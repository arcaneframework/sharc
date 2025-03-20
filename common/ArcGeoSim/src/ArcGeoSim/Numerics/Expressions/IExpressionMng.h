// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_EXPRESSIONS_IEXPRESSIONMNG_H
#define ARCGEOSIM_EXPRESSIONS_IEXPRESSIONMNG_H

#include "IFunction.h"
#include <arcane/utils/UtilsTypes.h> // d�finition de Real et String

using namespace Arcane;

class IExpressionMng
{
public:
  /** Constructeur de la classe */
  IExpressionMng() 
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~IExpressionMng() { }
  
public:

  //! Initialisation
  virtual void init() = 0;

  //! Access to constante
  virtual Real constant(const String & name) const = 0;

  //! teste l'existence d'une constante de nom donn�
  virtual const Real * constantLookup(const String & name) const = 0;

  //! Access to function
  virtual IFunction * function(const String & name) const = 0;

  // M�thodes a ajouter

};

#endif /* ARCGEOSIM_EXPRESSIONS_IEXPRESSIONMNG_H */
