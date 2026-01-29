// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * PerfCountMng.h
 *
 *  Created on: Jan 26, 2012
 *      Author: gratienj
 */

#ifndef ARCGEOSIM_PERFCOUNTERMNG_H_
#define ARCGEOSIM_PERFCOUNTERMNG_H_

#include <map>
#include <iostream>
#include <iomanip>
#include "ArcGeoSim/Utils/PerfTools/rdtsc.h"

template<typename PhaseT>
class PerfCounterMng
{
public:
  typedef unsigned long long int         ValueType ;
  typedef PhaseT                         PhaseType ;
  typedef std::pair<ValueType,ValueType> CountType ;
  typedef std::map<PhaseType,CountType>  CountListType ;

  PerfCounterMng()
  : m_last_value(0)
  {
    m_cpu_frec = getCpuFreq() ;
  }

  virtual ~PerfCounterMng(){}

  void init(PhaseType const& phase) {
    CountType& count = m_counts[phase] ;
    count.first = 0 ;
    count.second = 0 ;
  }
  void start(PhaseType const& phase)
  {
    rdtsc(&m_counts[phase].second) ;
  }
  void stop(PhaseType const& phase)
  {
    CountType& count = m_counts[phase] ;
    rdtsc(&m_last_value) ;
    m_last_value = m_last_value - count.second ;
    count.first += m_last_value ;
  }

  ValueType getLastValue() {
    return m_last_value ;
  }

  ValueType getValue(PhaseType const& phase) {
    return m_counts[phase].first ;
  }

  double getValueInSeconds(PhaseType const& phase) {
    return m_counts[phase].first/m_cpu_frec*1E-6 ;
  }

  void printInfo() const {
    std::cout<<"PERF INFO : "<<std::endl ;
    std::cout<<std::setw(10)<<"COUNT"<<" : "<<"VALUE"<<std::endl ;
    for(typename CountListType::const_iterator iter = m_counts.begin();iter!=m_counts.end();++iter)
    {
      std::cout<<std::setw(10)<<(*iter).first<< " : "<<(*iter).second.first/m_cpu_frec*1E-6<<'\n' ;
    }
  }

  void printInfo(std::ostream& stream) const {
    stream<<"PERF INFO : "<<std::endl ;
    stream<<std::setw(10)<<"COUNT"<<" : "<<"VALUE"<<std::endl ;
    for(typename CountListType::const_iterator iter = m_counts.begin();iter!=m_counts.end();++iter)
    {
      stream<<std::setw(10)<<(*iter).first<< " : "<<(*iter).second.first/m_cpu_frec*1E-6<<'\n' ;
    }
  }

  int getCpuFreq()
  {
    /* return cpu frequency in MHZ as read in /proc/cpuinfo */
    float ffreq = 0;
    int r = 0;
    char *rr = NULL;
#ifndef WIN32
    FILE *fdes = fopen("/proc/cpuinfo","r");
    char buff[256];
    size_t bufflength = 256;
    do{
      rr = fgets(buff,bufflength,fdes);
      r = sscanf(buff,"cpu MHz         : %f\n",&ffreq);
      if(r==1){
        break;
      }
    }while(rr != NULL);

    fclose(fdes);
#endif

    int ifreq = ffreq;
    return ifreq;
  }

private :
  ValueType     m_last_value ;
  CountListType m_counts ;
  double        m_cpu_frec ;
};

#endif /* PERFCOUNTERMNG_H_ */
