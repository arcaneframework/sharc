// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_UTILS_SINGLETON_H
#define ARCGEOSIM_UTILS_SINGLETON_H
/* Author : desrozis at Tue May 27 18:43:34 2014
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcGeoSim {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T>
class Singleton
{
public:
  
  static T& instance()
  {
    if(m_instance == 0)
      m_instance = new T;
    return *m_instance;
  }

  static void release()
  {
    delete m_instance;
    m_instance = 0;
  }

private:

  static T* m_instance;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T>
T* Singleton<T>::m_instance = 0;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_UTILS_SINGLETON_H */
