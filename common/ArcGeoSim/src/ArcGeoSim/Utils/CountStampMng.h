// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * CountStamp.h
 *
 *  Created on: Jun 15, 2012
 *      Author: gratienj
 */

#ifndef COUNTSTAMP_H_
#define COUNTSTAMP_H_

namespace ArcGeoSim
{
  class CountStampMng
  {
  public:
    CountStampMng()
    : m_count_stamp(-1){}
    virtual ~CountStampMng(){}
    int getCountStamp() const {
      return m_count_stamp ;
    }
    int getNewCountStamp() const {
      return ++m_count_stamp ;
    }
  protected :
    mutable int m_count_stamp ;
  };

  class CountStampObject
  {
  public:
    CountStampObject()
    : m_count_stamp(-1){}
    virtual ~CountStampObject(){}
    bool isUpdated(int count) const {
      return m_count_stamp >= count ;
    }
    void setUpdated(int count) {
      m_count_stamp = count ;
    }
    int getCountStamp() const {
      return m_count_stamp ;
    }
  protected :
     int m_count_stamp ;
  } ;
} /* namespace ArcGeoSim */
#endif /* COUNTSTAMP_H_ */
