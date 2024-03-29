// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_ACTIVITY_TIMESTEPENTRYPOINT_H
#define ARCGEOSIM_ARCGEOSIM_TIME_ACTIVITY_TIMESTEPENTRYPOINT_H
/* Author : desrozis at Thu Oct 30 11:22:25 2014
 * Generated by createNew
 */

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include <map>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace TimeStep {

  class EntryPoint
  {
  protected:
    
    EntryPoint(Arcane::Integer key, Arcane::String name);
    
  public:

    EntryPoint() 
      : m_key(-1)
      , m_name() {}
 
    EntryPoint(const EntryPoint& ep) 
      : m_key(ep.m_key)
      , m_name(ep.m_name) {}

    ~EntryPoint() {}
    
    Arcane::Integer key() const { return m_key; }

    Arcane::String name() const { return m_name; }
    
    operator Arcane::Integer() const { return m_key; }
    
    bool operator==(const EntryPoint& ep) const { return m_key == ep.m_key; }
    
    EntryPoint& operator=(const EntryPoint& ep) 
    {
      m_key = ep.m_key;
      m_name = ep.m_name;
      return *this;
    }

    void notify();

  protected:
    
    Arcane::Integer m_key;
    Arcane::String m_name;
    
    friend class EntryPoints;
  };
  
  struct Begin : public EntryPoint
  {
    Begin() : EntryPoint(0,"Begin") {}
  };
  
  struct End : public EntryPoint
  {
    End() : EntryPoint(1,"End") {}
  };
  
  struct BeforeRestoreSave : public EntryPoint
  {
    BeforeRestoreSave() : EntryPoint(2,"BeforeRestoreSave") {}
  };
  
  class EntryPoints
  {
  public:
    enum eCheckPolicy { CreateIfNew, CheckIsNew, CheckExisting };

  public:
    
    EntryPoints();

    ~EntryPoints() {}
    
    Arcane::Integer numberOfEntryPoints() const { return m_entry_points.size(); }

    EntryPoint newUserEntryPoint();

    EntryPoint userEntryPoint(Arcane::String name, eCheckPolicy = CreateIfNew);
    
  private:
    
    static void checkName(Arcane::String name);
    static void checkKey(Integer key);

  private:
    
    typedef std::map<String, Integer> EntryPointInternalMap;
    EntryPointInternalMap m_entry_points;
  };
  
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_ACTIVITY_TIMESTEPENTRYPOINT_H */
