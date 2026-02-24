// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "TimeLoopSnapshotManager.h"
#include <arcane/ISubDomain.h>

/*---------------------------------------------------------------------------*/

ArcGeoSim::TimeLoopSnapshotManager::
TimeLoopSnapshotManager(Arcane::ISubDomain* sd, Arcane::String snapshot_prefix)
  : Arcane::TraceAccessor(sd->traceMng())
  , m_verbosity(false)
  , m_sub_domain(sd)
  , m_snapshot_prefix(snapshot_prefix)
  , m_time(Arcane::VariableBuildInfo(sd,"GlobalTime"))
  , m_time_n(Arcane::VariableBuildInfo(sd,"GlobalOldTime"))
  , m_final_time(Arcane::VariableBuildInfo(sd,"GlobalFinalTime"))
  , m_iteration(Arcane::VariableBuildInfo(sd,"GlobalIteration"))
  , m_save_time(m_time())
  , m_save_time_n(m_time_n())
  , m_save_final_time(m_final_time())
  , m_save_iteration(m_iteration()) {}

/*---------------------------------------------------------------------------*/

ArcGeoSim::TimeLoopSnapshotManager::
~TimeLoopSnapshotManager()
{
  for(auto* v : m_to_delete) {
    delete v; 
  }
}

/*---------------------------------------------------------------------------*/

Arcane::String
ArcGeoSim::TimeLoopSnapshotManager::
_ownNamingRule(Arcane::String name) const
{
  return m_snapshot_prefix + name;
}

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeLoopSnapshotManager::
restore()
{
  m_iteration = m_save_iteration;
  m_time = m_save_time;
  m_time_n = m_save_time_n;
  m_final_time = m_save_final_time;

  for(std::map<Arcane::IVariable*,Arcane::VariableRef*>::iterator var = m_clones.begin();
      var != m_clones.end(); ++var) { 
    var->first->data()->copy(var->second->variable()->data());
  }
}

/*---------------------------------------------------------------------------*/

void
ArcGeoSim::TimeLoopSnapshotManager::
save()
{ 
  m_save_iteration = m_iteration();
  m_save_time = m_time();
  m_save_time_n = m_time_n();
  m_save_final_time = m_final_time();
  
  for(std::map<Arcane::IVariable*,Arcane::VariableRef*>::iterator var = m_clones.begin();
      var != m_clones.end(); ++var) {
    var->second->variable()->data()->copy(var->first->data());
  }
}

/*---------------------------------------------------------------------------*/

void 
ArcGeoSim::TimeLoopSnapshotManager::
_snap(Arcane::IVariable* v, Arcane::VariableRef* snapshot, bool is_own)
{
  ARCANE_ASSERT((v != NULL), ("Arcane::IVariable pointer null"));
  ARCANE_ASSERT((snapshot != NULL), ("Arcane::VariableRef pointer null"));
  ARCANE_ASSERT((!_greedyContains(v)),("Arcane::IVariable already registered in TimeLoopSnapshotManager"));
  ARCANE_ASSERT((!_greedyContains(snapshot->variable())),("Arcane::VariableRef already registered in TimeLoopSnapshotManager"));

  m_clones[v] = snapshot;

  if(is_own)
    m_to_delete.insert(snapshot);
}
  
/*---------------------------------------------------------------------------*/

Arcane::IVariable*
ArcGeoSim::TimeLoopSnapshotManager::
_snapshot(Arcane::IVariable* v) const
{
  ARCANE_ASSERT((v != NULL), ("Arcane::IVariable pointer null"));

  std::map<Arcane::IVariable*, Arcane::VariableRef*>::const_iterator it = m_clones.find(v);
  if(it == m_clones.end())
    fatal() << "variable '" << v->name() << "' not registered in TimeLoopSnapshotManager";
  
  return it->second->variable();
}

/*---------------------------------------------------------------------------*/

bool 
ArcGeoSim::TimeLoopSnapshotManager::
_greedyContains(Arcane::IVariable* v) const
{ 
  std::map<Arcane::IVariable*, Arcane::VariableRef*>::const_iterator it = m_clones.begin();
  for(; it != m_clones.end(); ++it) {
    if(it->first == v) return true;
    if(it->second->variable() == v) return true;
  }
  return false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
