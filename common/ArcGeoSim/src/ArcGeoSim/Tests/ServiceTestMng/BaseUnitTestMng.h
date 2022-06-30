// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * BaseUnitTestMng.h
 *
 *  Created on: 31 janv. 2014
 *      Author: gratienj
 */

#ifndef BASEUNITTESTMNG_H_
#define BASEUNITTESTMNG_H_

namespace ArcGeoSim
{

  template<typename ImplT>
  class BaseUnitTestMng
  {
  public:
    BaseUnitTestMng(ImplT* impl)
  : m_impl(impl){}
    virtual ~BaseUnitTestMng(){}

    virtual void test()
    {
        m_impl->info() << "compute";
        int success = m_impl->options()->service[0]->test();
        if(success==1)
        {
          m_impl->info()<<"Test Iteration has succeeded";
          if(!m_impl->options()->service[0]->goOn())
          {
            m_impl->info()<<"STOP PERFECT";
            m_impl->subDomain()->timeLoopMng()->stopComputeLoop(true);
          }
        }
        else
          m_impl->fatal()<<"Test has failed";
    }
  private :
    ImplT* m_impl ;
  };

} /* namespace ArcGeoSim */

#endif /* BASEUNITTESTMNG_H_ */
