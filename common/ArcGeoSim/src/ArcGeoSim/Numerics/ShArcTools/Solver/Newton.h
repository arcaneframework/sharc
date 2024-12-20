// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2024 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCNUM_NUMERICS_SOLVER_ALGORITHMS_NEWTON_H
#define ARCNUM_NUMERICS_SOLVER_ALGORITHMS_NEWTON_H
/* Author : desrozis at Tue Mar  4 10:33:53 2014
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
 * Patron de conception pour un Newton
 * NB : on pourrait aller plus loin dans l'abstraction d'algo...
 *      ici ce n'est finalement qu'une methode iterative
 *
 * La classe TemplateObject doit fournir les methodes suivantes :
 *
 * void newtonLinearize()        : linearisation du systeme non lineaire et calcul des residus
 * Arcane::Real newtonError()    : calcul de l'erreur des residus
 * void newtonAssemble()         : assemblage du systeme lineaire
 * Geoxim::Results newtonSolve() : resolution du systeme lineaire
 * void newtonUpdate()           : mise a jour des inconnues
 * void newtonBeforeSolve()      : etape avant la resolution du systeme lineaire
 * void newtonAfterSolve()       : etape apres la resolution du systeme lineaire
 *
 * Exemple :
 *
 * struct NonLinearModel
 * {
 *   void newtonLinearize() {
 *     derivative = 3 * x * x;
 *     residual = x * x * x;
 *   }
 *   Arcane::Real newtonError() { return Arcane::math::abs(residual); }
 *   void newtonAssemble() {}
 *   void newtonBeforeSolve() {}
 *   void newtonAfterSolve() {}
 *   Geoxim::Information newtonSolve() {
 *     dx = - residual / derivative;
 *     return Geoxim::Information::Converged();
 *   }
 *   void newtonUpdate() {
 *     x += dx;
 *   }
 *   Arcane::Real x;
 *   Arcane::Real dx;
 *   Arcane::Real residual;
 *   Arcane::Real derivative;
 * };
 *
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//#include "Geoxim/Numerics/Solver/Results.h"

#include <arcane/utils/TraceAccessor.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcNum::Algorithm {

  template<typename TemplateObject>
  class Newton
    : public Arcane::TraceAccessor
  {
  public:

    Newton(TemplateObject& object, Arcane::ITraceMng* trace)
      : Arcane::TraceAccessor(trace)
      , m_object(object)
      , m_iteration_max(10)
      , m_relative_tolerance(1e-5)
      , m_tolerance(1.e-7)
      , m_iteration(0)
      , m_verbose(true) {}

    ~Newton() {}

  public:

    void disableVerbosity() { m_verbose = false; }

    void parameterize(Arcane::Integer iteration_max,
                      Arcane::Real relative_tolerance,
                      Arcane::Real tolerance)
    {
      m_iteration_max = iteration_max;
      m_relative_tolerance = relative_tolerance;
      m_tolerance = tolerance;

      //m_results.reserve(m_iteration_max);
    }

    Arcane::Integer lastIteration() const { return m_iteration; }

    bool solve();

  private:

    TemplateObject& m_object;

    Arcane::Integer m_iteration_max;
    Arcane::Real m_relative_tolerance;
    Arcane::Real m_tolerance;

    Arcane::Integer m_iteration;
  
    bool m_verbose;
  
    bool m_results;
  };

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

  template<typename TemplateObject>
  bool
  Newton<TemplateObject>::
  solve()
  {
    //m_results.reset();

    m_iteration = 0;

    m_object.newtonLinearize();

    auto err_k = m_object.newtonError(), rel_err_k = 1.;

    const auto err_0 = (err_k > m_tolerance) ? err_k : 1.;

    //m_results.add(err_k, rel_err_k);

    if(m_verbose)
      info() << " *** Newton algorithm : ";

    if(m_verbose) {
      std::ios::fmtflags f(std::cout.flags());
      info() << "Newton Iteration [" << std::right << std::setw(2) << m_iteration << "] * Residual error = "
             << std::right << std::setw(15) << std::scientific << std::setprecision(10) << err_k
             << "  | relative error = "
             << std::right << std::setw(15) << std::scientific << std::setprecision(10) << rel_err_k;
      std::cout.flags(f);
    }

    while( (rel_err_k > m_relative_tolerance) && (m_iteration < m_iteration_max) ) {

      m_iteration ++;

      m_object.newtonAssemble();

      m_object.newtonBeforeSolve(m_iteration);

      const bool r = m_object.newtonSolve();

      if(!r) {
        if(m_verbose) {
          info() << " *** Newton error : non linear solver failed because linear solver didn't converge";
          //info() << " *** Newton error : status=" << r.status() << ", reason=" << r.errorReason();
        }
        return r;
      }

      m_object.newtonAfterSolve(m_iteration);
      
      bool NonPhysicalSolution = false;
      m_object.newtonUpdate(NonPhysicalSolution);
      if(NonPhysicalSolution){
        if(m_verbose)
          info() << " *** Newton error : non physical solution";
        //return m_results.failed(Results::NonPhysicalSolution);
        return false;
      }

      m_object.newtonLinearize();

      err_k = m_object.newtonError();

      rel_err_k = err_k / err_0;

      //m_results.add(err_k, rel_err_k);

      if(m_verbose) {
        std::ios::fmtflags f(std::cout.flags());
        info() << "Newton Iteration [" << std::setw(2) << m_iteration << "] * Residual error = "
               << std::right << std::setw(15) << std::scientific << std::setprecision(10) << err_k
               << "  | relative error = " << std::right << std::setw(15)
               << std::scientific << std::setprecision(10) << rel_err_k ;
        std::cout.flags(f);
      }

      if(err_k < m_tolerance) {
        if(m_verbose) {
          info() << " *** Newton converged with Tolerance Criteria [ ||err_k|| = " << err_k << " < " << m_tolerance << " ]";
        }
        return true;
      }
    }

    if(rel_err_k < m_relative_tolerance ) {
      if (m_verbose) {
        info() << " *** Newton converged with RelativeTolerance Criteria [ ||err_k|| = " << err_k << " ||err_0|| = "
               << err_0 << " ||err_k||/||err_0|| = " << rel_err_k << " < " << m_relative_tolerance << "]";
      }
      return true;
    }

    if(m_verbose) {
      info() << " *** Newton diverged : max iteration reached";
    }

    return false;
  }

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCNUM_NUMERICS_SOLVER_ALGORITHMS_NEWTON_H */
