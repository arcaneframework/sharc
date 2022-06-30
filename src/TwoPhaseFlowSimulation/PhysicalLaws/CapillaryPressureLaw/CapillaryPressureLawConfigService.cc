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

#include "CapillaryPressureLawConfig_axl.h"

//#include "Geoxim/Accessors/PhysicalSystemAccessor.h"

#include "ArcRes/Entities/System.h"
#include "ArcRes/XPath.h"

#include "CapillaryPressureLawType_law.h"
#include "CapillaryPressureLaw.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class CapillaryPressureLawConfigService
        : public ArcaneCapillaryPressureLawConfigObject
    //, private Geoxim::PhysicalSystemAccessor
{
public:

    CapillaryPressureLawConfigService(const Arcane::ServiceBuildInfo & sbi)
            : ArcaneCapillaryPressureLawConfigObject(sbi) {}

    ~CapillaryPressureLawConfigService() {}

public:

    void configure(Law::FunctionManager& function_mng, ArcRes::System& system);

private:

    CapillaryPressureLaw m_law;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
CapillaryPressureLawConfigService::
configure(Law::FunctionManager& function_mng, ArcRes::System& system)
{
    // Set Law Parameters

    m_law.setParameters(options()->parameters().Pe(),
                        options()->parameters().SrRef(),
                        options()->parameters().Sr(),
                        options()->parameters().lambda() );

    // Set Law Arguments

    CapillaryPressureLawType::Signature signature;

    signature.capillaryPressure = ArcRes::XPath::property(system, options()->output().capillaryPressure());
    signature.saturation = ArcRes::XPath::property(system, options()->input().saturation());

    // Register Law
    /*if(options()->check())
    {
      CapillaryPressureLawType::EpsilonDerivatives epsilon;
      epsilon.saturation = 1.e-6;
      auto f = std::make_shared<CapillaryPressureLawType::DebugFunction>(
              signature, m_law, &CapillaryPressureLaw::eval,
              epsilon, traceMng());
      function_mng << f;
    }
    else
    {*/
    function_mng << std::make_shared<CapillaryPressureLawType::Function>(signature, m_law,
                                                                                 &CapillaryPressureLaw::eval);
    //}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_CAPILLARYPRESSURELAWCONFIG(CapillaryPressureLawConfig,CapillaryPressureLawConfigService);
