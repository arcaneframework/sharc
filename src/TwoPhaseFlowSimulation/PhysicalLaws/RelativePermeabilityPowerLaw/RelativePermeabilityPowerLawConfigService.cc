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

#include "RelativePermeabilityPowerLawConfig_axl.h"

#include "ArcRes/Entities/System.h"
#include "ArcRes/XPath.h"

#include "RelativePermeabilityPowerLawType_law.h"
#include "RelativePermeabilityPowerLaw.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class RelativePermeabilityPowerLawConfigService
  : public ArcaneRelativePermeabilityPowerLawConfigObject
{
public:

  RelativePermeabilityPowerLawConfigService(const Arcane::ServiceBuildInfo & sbi)
: ArcaneRelativePermeabilityPowerLawConfigObject(sbi) {}

  ~RelativePermeabilityPowerLawConfigService() {}

public:

  void configure(Law::FunctionManager& function_mng, ArcRes::System& system);

private:

  RelativePermeabilityPowerLaw m_law;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
RelativePermeabilityPowerLawConfigService::
configure(Law::FunctionManager& function_mng, ArcRes::System& system)
{
  // Set Law Parameters
  m_law.setParameters(options()->parameters().Swi(),
      options()->parameters().Sgc(),
      options()->parameters().alpha() );

  // Set Law Arguments
  RelativePermeabilityPowerLawType::Signature signature;
  signature.relativePermeability = ArcRes::XPath::property(system, options()->output().relativePermeability());
  signature.saturation = ArcRes::XPath::property(system, options()->input().saturation());

  // Register Law
  function_mng << std::make_shared<RelativePermeabilityPowerLawType::Function>(signature, m_law,
      &RelativePermeabilityPowerLaw::eval);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_RELATIVEPERMEABILITYPOWERLAWCONFIG(RelativePermeabilityPowerLawConfig,RelativePermeabilityPowerLawConfigService);
