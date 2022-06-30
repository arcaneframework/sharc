// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "Delegate.h"
/* Author : desrozis at Fri Jun 12 14:15:10 2015
 * Generated by createNew
 */

#include "ArcGeoSim/Appli/Architecture/IExtraEntryPoints.h"
#include "ArcGeoSim/Appli/Architecture/INextTimeComputer.h"
#include "ArcGeoSim/Appli/Architecture/IIterationInformation.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IExtraEntryPoints;
class INextTimeComputer;
class IIterationInformation;

template class Delegate<IExtraEntryPoints>;
template class Delegate<INextTimeComputer>;
template class Delegate<IIterationInformation>;

template<> IExtraEntryPoints* Delegate<IExtraEntryPoints>::m_unique_delegate = NULL;
template<> INextTimeComputer* Delegate<INextTimeComputer>::m_unique_delegate = NULL;
template<> IIterationInformation* Delegate<IIterationInformation>::m_unique_delegate = NULL;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
