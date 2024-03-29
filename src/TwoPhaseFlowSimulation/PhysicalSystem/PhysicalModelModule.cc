// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
/* Author : desrozis at Wed Jan 14 08:34:58 2015
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ISystemBuilder.h"

#include "PhysicalModel_axl.h"

#include "IPhysicalSystem.h"
#include "IVariableManager.h"

#include "ArcGeoSim/Appli/AppService.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class PhysicalModelModule
  : public ArcanePhysicalModelObject
  , private IPhysicalSystem
  , private IVariableManager
{
public:
 
  PhysicalModelModule(const Arcane::ModuleBuildInfo& mbi)
    : ArcanePhysicalModelObject(mbi)
    , m_initialized(false)
    , m_domain("Domain", mesh())
    , m_domain_tn("DomainTN", mesh())
    , m_domain_t0("DomainT0", mesh()) {}

  ~PhysicalModelModule() {}
  
public:
  
  void build();

  void init();

  // Destruction du modele Gump
  void exit();

  /** Retourne le numero de version du module */
  Arcane::VersionInfo versionInfo() const { return Arcane::VersionInfo(1,0,0); }
  
  //! Retourne le systeme physique (retour const impossible 
  //  car necessite d'acces au conteneur des proprietes)
  ArcRes::System system() const;

  //! Retourne le gestionnaire des variables
  Law::VariableCellFolder& domain() { return m_domain; }
  Law::VariableCellFolder& domainTn() { return m_domain_tn; }
  Law::VariableCellFolder& domainT0() { return m_domain_t0; }

private:

  void _fatalIfNotInitialized() const;

private:

  bool m_initialized;

  //! Systeme
  ArcRes::System m_system;

  //! Manager des conteneurs de lois
  Law::VariableCellFolder m_domain;
  Law::VariableCellFolder m_domain_tn;
  Law::VariableCellFolder m_domain_t0;

};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
PhysicalModelModule::
_fatalIfNotInitialized() const
{
  if(not m_initialized)
    fatal() << "Data model manager is not initialized, call init method before";
}

/*---------------------------------------------------------------------------*/

void 
PhysicalModelModule::
build()
{
  ArcGeoSim::MutableAppService<IPhysicalSystem>() = this;
  ArcGeoSim::MutableAppService<IVariableManager>() = this;
}

/*---------------------------------------------------------------------------*/

void
PhysicalModelModule::
init()
{
  if(m_initialized) return;

  // Construction du systeme
  if(options()->system.size() == 1)
    m_system = options()->system[0]->buildSystem();

  auto active_cells_lids = mesh()->allActiveCells().view().localIds();
  Arcane::CellGroup domain_cells = mesh()->cellFamily()->createGroup("DomainCells", active_cells_lids);

  // Gestionnaire des variables
  m_domain.setSupport(domain_cells);
  m_domain_tn.setSupport(domain_cells);
  m_domain_t0.setSupport(domain_cells);

  m_initialized = true;
}

/*---------------------------------------------------------------------------*/

ArcRes::System
PhysicalModelModule::
system() const
{
  _fatalIfNotInitialized();

  return m_system;
}

/*---------------------------------------------------------------------------*/

void
PhysicalModelModule::
exit()
{
  _fatalIfNotInitialized();

  info() << "Destroy gump model...";

  m_system.kill();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_MODULE_PHYSICALMODEL(PhysicalModelModule);
