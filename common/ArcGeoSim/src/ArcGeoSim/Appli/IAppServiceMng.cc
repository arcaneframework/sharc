// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "IAppServiceMng.h"

#include <arcane/IModule.h>
#include <arcane/IEntryPoint.h>
#include <arcane/ITimeLoopMng.h>
#include <arcane/utils/FatalErrorException.h>

#include <arcane/utils/FatalErrorException.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IAppServiceMng * IAppServiceMng::m_instance = NULL;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IAppServiceMng::
IAppServiceMng()
  : m_time_loop_mng(NULL)
{
  registerInstance() ;
}

void IAppServiceMng::
registerInstance()
{
  if (m_instance)
    throw Arcane::FatalErrorException("IAppServiceMng already registered");
  m_instance = this;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
IAppServiceMng::
addService(IService* service)
{
  ARCANE_ASSERT((service != NULL),("Service pointer null"));
  _traceMng()->debug() << "Adding Service " << typeid(*service).name() << " (" << service << ")";
  SharedPointer p(new PointerT<IService>(service));
  m_services.add(std::pair<SharedPointer,bool>(p,true)) ;
}
  
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
IAppServiceMng::
addService(IService* service, const String & type_name)
{
  ARCANE_ASSERT((service != NULL),("Service pointer null"));
  _traceMng()->debug() << "Adding Service " << typeid(*service).name() << " for " << type_name << " (" << service << ")";
  SharedPointer p(new PointerT<IService>(service));
  m_services.add(std::pair<SharedPointer,bool>(p,true)) ; 
}
  
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
IAppServiceMng::
addMissingServiceInfo(const String & type_id, const String & type_name)
{
  _traceMng()->debug() << "Adding missing Service " << type_id << " for " << type_name;
  m_missing_service_infos[type_id] = type_name ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
IAppServiceMng::
_removeMissingServiceInfo(const String & type_id)
{
  m_missing_service_infos.erase(type_id);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
IAppServiceMng::
setTimeLoopMng(ITimeLoopMng * time_loop_mng)
{
  m_time_loop_mng = time_loop_mng;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
IAppServiceMng::
_checkBeforeFind(const char * name)
{
  std::map<String,String>::const_iterator i_missing_service = m_missing_service_infos.find(name);
  if (i_missing_service != m_missing_service_infos.end())
    {
      String context;
      if (m_time_loop_mng) {
        IEntryPoint * entry = m_time_loop_mng->currentEntryPoint();
        context = " in " + entry->module()->name() + "::" + entry->name();
      }
      _traceMng()->fatal() 
        << "Requested optional " << i_missing_service->second << " service" << context << " not configured in your .arc file";
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
IAppServiceMng::
_checkNotFound(const char * name)
{
  String context;
  if (m_time_loop_mng) {
    IEntryPoint * entry = m_time_loop_mng->currentEntryPoint();
    context = " in " + entry->module()->name() + "::" + entry->name();
  }
  _traceMng()->fatal()
    << "Requested service typeid " << name << context << " cannot be found";
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ITraceMng *
IAppServiceMng::
_traceMng()
{
  IService * s;
  IModule * m;
  if ((s = dynamic_cast<IService*>(this))) { // IAppServiceMng is a service
    return s->serviceParent()->traceMng();
  } else if ((m = dynamic_cast<IModule*>(this))) {
    return m->traceMng();
  } else {
    throw Arcane::FatalErrorException("IAppServiceMng is neither a IService nor a IModule");
    return NULL;
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IAppServiceMng *
IAppServiceMng::
instance(IServiceMng * sm, bool fatal_if_not_found)
{
  if (m_instance) 
    {
      m_instance->initializeAppServiceMng();
      return m_instance;
    }
  else if (sm == NULL)
    {
      throw Arcane::FatalErrorException("Cannot find IAppServiceMng by static registration");
    }
  else 
    {
      throw Arcane::FatalErrorException("IAppServiceMng dynamic instanciation is no more allowed");
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
