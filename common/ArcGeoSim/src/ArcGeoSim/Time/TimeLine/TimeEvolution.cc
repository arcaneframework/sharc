// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "TimeEvolution.h"
/* Author : desrozis at Wed Oct 29 10:21:16 2014
 * Generated by createNew
 */

#ifdef WIN32
#include <ciso646>
#endif

#include "ArcGeoSim/Time/ITimeStepComputer.h"

#include <arcane/ISubDomain.h>

/*---------------------------------------------------------------------------*/

ArcGeoSim::TimeEvolution::
TimeEvolution(Arcane::ISubDomain* sd)
  : Arcane::TraceAccessor(sd->traceMng())
  , TimeVariables(sd)
  , m_computer_for_restore(NULL)
  , m_restore(false) {}

/*---------------------------------------------------------------------------*/

ArcGeoSim::TimeEvolution::
~TimeEvolution()
{
  for(Arcane::Integer i = 0; i < m_ownership_computers.size(); ++i)
    delete m_ownership_computers[i];
}

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeEvolution::
registerTimeStepComputer(TimeStep::IComputer* computer, 
                         Ownership::ePolicy owner)
{
  if(computer == NULL)
    fatal() << "Time step computer pointer null";

  m_computers.add(computer);

  if(owner == Ownership::Take)
    m_ownership_computers.add(computer);
}

/*---------------------------------------------------------------------------*/

void 
ArcGeoSim::TimeEvolution::
replay()
{
  if(m_computer_for_restore != NULL || m_restore == true)
      fatal() << "Error in TimeEvolution - Use TimeStep::Replay for failed time step";

  m_restore = true;
}

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeEvolution::
replay(TimeStep::IComputer* computer)
{
  if(computer == NULL) 
    fatal() << "TimeStep::IComputer pointer null";

  if(m_computer_for_restore != NULL || m_restore == true)
    fatal() << "Error in TimeEvolution - Use TimeStep::Replay for failed time step";

  const Arcane::Integer size = m_computers.size();

  bool is_registered = false;

  for(Arcane::Integer i = 0; i < size; ++i) {
    is_registered |= (m_computers[i] == computer);
  }

  if(not is_registered) 
    fatal() << "Error in TimeEvolution - Time step computer for restore is not registered";

  m_computer_for_restore = computer;
  m_restore = true;
}

/*---------------------------------------------------------------------------*/

Arcane::Real
ArcGeoSim::TimeEvolution::
suggestedNextTimeStep() const
{
  const Arcane::Integer size = m_computers.size();

  if(size == 0) {
    return m_dt();
  }

  Arcane::Real _dt = Arcane::FloatInfo<Arcane::Real>::maxValue();

  for(Arcane::Integer i = 0; i < size; ++i) {
    _dt = Arcane::math::min(_dt, m_computers[i]->nextTimeStep());
  }

  return _dt;
}

/*---------------------------------------------------------------------------*/

Arcane::Real
ArcGeoSim::TimeEvolution::
suggestedRestoredTimeStep() const
{
  Arcane::Real _dt = Arcane::FloatInfo<Arcane::Real>::maxValue();
  
  if(m_computer_for_restore == NULL) {

    const Arcane::Integer size = m_computers.size();

    if(size == 0)
      fatal() << "Restore can't be used - no policy for restoring!";

    for(Arcane::Integer i = 0; i < size; ++i) {
      _dt = Arcane::math::min(_dt, m_computers[i]->restoredTimeStep());
    }

  } else {

    _dt = Arcane::math::min(_dt, m_computer_for_restore->restoredTimeStep());

    m_computer_for_restore = NULL;
  }

  m_restore = false;

  return _dt;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
