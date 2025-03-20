// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_ACTIVITY_REPLAYTIMESTEP_H
#define ARCGEOSIM_ARCGEOSIM_TIME_ACTIVITY_REPLAYTIMESTEP_H

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#if (ARCANE_VERSION >= 12003)  
#include <arcane/utils/GoBackwardException.h>
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace TimeStep {

  class IComputer;
  
  class Replay 
  {
  public:
    
    Replay();

    Replay(IComputer* computer);
    
    // Pour windows : les exceptions sont recopi�es!
    // Si RAII, besoin de g�rer une unique action pour plusieurs objets
    Replay(const Replay&);
    
    ~Replay();
    
    void operator[](Arcane::String message);
    
  private:
    
    bool m_post_message;
    
    IComputer* m_computer;
  };
  
  
  class ReplayException 
#if (ARCANE_VERSION >= 12003)  
    : public Arcane::GoBackwardException
#endif
  {
  public:
    
    ReplayException(Arcane::String message);

    ReplayException(IComputer* computer,
                    Arcane::String message);
    
    ReplayException(const ReplayException& r);
    
    ~ReplayException() throw() {}

  private:
    
    Arcane::String m_message;
    
    Replay m_replay;
  };

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_ACTIVITY_REPLAYTIMESTEP_H */
