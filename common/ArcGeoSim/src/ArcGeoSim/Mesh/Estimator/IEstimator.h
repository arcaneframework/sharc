// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * IEstimateur.h
 *
 *  Created on: Jun 28, 2011
 *      Author: zerguinf
 */

#ifndef ARCGEOSIM_MESH_ESTIMATOR_IESTIMATOR_H_
#define ARCGEOSIM_MESH_ESTIMATOR_IESTIMATOR_H_

#include <arcane/ArcaneTypes.h>
#include <arcane/VariableTypedef.h>

class INumericalModelVisitor;

//! interface pour estimateurs d'erreurs
class IEstimator
{

public:
  //! constructeur de la classe
  IEstimator()
  {};

  //! destructeur de la classe
  virtual ~IEstimator()
  {};
  virtual void init() = 0 ;
  virtual void init(Arcane::IMesh* mesh) = 0 ;

  virtual INumericalModelVisitor* getNumericalModelOp() {
    return NULL ;
  }

  virtual bool isNormalized() const {
    return false ;
  }
  //virtual void compute(VariableCellReal & error,IMesh *mesh) = 0;


public:
  //! calcul de la derivee
  virtual void computeDerivative(const Arcane::VariableCellReal& variable,
                                 const Arcane::VariableCellReal& variable_tn,
                                 Arcane::Real dt,
                                 Arcane::VariableCellReal& variable_result,
                                 Arcane::IMesh *mesh) = 0;
 /*
  //! calcul de la derivee spatiale d'ordre un
  virtual void firstOrderSpatialDerivative(const  VariableCellReal& variable,VariableCellReal & variable_in_space,IMesh *mesh) = 0;
  //! calcul de la derivee temporelle d'ordre un
  virtual void firstOrderTimeDerivative(const VariableCellReal& variable,const VariableCellReal& variable_tn,Real dt,VariableCellReal & variable_dt,IMesh *mesh)=0;
  //! calcul de la derviee mixte d'ordre deux
  virtual void mixedDerivative(const VariableCellReal& variable,const VariableCellReal& variable_tn,Real dt,VariableCellReal & variable_mixed,IMesh *mesh)=0;
  //! calcul de la norme
  virtual Real normL2(IMesh *mesh,const VariableCellReal & estimateur)=0;*/

  virtual void update(Arcane::IMesh* mesh) {}
};

#endif /* ARCGEOSIM_MESH_ESTIMATOR_IESTIMATEUR_H_ */
