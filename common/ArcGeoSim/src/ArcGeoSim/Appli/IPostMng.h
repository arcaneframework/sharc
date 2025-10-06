// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef IPOSTMNG_H
#define IPOSTMNG_H

#include "ArcGeoSim/Physics/Units/IUnitsSystem.h"

#include <arcane/ItemGroup.h>
#include <arcane/VariableTypedef.h>

class IInfoModel;

class IPostMng
{
public:
  IPostMng() {}
  
  virtual ~IPostMng() {};

public:
  
  //! Initialise
  virtual void init() = 0 ;

  //!update manager
  virtual void update() = 0 ;

  //! Start calculations
  virtual void start() = 0 ;

  //! set if to be updated ;
  virtual void setUpdated(bool is_updated) = 0 ;

  virtual Integer outputLevel() = 0 ;
  virtual Integer debugLevel() = 0 ;
  virtual const Array<CellGroup>& getCellGroupForPost() = 0 ;
  virtual const Array<CellGroup>& getCellGroupForDebug() = 0 ;
  virtual const DualNodeGroup& getDualNodeGroupForDebug() const = 0 ;
  virtual bool hasDualNodeForDebug(const DualNode& dnode) const = 0 ;
  virtual bool hasDualNodeForDebug(const DualNode& dnode) = 0 ;
  virtual void printVars() = 0 ;
  virtual void printVars(const String& var_name, VariableCellReal& var, IUnitsSystem::UnitType unit) = 0 ;
  virtual void printVars(const String& var_name, VariableCellArrayReal& var, IUnitsSystem::UnitType unit) = 0 ;

  virtual void addInfoModel(IInfoModel* model) = 0 ;
  virtual void printInfoModels() = 0 ;
  virtual void printCurrentTimeInfoModels() = 0 ;
};

#endif
