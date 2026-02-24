// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_TIMELOOPSNAPSHOTMANAGER_H
#define ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_TIMELOOPSNAPSHOTMANAGER_H

#include "ArcGeoSim/Appli/ITimeLoopSnapshotManager.h"

#include <arcane/utils/TraceAccessor.h>

#include <map>
#include <set>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class TimeLoopSnapshotManager
  : public ITimeLoopSnapshotManager
  , public Arcane::TraceAccessor
{
public:
  
  TimeLoopSnapshotManager(Arcane::ISubDomain* sd, Arcane::String snapshot_prefix="TimeLoopSnapshot_");
  
  virtual ~TimeLoopSnapshotManager();
  
public:

  void init() { /* Pour �tre partag� */ }

  void setVerbosity(bool verbosity) { m_verbosity = verbosity; }
  
  bool verbosity() const { return m_verbosity; }
  
  void save();

  void restore();

private:

  Arcane::String _ownNamingRule(Arcane::String name) const;

  void _snap(Arcane::IVariable* v, Arcane::VariableRef* snapshot, bool is_own);

  Arcane::IVariable* _snapshot(Arcane::IVariable* v) const;

  bool _greedyContains(Arcane::IVariable* v) const;

private:

  bool m_verbosity;

  Arcane::ISubDomain* m_sub_domain;

  std::map<Arcane::IVariable*, Arcane::VariableRef*> m_clones;

  std::set<Arcane::VariableRef*> m_to_delete;

  Arcane::VariableScalarReal m_time;
  Arcane::VariableScalarReal m_time_n;
  Arcane::VariableScalarReal m_final_time;
  Arcane::VariableScalarInt32 m_iteration;

  Arcane::Real m_save_time;
  Arcane::Real m_save_time_n;
  Arcane::Real m_save_final_time;
  Arcane::Integer m_save_iteration;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_TIMELOOPSNAPSHOTMANAGER_H */
