// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_PHYSICS_LAW_FUNCTIONMANAGER_H
#define ARCGEOSIM_PHYSICS_LAW_FUNCTIONMANAGER_H
/* Author : desrozis at Tue Oct 30 16:51:25 2012
 * Generated by createNew
 */

/*
 * \ingroup Law
 * \brief Base de donnees de fonctions
 *
 * Voir les fonctions de remplissage dans Function.h
 * Les algorithmes doivent etre retenus en memoire par l'application
 * 
 * \code
 *
 * FunctionManager function_mng;
 * 
 * ExampleAlgo algo();
 * ExampleAlgo::Signature signature;
 *
 * // Remplissage de la signature
 * ...
 *
 * function_mng << createFunction(0, algo, signature);
 *
 * info() << function_mng;
 *
 * const IFunction& f = function_mng[0]; 
 * 
 * info() << f;
 *
 * \endcode
 *
 */

#include "ArcGeoSim/Physics/Law2/FunctionGraph.h"

#include <map>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
 * \ingroup Law
 * \brief Classe de stockage des fonctions
 *
 * Met e disposition les dependances par proprietes
 *
 */

class FunctionManager
{
private:

  typedef std::shared_ptr<IFunction> FunctionPtr;
  typedef Arcane::SharedArray<FunctionPtr> FunctionVector;
  typedef std::map<Property, PropertyVector> DependenciesMap;

public:

  //! Enumerateur de fonctions
  class Enumerator
  {
  public:
    Enumerator(const FunctionVector& f)
      : m_functions(f)
      , m_current(m_functions.begin())
      , m_end(m_functions.end())
      , m_count(0) {}
    Enumerator(const Enumerator& e)
      : m_functions(e.m_functions)
      , m_current(e.m_current)
      , m_end(e.m_end)
      , m_count(e.m_count){}
    void reset() { m_current = m_functions.begin(); m_count = 0; }
    bool hasNext() const { return m_current != m_end; }
    void operator++() { m_current++; m_count++; }
    const IFunction& function() const { return **m_current; }
    IFunction& non_const_function() const { return **m_current; }
    Integer functionId() const { return (*m_current)->id(); }
    Integer count() const { return m_count; }
    Integer size() const { return m_functions.size(); }
  private:
    FunctionVector m_functions;
    FunctionVector::const_iterator m_current;
    FunctionVector::const_iterator m_end;
    Integer m_count;
  };

  FunctionManager(bool fatal_if_unregistered = false, bool verbose = true, DerivativesMode dependencies_mode = eDirect);

  virtual ~FunctionManager(){}

  //! Enclenche le mode verbose
  void enableVerbosity() { m_verbose = true; }

  //! Enclenche le mode silencieux
  void disableVerbosity() { m_verbose = false; }

  //! Enregistrement d'une fonction
  FunctionManager& operator<<(FunctionPtr f);

  //! Enregistrement d'une fonction
  void registerOrReplace(FunctionPtr f);

  //! Accession d'une fonction par son identifiant unique
  bool contains(const Property& p) const;

  //! Accession d'une fonction par son identifiant unique
  const IFunction& operator[](Integer i) const;

  //! Accession d'une fonction calculant une propriete
  const IFunction& operator[](const Property& p) const;

  //! Nombre de fonctions
  inline Integer size() const { return static_cast<Arcane::Integer>(m_functions.size()); }

  //! Enumerateur de fonction
  Enumerator functions() const ;

  //! Enumerateur de fonction parametres
  Enumerator parameters() const ;

  //! Enumerateur de fonction
  Enumerator functions(Arcane::IntegerConstArrayView ids) const;

  bool isRoot(const IFunction&  f) const { return dependencies(f) == f.in(); }

  //! Dependances d une propriete
  const PropertyVector& dependencies(const Property& p) const { return m_dependencies[p]; }

  //! Dependances d une fonction
  const PropertyVector& dependencies(const IFunction& f) const { return dependencies(f.out()[0]); }

  //! Dependances des proprietes composees d une fonction
  const std::map<Law::Property, Law::PropertyVector> dependenciesGraph(const IFunction& f) const ;

  //! Recalcule des dependances des proprietes
  void recomputeDependencies() const;

  //! Ecrit le graphe au format .dot (complet si verbeux sinon plus proche implementation plutot pour debug)
  void printDotGraph(std::ostream& nout, const bool verbose = true) const;

private:

  //! Recalcule des dependances des proprietes si besoin
  void _recomputeDependenciesIfRequired() const;

  //! Stragtegie si fonction absente mais demandee
  bool m_fatal_if_unregistered;

  //! Verbosity
  bool m_verbose;

  //! Stragtegie pour le calcul de dependances
  DerivativesMode m_dependencies_mode;

  //! Manager de trace
  Arcane::ITraceMng* m_trace;

  //! Graphe de Fonctions
  FunctionGraph m_functions;

  //! Ensemble des fonctions triees
  mutable FunctionVector m_functions_sort;

  //! Ensemble des fonctions triees
  mutable FunctionVector m_parameters_sort;


  //! Dependances par proprietes
  mutable DependenciesMap m_dependencies;

  //! Fonctions parametres (evaluees en premier et sans derivees)
  FunctionGraph m_parameters;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

std::ostream& operator<<(std::ostream& nout, const FunctionManager& f);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_PHYSICS_LAW_FUNCTIONTREE_H */
