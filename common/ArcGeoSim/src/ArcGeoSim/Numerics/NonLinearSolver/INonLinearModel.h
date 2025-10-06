// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef INONLINEARMODEL_H
#define INONLINEARMODEL_H
/* Author : gratienj at Mon Aug 25 2008
 * Interface for NonLinearModel
 * This interface is design to implement Non linear Numerical models which are based on
 * a non linear system that can be solved by an iterative non linear solver
 */

class INonLinearModel
{
public:
  //!default destructor
  virtual ~INonLinearModel() { }

  //! methods to call specific actions to start non linear steps
  virtual void startNewtonStep() = 0 ;

  //! methods to call specific actions to end non linear steps
  virtual void endNewtonStep() = 0 ;
};

class IIterationNonLinearModel
{
public :
  IIterationNonLinearModel(){}
  virtual ~IIterationNonLinearModel() {}
  virtual void setCurrentNonLinearIteration(Integer iteration) = 0 ;
};

class INonLinearSystem ;
class INonLinearProblem
{
public:
  //!default destructor
  virtual ~INonLinearProblem() { }

  //! NonLinearSystem accessor
  virtual INonLinearSystem* getNonLinearSystem() { return NULL ; }
} ;

#endif
