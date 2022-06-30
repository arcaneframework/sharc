// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "../IInitialCondition.h"

#include "ArcGeoSim/Numerics/Expressions/ExpressionBuilder/ExpressionBuilderR3vR1Core.h"

#include "Expression_axl.h"

#include "ArcRes/Entities/System.h"
#include "ArcRes/XPath.h"
#include "ArcGeoSim/Appli/AppService.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ExpressionService
  : public ArcaneExpressionObject
{
public:
  
  ExpressionService(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneExpressionObject(sbi) {}
  
  ~ExpressionService() {}
  
public:

  void init(ArcRes::System& system, Law::VariableCellFolder& domain);

  void revaluate(ArcRes::System& system, Law::VariableCellFolder& domain);

private:

  void _evaluate(ArcRes::System& system, Law::VariableCellFolder& domain) ;

private:

  Gump::ScalarRealProperty m_property;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
ExpressionService::
init(ArcRes::System& system, Law::VariableCellFolder& domain)
{
  _evaluate(system, domain);
}

/*---------------------------------------------------------------------------*/

void 
ExpressionService::
revaluate(ArcRes::System& system, Law::VariableCellFolder& domain)
{
  _evaluate(system, domain) ;
}

/*---------------------------------------------------------------------------*/

void 
ExpressionService::
_evaluate(ArcRes::System& system, Law::VariableCellFolder& domain){
  ArcGeoSim::AppService<IGeometryMng> geometryMng ;

  IGeometryMng::Real3Variable cellCenters =
      geometryMng -> getReal3VariableProperty(allCells(), IGeometryProperty::PCenter) ;

  m_property = ArcRes::XPath::scalarRealProperty(system, options() -> property()) ;

  auto variable = Law::values(domain,m_property) ;

  const Arcane::ItemGroup& g(variable.itemGroup()) ;

  if(g.itemKind() != Arcane::IK_Cell)
    fatal() << "Needs a cell group" ;

  const auto& condition(options() -> condition()) ;
  condition -> init() ;

  ENUMERATE_CELL(icell, g) {
    Arcane::Real3& center(cellCenters[icell]) ;
    variable[icell] = condition -> eval(center[0], center[1], center[2]) ;
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_EXPRESSION(Expression,ExpressionService);
