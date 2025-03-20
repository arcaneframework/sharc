// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * ProfilerMng.h
 *
 *  Created on: Feb 2, 2012
 *      Author: gratienj
 *
 *
 */

#ifndef PROFILERMNG_H_
#define PROFILERMNG_H_

#ifndef WIN32
#ifdef USE_GOOGLE_PERF_TOOLS
#include <google/profiler.h>
#endif 
#endif 

namespace ArcGeoSim
{

  class ProfilerMng
  {
  public:
    ProfilerMng(std::string file_name="perf.log")
    : m_file_name(file_name)
    {}

    virtual ~ProfilerMng(){}

    void setFileName(std::string file_name) {
      m_file_name = file_name ;
    }

    void start() {
#ifndef WIN32
#ifdef USE_GOOGLE_PERF_TOOLS
      ProfilerStart(m_file_name.c_str()) ;
#endif
#endif
    }
    void stop() {
#ifndef WIN32
#ifdef USE_GOOGLE_PERF_TOOLS
      ProfilerStop() ;
#endif
#endif
    }
  private :
    std::string m_file_name ;
  };

} /* namespace ArcGeoSim */
#endif /* PROFILERMNG_H_ */
