﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef DEMONSTRATOR_DEMONSTRATOR_DEMONSTRATORMODULE_H
#define DEMONSTRATOR_DEMONSTRATOR_DEMONSTRATORMODULE_H
/* Author : havep at Thu Apr  2 10:29:37 2009
 * Generated by createNew
 */

#include <arcane/IPostProcessorWriter.h>
#include <arcane/IInitialPartitioner.h>
#include <arcane/IMeshPartitioner.h>
#include <arcane/Directory.h>

#include "ArcGeoSim/Tests/IServiceValidator.h"
#include "Demonstrator_axl.h"

#include "ArcGeoSim/Mesh/Exporters/ArcGeoSimPostProcessingBase.h"
#include "ArcGeoSim/Appli/Architecture/Delegate.h"
#include "ArcGeoSim/Appli/Architecture/IIterationInformation.h"

using namespace Arcane;

class DemonstratorModule
  : public ArcaneDemonstratorObject
  , public IAppServiceMng
  , public ArcGeoSim::Delegate<ArcGeoSim::IIterationInformation>
{
public:

  DemonstratorModule(const Arcane::ModuleBuildInfo& mbi);

  virtual ~DemonstratorModule();

protected:

  void initializeAppServiceMng();

public:

  void build();
  void prepareInit();
  void init();
  void continueInit();
  void endInit();
  void beginTimeStep();
  void endTimeStep();
  void endSimulation();

  /** Retourne le numero de version du module */
  virtual Arcane::VersionInfo versionInfo() const { return Arcane::VersionInfo(1,0,0); }

private:

  void postProcessingStartInit();
  void postProcessingInit();
  void postProcessingExit();
  bool _hasPostProcessing() const;
  void _validate();
  void updateGeometry();

  //! Affichage de l'information en temps
  void timeStepInformation(Arcane::Integer nb_loop);

private:

  bool m_initialized;
  bool m_forecast_compute;
  Integer m_max_iteration;
  IGeometryMng * m_geometry_mng;
  IGeometryPolicy * m_geometry_policy;
  ArcGeoSimPostProcessingBase m_post_processing;
};

#endif /* DEMONSTRATOR_DEMONSTRATOR_DEMONSTRATORMODULE_H */