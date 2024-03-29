// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_IEVENTCALENDAR_H
#define ARCGEOSIM_ARCGEOSIM_TIME_IEVENTCALENDAR_H
/* Author : desrozis at Fri Oct 24 17:55:37 2014
 * Generated by createNew
 */

#include "ArcGeoSim/Time/Activity/TimeStepEntryPoint.h"
#include "ArcGeoSim/Time/OwnershipPolicy.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IEvent;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IEventCalendar
{
public:

  virtual ~IEventCalendar() {}
  
public:

  //! Schedule event in calendar according to ownership policy
  virtual void schedule(IEvent* event, Ownership::ePolicy owner = Ownership::Take) = 0;

  //! Execute event according to user time step entry point
  virtual void execute(const TimeStep::EntryPoint& entry_point) = 0;
 
  //! Print info of events
  virtual void print() const = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_IEVENTCALENDAR_H */
