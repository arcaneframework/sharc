// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_TIMEEVOLUTION_H
#define ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_TIMEEVOLUTION_H
/* Author : desrozis at Wed Oct 29 10:21:16 2014
 * Generated by createNew
 */

#include "ArcGeoSim/Time/ITimeEvolution.h"

#include "ArcGeoSim/Time/TimeVariables.h"

#include <arcane/utils/TraceAccessor.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace TimeStep {

  class IComputer;

}

class TimeEvolution
  : public ITimeEvolution
  , private Arcane::TraceAccessor
  , private TimeVariables
{
public:
  
  TimeEvolution(Arcane::ISubDomain* sd);
  
  virtual ~TimeEvolution();
  
public:

  //!@{ ITimeEvolution
  void registerTimeStepComputer(TimeStep::IComputer* policy, 
                                Ownership::ePolicy owner);

  void replay();
  void replay(TimeStep::IComputer* policy);

  Arcane::Real suggestedNextTimeStep() const;

  Arcane::Real suggestedRestoredTimeStep() const;
  //!}

private:

  Arcane::SharedArray<TimeStep::IComputer*> m_computers;
  Arcane::SharedArray<TimeStep::IComputer*> m_ownership_computers;

  mutable TimeStep::IComputer* m_computer_for_restore;
  mutable bool m_restore;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_TIMEEVOLUTION_H */
