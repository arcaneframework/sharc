// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_MODULEMASTER_H
#define ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_MODULEMASTER_H


// Temporary fix removing deep ArcGeoSim dependencies
// #include "ArcGeoSim/Utils/ArcGeoSim.h"
#define BEGIN_ARCGEOSIM_NAMESPACE namespace ArcGeoSim {
#define END_ARCGEOSIM_NAMESPACE }

#include <arcane/ModuleMaster.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class BackwardManager;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ModuleMaster
  : public Arcane::ModuleMaster
{
public:
  
  ModuleMaster(const Arcane::ModuleBuildInfo& mbi);
  
  virtual ~ModuleMaster();
 
private:

  //! Modifications li�es au temps
  // 
  // global_time += global_deltat
  //
  // On souhaite en avoir la ma�trise pour �viter
  // les erreurs num�riques pour les �v�nements
  void timeIncrementation();

  //! Affichage de l'information en temps
  void timeStepInformation();

  //! Points d'entr�es AutoLoadBegin
  void _arcgeosimFirstBuild();
  void _arcgeosimFirstStartInit();
  void _arcgeosimFirstContinueInit();
  void _arcgeosimFirstInit();
  void _arcgeosimFirstComputeLoop();
  void _arcgeosimFirstOnMeshChanged();
  void _arcgeosimFirstRestore();
  void _arcgeosimFirstExit();
  
  //! Points d'entr�es AutoLoadEnd
  void _arcgeosimLastBuild();
  void _arcgeosimLastStartInit();
  void _arcgeosimLastContinueInit();
  void _arcgeosimLastInit();
  void _arcgeosimLastComputeLoop();
  void _arcgeosimLastOnMeshChanged();
  void _arcgeosimLastRestore();
  void _arcgeosimLastExit();
  
private:

  //! Gestionnaire de retour-arri�re
  BackwardManager* m_backward_mng;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_MODULEMASTER_H */
