// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * IStopCriteriaEstimator.h
 *
 *  Created on: 28 janv. 2015
 *      Author: Soleiman YOUSEF
 */

#ifndef ISTOPCRITERIAESTIMATOR_H_
#define ISTOPCRITERIAESTIMATOR_H_

#ifdef USE_ALIEN_V0
#include "ALIEN/LinearAlgebra2.h"
#endif
#ifdef USE_ALIEN_V1
#include "ALIEN/Alien-IFPEN.h"
#endif

class IStopCriteriaEstimator
{
public :
#ifdef USE_ALIEN_V0
  typedef Alien::Vector VectorType ;
#endif
#ifdef USE_ALIEN_V1
  typedef Alien::Vector VectorType ;
#endif
  IStopCriteriaEstimator(){}
  virtual ~IStopCriteriaEstimator(){}
  //virtual bool computeLinearStopCriteria(VectorType& x, Real const& tolerance) = 0 ;
  virtual Real GetGlobalSpatialEstimator()= 0 ;
  virtual void GetLocalSpatialEstimator(Arcane::VariableCellReal& spatial_estimate)= 0 ;
//  virtual Real GetGlobalAlgebraicEstimator()= 0 ;
//  virtual void GetLocalAlgebraicEstimator(Arcane::VariableCellReal& algebraic_estimate)= 0 ;
//  virtual void GetEstimatorsToPrint(VectorType& sp, VectorType& alg, VectorType& not_sp, VectorType& not_alg)= 0 ;
//  virtual void copyBinVecB(const VectorType& b, VectorType& vec_b)= 0 ;
};

class IStopCriteriaEstimatorModel
{
public :
  IStopCriteriaEstimatorModel(){}
  virtual ~IStopCriteriaEstimatorModel(){}
  virtual void setStopCriteriaEstimator(IStopCriteriaEstimator* value) = 0 ;
};


#endif /* ISTOPCRITERIAESTIMATOR_H_ */
