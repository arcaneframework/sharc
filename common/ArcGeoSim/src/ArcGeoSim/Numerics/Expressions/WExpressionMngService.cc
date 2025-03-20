// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "WExpressionMngService.h"

using namespace Arcane;

#include "IFunction.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
WExpressionMngService::
init()
{
  const Integer nbConstant = options()->constant.size();
  for(Integer iconstant=0; iconstant < nbConstant; ++iconstant)
    {
      const CaseOptionsWExpressionMng::CaseOptionConstantTypeValue & constant = options()->constant[iconstant];
      m_constant_table[constant.name()] = constant.value();
      info() << "Loading constant " << constant.name();
    }

  const Integer nbFunction = options()->function.size();
  for(Integer ifunction=0; ifunction < nbFunction; ++ifunction)
    {
      const CaseOptionsWExpressionMng::CaseOptionFunctionTypeValue & function = options()->function[ifunction];
      m_function_table[function.name()] = function.value();
      info() << "Loading function " << function.name();
    }
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_WEXPRESSIONMNG(WExpressionMng,WExpressionMngService);
