// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "Event.h"
/* Author : desrozis at Thu Oct 30 16:54:25 2014
 * Generated by createNew
 */

#ifdef WIN32
#include <ciso646>
#endif

#include "ArcGeoSim/Time/Observer/IObservable.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Event::
Event(Arcane::String name,
      Arcane::Real date,
      Arcane::Integer entry_points,
      Arcane::Integer types)
  : EventBasics(name,date,entry_points,types)
{
  if(types & Recurrent)
    fatal() << "ArcGeoSim::Event is not suitable for reccurent events";
}

/*---------------------------------------------------------------------------*/

Event::
Event(Arcane::String name,
      Arcane::Real start_date,
      Arcane::Real end_date,
      Arcane::Integer entry_points,
      Arcane::Integer types)
  : EventBasics(name,start_date,end_date,entry_points,types)
{
  if(types & Recurrent)
    fatal() << "ArcGeoSim::Event is not suitable for reccurent events";
}

/*---------------------------------------------------------------------------*/

void
Event::
notify(eTimeStepState state)
{
  switch(state) {
  case TimeStepIsFinished:
    if(m_start_date == m_end_date) {
      if(m_start_date == m_time_n()) {
        m_observable->detachObserver(this);
        return;
      }
    } else if(m_end_date == m_time()) {
      m_observable->detachObserver(this);
      return;
    }
    break;
  case TimeStepIsRestored:
    if(m_start_date == m_end_date) {
      if(m_start_date == m_time_n()) {
        if(not(m_types        & Persistent) &&
           not(m_entry_points & TimeStep::End())) {
          m_observable->detachObserver(this);
          return;
        }
      }
    }
    break;
  default:
    fatal() << "Time step type is not defined";
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
