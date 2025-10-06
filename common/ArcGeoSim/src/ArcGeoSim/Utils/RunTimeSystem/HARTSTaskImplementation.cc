// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* HARTSTaskImplementation.cc                                    (C) 2000-2010 */
/*                                                                           */
/* Impl�mentation des t�ches utilisant TBB (Intel Threads Building Blocks).  */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Utils/RunTimeSystem/HARTSTaskImplementation.h"


ARCANE_BEGIN_NAMESPACE
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

HARTSTaskImplementation* HARTSTaskImplementation::m_instance = NULL ;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_APPLICATION_FACTORY(HARTSTaskImplementation,ITaskImplementation,
                                    HARTSTaskImplementation);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
