// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * IRunTimeSystemMng.h
 *
 *  Created on: Nov 1, 2014
 *      Author: gratienj
 */

#ifndef IRUNTIMESYSTEMMNG_H_
#define IRUNTIMESYSTEMMNG_H_

namespace ArcGeoSim
{
  class IThreadEnv
  {
  public :
    IThreadEnv(){}
    virtual ~IThreadEnv(){}
    virtual Integer getNbThreads() const = 0 ;
  };

  class IRunTimeSystemMng
  {
  public:
    IRunTimeSystemMng(){}
    virtual ~IRunTimeSystemMng(){}
    virtual void init() = 0 ;
    virtual IThreadEnv* getThreadEnv() = 0 ;
  };

} /* namespace ArcGeoSim */

#endif /* IRUNTIMESYSTEMMNG_H_ */
