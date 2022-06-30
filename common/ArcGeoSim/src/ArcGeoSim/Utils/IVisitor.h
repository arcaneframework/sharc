// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef IVISITOR_H
#define IVISITOR_H

//BEGIN_NAMESPACE_PROJECT

/**
 * Interface des visitor
 */
class IVisitor
{
public:
  /** Constructeur de la classe */
  IVisitor() {}
  
  /** Destructeur de la classe */
  virtual ~IVisitor() {}
  
public:
  /** 
   *  Initialise 
   */
  virtual void init() = 0;
  
};


//END_NAMESPACE_PROJECT

#endif
