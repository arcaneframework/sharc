// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "FunctionManager.h"
#include <arcane/ISubDomain.h>
#include <ArcGeoSim/Physics/Law2/FunctionGraphPrinter.h>
/* Author : desrozis at Tue Nov  6 12:32:59 2012
 * Generated by createNew
*/

extern "C" Arcane::ISubDomain* _arcaneGetDefaultSubDomain();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

FunctionManager::
FunctionManager(bool fatal_if_unregistered, bool verbose, DerivativesMode dependencies_mode)
: m_fatal_if_unregistered(fatal_if_unregistered)
, m_verbose(verbose)
, m_dependencies_mode(dependencies_mode)
, m_trace(_arcaneGetDefaultSubDomain()->traceMng())
, m_functions(fatal_if_unregistered, m_trace)
, m_parameters(fatal_if_unregistered, m_trace){}

/*---------------------------------------------------------------------------*/

FunctionManager&
FunctionManager::
operator<<(FunctionPtr f)
{
  if(f->isParameter())
    m_parameters.add(f);
  else
    m_functions.add(f);
  // modification du graphe les dependances doivent etre recalculees
  m_dependencies.clear();
  return *this;
}

/*---------------------------------------------------------------------------*/

void
FunctionManager::
registerOrReplace(FunctionPtr f)
{
  const Integer id = f->id();
  if(m_functions.find(id) != m_functions.end())
    m_functions.replace(f);
  else if(m_parameters.find(id) != m_parameters.end())
    m_parameters.replace(f);
  else{
    *this<<f;
    recomputeDependencies();
  }
}


/*---------------------------------------------------------------------------*/

const IFunction&
FunctionManager::
operator[](Integer i) const
{
  const auto it_func = m_functions.find(i);
  if(it_func != m_functions.end())
    return *it_func;
  const auto it_param = m_parameters.find(i);
  if(it_param != m_parameters.end())
    return *it_param;
  throw Arcane::String::format("Function with id='{0}' is not registered", i);
}

/*---------------------------------------------------------------------------*/

bool
FunctionManager::
contains(const Property& p) const
{
  const auto it_func = m_functions.find(p);
  const auto it_param = m_parameters.find(p);
  return ((it_func != m_functions.end()) || (it_param != m_parameters.end())) ;
}

/*---------------------------------------------------------------------------*/

const IFunction&
FunctionManager::
operator[](const Property& p) const
{
  const auto it_func = m_functions.find(p);
  if(it_func != m_functions.end())
    return *it_func;
  const auto it_param = m_parameters.find(p);
  if(it_param != m_parameters.end())
    return *it_param;
  throw Arcane::String::format("Function to compute property '{0}' is not registered", p.name());
}

/*---------------------------------------------------------------------------*/

FunctionManager::Enumerator
FunctionManager::
functions(IntegerConstArrayView idx_output) const
{
  _recomputeDependenciesIfRequired();
  const bool recursive = (m_dependencies_mode!=eDirect);
  return m_functions.toSortVector(idx_output, recursive);
}

/*---------------------------------------------------------------------------*/

FunctionManager::Enumerator
FunctionManager::
functions() const
{
  _recomputeDependenciesIfRequired();
  return m_functions_sort;
}

/*---------------------------------------------------------------------------*/

FunctionManager::Enumerator
FunctionManager::
parameters() const {
  _recomputeDependenciesIfRequired();
  return m_parameters_sort;
}

/*---------------------------------------------------------------------------*/

const std::map<Law::Property, Law::PropertyVector>
FunctionManager::
dependenciesGraph(const IFunction& f) const {
  std::map<Law::Property, Law::PropertyVector> composed_prop_dependencies;
  PropertyVector in_prop = f.in();
  PropertyVector graph_dependencies = dependencies(f);
  for(int i_prop = 0; i_prop < in_prop.size(); ++i_prop){
    auto current_prop = in_prop[i_prop];
    if(!graph_dependencies.contains(current_prop))
      composed_prop_dependencies.insert(std::make_pair(current_prop, m_dependencies[current_prop]));
  }
  return composed_prop_dependencies;
}

/*---------------------------------------------------------------------------*/

void
FunctionManager::
recomputeDependencies() const
{
  m_dependencies.clear();
  m_dependencies = m_functions.computeDependencies(m_dependencies_mode);
  auto dependencies_param = m_parameters.computeDependencies();
  m_dependencies.insert(dependencies_param.begin(),dependencies_param.end());
  m_functions_sort = m_functions.toSortVector();
  m_parameters_sort = m_parameters.toSortVector();
}

void
FunctionManager::
_recomputeDependenciesIfRequired() const
{
  if(m_dependencies.empty())
    recomputeDependencies();
}

void
FunctionManager::
printDotGraph(std::ostream& nout, const bool verbose) const
{
  if(verbose){
    const FunctionVector functions = m_functions.toSortVector();
    FunctionGraphPrinter::printDotGraph(functions, nout);
  }
  else{
    m_functions.printDotGraph(nout);
  }
}

/*---------------------------------------------------------------------------*/

std::ostream& operator<<(std::ostream& nout, const FunctionManager& f)
{
  nout << "Function manager info :";
  if(f.size() == 0) {
    nout << " {empty}";
    return nout;
  }
  for(auto e = f.functions(); e.hasNext(); ++e)
    nout << "\n" << e.function();

  for(auto p = f.parameters(); p.hasNext(); ++p)
    nout << "\n" << p.function();
  return nout;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
