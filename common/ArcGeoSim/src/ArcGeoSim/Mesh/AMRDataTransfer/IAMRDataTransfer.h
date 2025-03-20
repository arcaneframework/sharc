// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_AMRDATATRANSFER_IAMRDATATRANSFER_H_
#define ARCGEOSIM_MESH_AMRDATATRANSFER_IAMRDATATRANSFER_H_

#include "arcane/ArcaneTypes.h"

using namespace Arcane;

#include "TypesInterpolator.h"

//! interface de transport de donn�es entre maillages
class IAMRDataTransfer {

public:
  //! constructeur de la classe
  IAMRDataTransfer() {};

  //! destructeur de la classe
  virtual ~IAMRDataTransfer() {};

public:
  //! initialisation
  virtual void init() = 0;
  //! ajout d'une propriete scalaire a transferer
  virtual void addDataToRegister(String name, VariableFaceReal & data) = 0;
  virtual void addDataToRegister(String name, VariableFaceInteger & data) = 0;
  virtual void addDataToRegister(String name, VariableFaceInt64 & data) = 0;
  virtual void addDataToRegister(String name, VariableCellReal & data) = 0;
  virtual void addDataToRegister(String name, VariableCellReal & data, VariableParticleReal & particle_data) = 0;
  virtual void addDataToRegister(String name, VariableCellInteger & data) = 0;
  //! ajout d'une propriete vectorielle a transferer
  virtual void addDataToRegister(String name, VariableCellReal3 & data) = 0;
  virtual void addDataToRegister(String name, VariableCellArrayReal3 & data) = 0;
  //! ajout d'une propriete tensorielle a transferer
  virtual void addDataToRegister(String name, VariableCellReal3x3 & data) = 0;
  //! ajout d'une propriete tensorielle a transferer
  virtual void addExtensiveDataToRegister(String name, VariableCellArrayReal & data) = 0;
  virtual void addDataToRegister(String name, VariableCellArrayReal & data) = 0;
  virtual void addDataToRegister(String name, VariableFaceArrayReal & data) = 0;
  //! enregistrer la fonction de transport donnee par l'utilisateur
  virtual void registerTransportFunctor() = 0;

  virtual bool contains(Arcane::String name) const = 0;
};

#endif /* ARCGEOSIM_MESH_AMRDATATRANSFER_IAMRDATATRANSFER_H_ */
