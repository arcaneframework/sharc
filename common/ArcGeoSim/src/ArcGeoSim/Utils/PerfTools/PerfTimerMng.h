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

#ifndef PERFTIMERMNG_H_
#define PERFTIMERMNG_H_

#include <boost/timer.hpp>
#include <map>
#include <iostream>
#include <iomanip>

template<typename PhaseT>
class PerfTimerMng
{
public:
  typedef PhaseT                         PhaseType ;
  typedef std::pair<double,boost::timer> CountType ;
  typedef std::map<PhaseType,CountType>  CountListType ;
  PerfTimerMng()
  : m_last_value(0){}
  virtual ~PerfTimerMng(){}

  void init(PhaseType const& phase) {
    CountType& count = m_counts[phase] ;
    count.first = 0. ;
    count.second.restart() ;
  }
  void start(PhaseType const& phase)
  {
    m_counts[phase].second.restart() ;
  }
  void stop(PhaseType const& phase)
  {
    CountType& count = m_counts[phase] ;
    m_last_value = count.second.elapsed() ;
    count.first += m_last_value ;
  }

  double getLastValue() {
    return m_last_value ;
  }

  double getValue(PhaseType const& phase) {
    return m_counts[phase].first ;
  }

  void printInfo() const {
    std::cout<<"PERF INFO : "<<std::endl ;
    std::cout<<std::setw(10)<<"COUNT"<<" : "<<"VALUE"<<std::endl ;
    for(typename CountListType::const_iterator iter = m_counts.begin();iter!=m_counts.end();++iter)
    {
      std::cout<<std::setw(10)<<(*iter).first<< " : "<<(*iter).second.first<<'\n' ;
    }
  }

  void printInfo(std::ostream& stream) const {
    stream<<"PERF INFO : "<<std::endl ;
    stream<<std::setw(10)<<"COUNT"<<" : "<<"VALUE"<<std::endl ;
    for(typename CountListType::const_iterator iter = m_counts.begin();iter!=m_counts.end();++iter)
    {
      stream<<std::setw(10)<<(*iter).first<< " : "<<(*iter).second.first<<'\n' ;
    }
  }
private :
  double m_last_value ;
  CountListType m_counts ;
};

#endif /* PERFTIMERMNG_H_ */
