// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_APPLI_SHAREDSERVICE_H
#define ARCGEOSIM_APPLI_SHAREDSERVICE_H

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Appli/IAppServiceMng.h"

#include <arcane/ISubDomain.h>
#include <arcane/utils/ITraceMng.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C" Arcane::ISubDomain* _arcaneGetDefaultSubDomain();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
  
template<typename S>
class AppService
{
public:
    
  AppService()
    : m_app_service_mng(NULL)
    , m_shared(NULL)
	, m_is_required(true) {}

  S* operator->()       { return _lazy(m_is_required); }
  S* operator->() const { return _lazy(m_is_required); }
    
  operator S*()       { return _lazy(m_is_required); }
  operator S*() const { return _lazy(m_is_required); }
  
  bool operator==(S* s) const { return _lazy(m_is_required) == s; }
  bool operator!=(S* s) const { return _lazy(m_is_required) != s; }
  bool operator<(S* s) const { return _lazy(m_is_required) < s; }

protected:

  AppService(bool required)
    : m_app_service_mng(NULL)
    , m_shared(NULL)
	, m_is_required(required) {}

  IAppServiceMng* _lazyAppServiceMng(bool is_required) const
  {
    if(m_app_service_mng == NULL) {
      Arcane::ISubDomain* sub_domain = _arcaneGetDefaultSubDomain();
      Arcane::IServiceMng* services = sub_domain->serviceMng();
      m_app_service_mng = IAppServiceMng::instance(services, is_required);
    }
    return m_app_service_mng;
  }
  
  S* _lazy(bool is_required) const
  {
    if(m_app_service_mng == NULL) {
      if(_lazyAppServiceMng(is_required) == NULL)
        return NULL;
    }
    if(m_shared == NULL) {
      m_shared = m_app_service_mng->find<S>(is_required);
    }
    return m_shared;
  }

private:

  // const par design mais lazy pattern 
  mutable IAppServiceMng* m_app_service_mng;
  mutable S* m_shared;
  const bool m_is_required;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename S>
class MutableAppService
  : public AppService<S>
{
public:

  MutableAppService() {}

  MutableAppService<S>& operator=(S* service)
  {
    if(service == NULL)
      throw Arcane::FatalErrorException("Service pointer null");
    IAppServiceMng* app_service_mng = AppService<S>::_lazyAppServiceMng(true);
    S* s = app_service_mng->find<S>(false);
    if(s == NULL) {
      app_service_mng->addService(service);
    } else {
      throw Arcane::FatalErrorException("Error, shared service already registered");
    }
    return *this;
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename S>
void registerAppService(S* s)
{
  MutableAppService<S>() = s;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename S>
class OptionalAppService
  : public AppService<S>
{
public:
  
  OptionalAppService() : AppService<S>(false) {}

  bool isAvailable() { return AppService<S>::_lazy(false) != NULL; }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_APPLI_SHAREDSERVICE_H */
