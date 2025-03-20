// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_EXPRESSIONS_EXPRESSIONMNGSERVICE_H
#define ARCGEOSIM_EXPRESSIONS_EXPRESSIONMNGSERVICE_H

#include "IExpressionMng.h"

namespace Arcane { }
using namespace Arcane;

#include "ExpressionMng_axl.h"

#include <map>

class ExpressionMngService :
  public ArcaneExpressionMngObject
{
public:
  /** Constructeur de la classe */
  ExpressionMngService(const Arcane::ServiceBuildInfo & sbi) : 
    ArcaneExpressionMngObject(sbi) 
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~ExpressionMngService() {}
  
public:

  //! Initialisation
  void init();
 
  //! Access to constante
  Real constant(const String & name) const;

  //! teste l'existence d'une constante de nom donn�
  const Real * constantLookup(const String & name) const;

  //! Access to function
  IFunction * function(const String & name) const;

private:
  typedef std::map<String,Real> ConstantTable;
  ConstantTable m_constant_table;

  typedef std::map<String,IFunction*> FunctionTable;
  FunctionTable m_function_table;
};

#endif /* ARCGEOSIM_EXPRESSIONS_EXPRESSIONMNGSERVICE_H */
