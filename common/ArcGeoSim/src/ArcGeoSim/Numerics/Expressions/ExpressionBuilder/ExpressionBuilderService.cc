// -*- C++ -*-
#include <arcane/ArcaneVersion.h>
#include "ExpressionBuilderService.h"

#include "ExpressionBuilderR1vR1Core.h"
#include "ExpressionBuilderR2vR1Core.h"
#include "ExpressionBuilderR3vR1Core.h"
#include "ExpressionBuilderR4vR1Core.h"

#include "ArcGeoSim/Appli/IAppServiceMng.h"

#include "ArcGeoSim/Numerics/Expressions/IExpressionMng.h"

#include <arcane/utils/NotImplementedException.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
ExpressionBuilderService::
init()
{
  if (m_initialized) return;
  
  // Recherche de IExpressionMng
  IAppServiceMng* app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());
  IExpressionMng * expression_mng = app_service_mng->find<IExpressionMng>(false) ;

  // Initialisation du parser
  m_function_parser.init(expression_mng,&m_expression_mng,traceMng());
  
  // Lecture des constantes
  const Arcane::Integer nbConstant = options()->constant.size();
  for(Arcane::Integer iconstant=0; iconstant < nbConstant; ++iconstant) {
    m_expression_mng.setConstant(options()->constant[iconstant].name(),
                                 options()->constant[iconstant].value());
    info() << "Loading local constant " << options()->constant[iconstant].name();
  }
  
  // Parsing de l'expression
  m_function_parser.parseString(options()->expression());

  // Out Dimension
  Arcane::Integer out_dimension = getOutDimension();
  
  if(out_dimension != 1) {
    throw NotImplementedException("ExpressionBuilder cannot manage out dimension greater than 1");
  }
  
  // resize result container
  m_result.resize(out_dimension);
  m_result.fill(NULL);
  
  // Cast interne suivant le nombre de variables
  
  if(getInDimension() == 1 && out_dimension == 1)
      m_internal = new ExpressionBuilderR1vR1Core(&m_function_parser);
  
  if(getInDimension() == 2 && out_dimension == 1)
    m_internal = new ExpressionBuilderR2vR1Core(&m_function_parser);
  
  if(getInDimension() == 3 && out_dimension == 1)
    m_internal = new ExpressionBuilderR3vR1Core(&m_function_parser);
  
  if(getInDimension() == 4 && out_dimension == 1)
    m_internal = new ExpressionBuilderR4vR1Core(&m_function_parser);
  
  m_initialized = true;
}

/*---------------------------------------------------------------------------*/

Arcane::Integer
ExpressionBuilderService::
getInDimension() const
{
  return m_function_parser.getNbVariable();
}

/*---------------------------------------------------------------------------*/

Arcane::Integer
ExpressionBuilderService::
getOutDimension() const
{
  return 1;
}

/*---------------------------------------------------------------------------*/

void 
ExpressionBuilderService::
setParameter(const Arcane::String & name, const Arcane::Real & value)
{
  m_function_parser.setParameter(name, value);
}

/*---------------------------------------------------------------------------*/

void 
ExpressionBuilderService::
setParameter(Arcane::Integer index, const Arcane::Real & value)
{
  Arcane::String name = m_function_parser.getParameter(index);

  m_function_parser.setParameter(name, value);
}

/*---------------------------------------------------------------------------*/

Arcane::Integer
ExpressionBuilderService::
nbParameter() const
{
  return m_function_parser.getNbParameter();
}

/*---------------------------------------------------------------------------*/

Arcane::String
ExpressionBuilderService::
parameterName(Arcane::Integer index) const
{
  return m_function_parser.getParameter(index);
}

/*---------------------------------------------------------------------------*/

void
ExpressionBuilderService::
setVariable(const Arcane::String & name, const Arcane::Array<Arcane::Real> & variable)
{
  m_function_parser.setVariable(name, variable.unguardedBasePointer(),variable.size());
}

/*---------------------------------------------------------------------------*/

void
ExpressionBuilderService::
setVariable(Arcane::Integer index, const Arcane::Array<Arcane::Real> & variable)
{
  Arcane::String variable_name = m_function_parser.getVariable(index);

  m_function_parser.setVariable(variable_name, variable.unguardedBasePointer(),variable.size());
}

/*---------------------------------------------------------------------------*/

Arcane::Integer
ExpressionBuilderService::
nbVariable() const
{
  return m_function_parser.getNbVariable();
}

/*---------------------------------------------------------------------------*/

Arcane::String 
ExpressionBuilderService::
variableName(Integer index) const
{
  return m_function_parser.getVariable(index);
}

/*---------------------------------------------------------------------------*/

void
ExpressionBuilderService::
setEvaluationResult(Arcane::Array<Arcane::Real> & result)
{
  m_function_parser.setEvaluationResult(result.unguardedBasePointer(), result.size());
}

/*---------------------------------------------------------------------------*/

void
ExpressionBuilderService::
setDerivationResult(Arcane::Integer di, Arcane::Array<Arcane::Real> & result)
{
  fatal() << "Derivation not implemented";
}

/*---------------------------------------------------------------------------*/

void
ExpressionBuilderService::
setDerivationResult(const Arcane::String & di, Arcane::Array<Arcane::Real> & result)
{
  fatal() << "Derivation not implemented";
}

/*---------------------------------------------------------------------------*/

void 
ExpressionBuilderService::
eval()
{
  m_function_parser.eval();
}

/*---------------------------------------------------------------------------*/

void 
ExpressionBuilderService::
cleanup()
{
  m_function_parser.cleanup();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_EXPRESSIONBUILDER(ExpressionBuilder,ExpressionBuilderService);
