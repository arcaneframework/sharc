// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ObserverSet.h"

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeStep::ObserverSet::
attachObserver(IObserver* observer)
{
  if (std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
    m_observers.push_back(observer);
  }

  observer->attachObservable(m_observable);
}

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeStep::ObserverSet::
detachObserver(IObserver* observer)
{
  auto it = std::find(m_observers.begin(), m_observers.end(), observer);
  if(it != m_observers.end())
    m_observers.erase(it);
}

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeStep::ObserverSet::
notifyObservers(const EntryPoint& entry_point)
{
  // On recopie au cas o� les observers se d�tachent

  std::vector<IObserver*> observers = m_observers;
  for(Arcane::Integer i = 0; i < (Integer)observers.size(); ++i)
    observers[i]->notify(entry_point);
}

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeStep::ObserverSet::
notifyObservers(eTimeStepState state)
{
  // On recopie au cas o� les observers se d�tachent

  std::vector<IObserver*> observers = m_observers;
  for(Arcane::Integer i = 0; i < (Integer)observers.size(); ++i)
	observers[i]->notify(state);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
