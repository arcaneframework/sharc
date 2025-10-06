// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Physics/Law1/VariableManager.h"

#ifdef WIN32
#include <ciso646>
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

VariableManager::
VariableManager(Arcane::IMesh* mesh)
  : m_name("")
  , m_mesh(mesh)
  , m_trace(NULL)
  , m_verbose(false) 
{
  m_default_family_name[Arcane::IK_Particle] = "Particle";
}

/*---------------------------------------------------------------------------*/

VariableManager::
VariableManager(Arcane::IMesh* mesh, Arcane::ITraceMng* trace)
  : m_name("")
  , m_mesh(mesh)
  , m_trace(trace)
  , m_verbose(true) 
{
  m_default_family_name[Arcane::IK_Particle] = "Particle";
}


/*---------------------------------------------------------------------------*/
 
VariableManager::
VariableManager(Arcane::String name, Arcane::IMesh* mesh)
  : m_name(name)
  , m_mesh(mesh)
  , m_trace(NULL)
  , m_verbose(false)
{
  m_default_family_name[Arcane::IK_Particle] = "Particle";
}

/*---------------------------------------------------------------------------*/

VariableManager::
VariableManager(Arcane::String name, Arcane::IMesh* mesh, Arcane::ITraceMng* trace)
  : m_name(name)
  , m_mesh(mesh)
  , m_trace(trace)
  , m_verbose(true)
{
  m_default_family_name[Arcane::IK_Particle] = "Particle";
}

/*---------------------------------------------------------------------------*/

void 
VariableManager::
setDefaultFamily(Arcane::IItemFamily* family)
{
  ARCANE_ASSERT((family != nullptr),("Arcane::IItemFamily pointer null"));
  
  m_default_family_name[family->itemKind()] = family->name();
}

/*---------------------------------------------------------------------------*/
 
VariableManager& 
VariableManager::
operator<<(const std::shared_ptr<IAllocator>& info)
{
  info->addPrefix(m_name);
  info->setDefaultFamilyNames(m_default_family_name);
  return ( *this << info->create(m_mesh) );
}
  
/*---------------------------------------------------------------------------*/
 
VariableManager& 
VariableManager::
operator<<(const std::shared_ptr<VariableRef>& info)
{
  const Property& p = info->property();
    
  if(info->isMeshReference()) {
    if(info->isPartialReference()) {
      _check(m_partial_variable,p);
      m_partial_variable[p] << info;
    } else {
      _check(m_variable,p);
      m_variable[p] << info;
    }
  } else {
    if(info->isScalar()) {
      _check(m_scalar,p);
      m_scalar[p] << info;
    } else {
      _check(m_array,p);
      m_array[p] << info;
    }
  }
  _printRegisterInfo(info); 
  return *this;
}

/*---------------------------------------------------------------------------*/
 
void 
VariableManager::
_check(const Accessor::DataBase& base, const Property& p) const 
{
  Accessor::DataBase::const_iterator it = base.find(p);
    
  if((it != base.end()) && (p.name() != it->first.name()))
    throw Arcane::FatalErrorException("Property has a id already registered but with another name");
}

/*---------------------------------------------------------------------------*/

void 
VariableManager::
_printRegisterInfo(const std::shared_ptr<VariableRef>& info) const
{
  if(not m_verbose) return;
    
  m_trace->info() << "Register property '" << info->property().name() << "' in law variable manager";
  if(info->isMeshReference()) {
    if(info->isPartialReference()) 
      m_trace->info() << " ** Partial variable container of '" << info->dataType() << "' on '" << info->itemKind() << "'";
    else
      m_trace->info() << " ** Variable container of '" << info->dataType() << "' on '" << info->itemKind() << "'";
  } else {
    if(info->isScalar())
      m_trace->info() << " ** Scalar container of '" << info->dataType() << "'";
    else
      m_trace->info() << " ** Array container of '" << info->dataType() << "'";
  }
  m_trace->info() << " ** values variable name = '" << info->values().name() << "'";
  if(info->hasDerivatives())
    m_trace->info() << " ** derivatives variable name = '" << info->derivatives().name() << "'";
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
