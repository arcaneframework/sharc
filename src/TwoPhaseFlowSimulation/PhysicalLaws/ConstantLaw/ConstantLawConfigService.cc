// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "../ILawConfigurator.h"

#include "ConstantLawConfig_axl.h"

#include "ArcRes/Entities/System.h"
#include "ArcRes/XPath.h"

#include "ConstantLawType_law.h"
#include "ConstantLaw.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ConstantLawConfigService
  : public ArcaneConstantLawConfigObject
{
public:

  ConstantLawConfigService(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneConstantLawConfigObject(sbi) {}

  ~ConstantLawConfigService() {}

  void configure(Law::FunctionManager& function_mng, ArcRes::System& system);

private:

  ConstantLaw m_law;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
ConstantLawConfigService::
configure(Law::FunctionManager& function_mng, ArcRes::System& system)
{
  // Set Law Parameters
  m_law.setParameters(options()->parameters().value());

  // Set Law Arguments
  ConstantLawType::Signature signature;
  Gump::Property out = ArcRes::XPath::property(system, options()->output().result());
  signature.result = out;

  // Register Law
  function_mng << std::make_shared<ConstantLawType::Function>(
			  signature, m_law, &ConstantLaw::eval);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_CONSTANTLAWCONFIG(ConstantLawConfig,ConstantLawConfigService);
