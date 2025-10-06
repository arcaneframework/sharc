// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * HARTSImpl.h
 *
 *  Created on: Nov 2, 2014
 *      Author: gratienj
 */

#ifndef HARTSTHREADENV_H_
#define HARTSTHREADENV_H_

namespace ArcGeoSim
{

  class HARTSThreadEnv : public ArcGeoSim::IThreadEnv
  {
  public :
#ifdef USE_HARTS
    typedef RunTimeSystem::ThreadEnv ThreadEnvType ;
    typedef ArcGeoSim::IThreadEnv    BaseType ;
    HARTSThreadEnv(ThreadEnvType* impl=NULL)
     : BaseType()
     , m_impl(impl)
     {}

    HARTSThreadEnv(Integer nb_threads)
     {
        m_impl.reset(new ThreadEnvType(nb_threads)) ;
     }

    Integer getNbThreads() const {
      return m_impl->getNbThreads() ;
    }


    ThreadEnvType*  getImpl() {
      return m_impl.get() ;
    }

  private:
     boost::shared_ptr<ThreadEnvType>  m_impl ;
#else
    HARTSThreadEnv(){}
    virtual ~HARTSThreadEnv(){}
    Integer getNbThreads() const { return 0 ; }
#endif
  };

} /* namespace ArcGeoSim */

#endif /* HARTSTHREADENV_H_ */
