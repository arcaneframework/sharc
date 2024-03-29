// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
/* Author : desrozis at Wed Oct 29 18:23:42 2014
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Time/ITimeStepComputer.h"
#include "ArcGeoSim/Time/Defaults/EvolutionTypes.h"
#include "ArcGeoSim/Time/TimeVariables.h"

#include "TimeStepEvolutionPolicy_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class TimeStepEvolutionPolicyService 
  : public ArcaneTimeStepEvolutionPolicyObject
  , public ArcGeoSim::TimeVariables
{
public:
  
  TimeStepEvolutionPolicyService(const Arcane::ServiceBuildInfo & sbi) 
    : ArcaneTimeStepEvolutionPolicyObject(sbi)
    , ArcGeoSim::TimeVariables(sbi.subDomain())
    , m_initialized(false)
    , m_type(ArcGeoSim::EvolutionTypes::Geometric)
    , m_increase_factor(-1)
    , m_decrease_factor(-1) {}
  
  virtual ~TimeStepEvolutionPolicyService() {}
  
public:

  void init();

  Arcane::Real nextTimeStep() const;

  Arcane::Real restoredTimeStep() const;

private:

  bool m_initialized;

  ArcGeoSim::EvolutionTypes::eEvolutionType m_type;

  Arcane::Real m_increase_factor;
  Arcane::Real m_decrease_factor;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
TimeStepEvolutionPolicyService::
init()
{
  if(m_initialized) return;

  m_type = options()->type();

  m_increase_factor = options()->increaseFactor();

  if(m_type == ArcGeoSim::EvolutionTypes::Geometric && m_increase_factor < 1.) {
    fatal() << "Geometric time step evolution : Increase factor must be greater than one";
  }

  m_decrease_factor = options()->decreaseFactor();

  if(m_type == ArcGeoSim::EvolutionTypes::Geometric && m_decrease_factor > 1.) {
    fatal() << "Geometric time step evolution : Decrease factor must be lesser than one";
  }

  m_initialized = true;
}

/*---------------------------------------------------------------------------*/

Arcane::Real
TimeStepEvolutionPolicyService::
nextTimeStep() const
{
  ARCANE_ASSERT((m_initialized),("TimeStepEvolutionPolicyService not initialized"));

  return (m_type == ArcGeoSim::EvolutionTypes::Geometric) ?
      m_dt() * m_increase_factor : m_dt() + m_increase_factor;
}

/*---------------------------------------------------------------------------*/

Arcane::Real
TimeStepEvolutionPolicyService::
restoredTimeStep() const
{
  ARCANE_ASSERT((m_initialized),("TimeStepEvolutionPolicyService not initialized"));

  return (m_type == ArcGeoSim::EvolutionTypes::Geometric) ?
      m_dt() * m_decrease_factor : m_dt() - m_decrease_factor;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_TIMESTEPEVOLUTIONPOLICY(TimeStepEvolutionPolicy,TimeStepEvolutionPolicyService);
// Pour portage
// A retirer quand les applications auront fait les changements
ARCANE_REGISTER_SERVICE_TIMESTEPEVOLUTIONPOLICY(TimeStepMng,TimeStepEvolutionPolicyService);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
