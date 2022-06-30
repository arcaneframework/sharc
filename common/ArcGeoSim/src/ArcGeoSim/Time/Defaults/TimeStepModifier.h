// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_DEFAULTS_TIMESTEPMODIFIER_H
#define ARCGEOSIM_ARCGEOSIM_TIME_DEFAULTS_TIMESTEPMODIFIER_H
/* Author : desrozis at Wed Nov 12 09:46:49 2014
 * Generated by createNew
 */

#include "ArcGeoSim/Time/TimeVariables.h"
#include "ArcGeoSim/Time/Defaults/Event.h"

#include "ArcGeoSim/Utils/Optional.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class TimeStepModifier
    : public Event
{
public:
  
  TimeStepModifier(Arcane::String name,
                   Arcane::Real date,
                   const Optional<Arcane::Real>& new_min_dt,
                   const Optional<Arcane::Real>& new_max_dt,
                   const Optional<Arcane::Real>& new_dt);

  void execute();

private:

  //! New min time step parameter
  Optional<Arcane::Real> m_new_min_dt;

  //! New max time step parameter
  Optional<Arcane::Real> m_new_max_dt;

  //! New time step
  Optional<Arcane::Real> m_new_dt;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_DEFAULTS_TIMESTEPMODIFIER_H */
