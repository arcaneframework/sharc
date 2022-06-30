// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ArcGeoSimCodeService.h                                         (C) 2000-2011 */
/*                                                                           */
/* Service de code generique Arcane.                                         */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_IMPL_ARCGEOSIMCODESERVICE_H
#define ARCANE_IMPL_ARCGEOSIMCODESERVICE_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/CodeService.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ArcGeoSimCodeService
  : public Arcane::CodeService
{
public:

  ArcGeoSimCodeService(const Arcane::ServiceBuildInfo& sbi);
  virtual ~ArcGeoSimCodeService();

public:

  virtual bool parseArgs(Arcane::StringList& args);
  virtual Arcane::ISession* createSession();
  virtual void initCase(Arcane::ISubDomain* sub_domain,bool is_continue);

public:

  void build() {}

protected:

  virtual void _preInitializeSubDomain(Arcane::ISubDomain* sd);

private:
  Arcane::String m_case_name;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

