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

#include "PengRobinsonLawConfig_axl.h"

//#include "Geoxim/Accessors/PhysicalSystemAccessor.h"

#include "ArcRes/Entities/System.h"
#include "ArcRes/XPath.h"

#include "PengRobinsonLawType_law.h"
#include "PengRobinsonLaw.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class PengRobinsonLawConfigService
        : public ArcanePengRobinsonLawConfigObject
    //, private Geoxim::PhysicalSystemAccessor
{
public:

    PengRobinsonLawConfigService(const Arcane::ServiceBuildInfo & sbi)
            : ArcanePengRobinsonLawConfigObject(sbi) {}

    ~PengRobinsonLawConfigService() {}

public:

    void configure(Law::FunctionManager& function_mng, ArcRes::System& system);

private:

    PengRobinsonLaw m_law;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
PengRobinsonLawConfigService::
configure(Law::FunctionManager& function_mng, ArcRes::System& system)
{
    // Set Law Parameters
    // None

    // Set Law Arguments

    PengRobinsonLawType::Signature signature;

    signature.density = ArcRes::XPath::property(system, options()->output().density());
    signature.pressure = ArcRes::XPath::property(system, options()->input().pressure());
    signature.saturation = ArcRes::XPath::property(system, options()->input().saturation());

    // Register Law
    /*if(options()->check())
    {
      PengRobinsonLawType::EpsilonDerivatives epsilon;
      epsilon.saturation = 1.e-6;
      auto f = std::make_shared<PengRobinsonLawType::DebugFunction>(
              signature, m_law, &PengRobinsonLaw::eval,
              epsilon, traceMng());
      function_mng << f;
    }
    else
    {*/
    function_mng << std::make_shared<PengRobinsonLawType::Function>(signature, m_law,
                                                                                 &PengRobinsonLaw::eval);
    //}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_PENGROBINSONLAWCONFIG(PengRobinsonLawConfig,PengRobinsonLawConfigService);
