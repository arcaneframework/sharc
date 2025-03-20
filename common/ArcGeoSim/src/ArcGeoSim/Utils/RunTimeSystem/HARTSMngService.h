// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_HARTSMNG_H
#define ARCGEOSIM_HARTSMNG_H


#include "ArcGeoSim/Utils/RunTimeSystem/IRunTimeSystemMng.h"
#include "HARTSMng_axl.h"

/**
 * HARTS MNG
 */

class HARTSMngService :
    public ArcaneHARTSMngObject
{
public:

  /** !brief Constructeur de la classe */
  HARTSMngService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneHARTSMngObject(sbi)
  , m_thread_env(NULL)
  , m_is_initialized(false)
  { }
  
  /** !brief Destructeur de la classe */
  virtual ~HARTSMngService() {}
  
public:
  //!initialization
  void init() ;
  
  ArcGeoSim::IThreadEnv* getThreadEnv() ;

private:
  ArcGeoSim::IThreadEnv*  m_thread_env ;
  bool                                  m_is_initialized ; //!< True si init valid�
};

//END_NAME_SPACE_PROJECT

#endif /* ARCGEOSIM_HARTSMNG_H */
