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

#include "FluidDensityLawConfig_axl.h"

//#include "Geoxim/Accessors/PhysicalSystemAccessor.h"

#include "ArcRes/Entities/System.h"
#include "ArcRes/XPath.h"

#include "FluidDensityLawType_law.h"
#include "FluidDensityLaw.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class FluidDensityLawConfigService
        : public ArcaneFluidDensityLawConfigObject
    //, private Geoxim::PhysicalSystemAccessor
{
public:

    FluidDensityLawConfigService(const Arcane::ServiceBuildInfo & sbi)
            : ArcaneFluidDensityLawConfigObject(sbi) {}

    ~FluidDensityLawConfigService() {}

public:

    void configure(Law::FunctionManager& function_mng, ArcRes::System& system);

private:

    FluidDensityLaw m_law;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
FluidDensityLawConfigService::
configure(Law::FunctionManager& function_mng, ArcRes::System& system)
{
    // Set Law Parameters

    m_law.setParameters(options()->parameters().rho0(),
                        options()->parameters().betat(),
                        options()->parameters().betac(),
                        options()->parameters().t0(),
                        options()->parameters().c0());

    // Set Law Arguments

    FluidDensityLawType::Signature signature;

    signature.fluidDensity = ArcRes::XPath::property(system, options()->output().fluidDensity());
    signature.concentration = ArcRes::XPath::property(system, options()->input().concentration());
    signature.temperature = ArcRes::XPath::property(system, options()->input().temperature());

    function_mng << std::make_shared<FluidDensityLawType::Function>(signature, m_law,
                                                                                 &FluidDensityLaw::eval);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_FLUIDDENSITYLAWCONFIG(FluidDensityLawConfig,FluidDensityLawConfigService);
