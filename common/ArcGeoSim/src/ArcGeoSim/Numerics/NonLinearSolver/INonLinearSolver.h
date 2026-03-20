// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_NUMERICS_NONLINEARSOLVER_INONLINEARSOLVER_H
#define ARCGEOSIM_NUMERICS_NONLINEARSOLVER_INONLINEARSOLVER_H

/**
  * \author Jean-Marc GRATIEN
  * \version 1.0
  * \brief Interface du service de resolution non lineaire.
  *
  * Ce service � pour but de r�soudre un syst�me d'�quation non lin�aire
  * de type IConservativeEquationSystem
  *
  * Il utilise des op�rateurs permettant :
  * - de lin�ariser le syst�me : INonLinearSystemBuilder ;
  * - de construire un syt�me lin�aire de type Ax=b ;
  * - de r�cup�rer la solution de syst�me lin�aire Ax=b.
  *
  * Il utilise �galement :
  * - un solveur lin�aire de type ILinearSolver ;
  * - un crit�re d'arr�t.
  *
 */
#include <arcane/ArcaneVersion.h>
#ifdef USE_ALIEN_V0
#include <ALIEN/Algo/ILinearSolver.h>
#endif
#ifdef USE_ALIEN_V1
#include <ALIEN/Expression/Solver/ILinearSolver.h>
#endif
#ifdef USE_ALIEN_V2
#include <alien/expression/solver/ILinearSolver.h>
#endif

#if (ARCANE_VERSION >= 40105)
#include "alien/local_direct_solvers/IBaseLinearSolver.h"
#else
#include "ArcGeoSim/Numerics/NonLinearSolver/LocalDirectSolverImpl/ILinearSolver.h"
#endif

class INonLinearSystem ;
class INonLinearSystemVisitor ;
class INonLinearSystemBuilder ;
class INonLinearStopCriteria ;

#if (ARCANE_VERSION >= 40105)
namespace Alien
{
class ILinearSystemBuilder ;
class ILinearSystemVisitor ;

}
#else
class ILinearSystemBuilder ;
class ILinearSystemVisitor ;
#endif


class INonLinearSolver
{
public:

  struct Status
  {
    bool succeeded;
    Real residual;
    Integer iteration_count;
    Integer error ;
  };

#if (ARCANE_VERSION >= 40105)
  using IDirectSolverType        = Alien::LocalDirectSolverNamespace::ILinearSolver;
  using ILinearSystemBuilderType = Alien::ILinearSystemBuilder;
#else
  using IDirectSolverType        = LocalDirectSolverNamespace::ILinearSolver;
  using ILinearSystemBuilderType = ILinearSystemBuilder;
#endif

  /** Constructeur de la classe */
  INonLinearSolver()
    {
      ;
    }

  /** Destructeur de la classe */
  virtual ~INonLinearSolver() { }

public:

  //! Initialisation
  virtual void init() = 0;

  //! Setting non linear system builder
  virtual void setNonLinearSystemBuilder(INonLinearSystemBuilder * non_linear_builder) = 0;

  //! Setting linear solver
  virtual void setLinearSolver(IDirectSolverType * solver) = 0;
  virtual void setLinearSolver(Alien::ILinearSolver * solver) = 0;

  //! Setting linear system builder
  virtual void setLinearSystemBuilder(ILinearSystemBuilderType * linear_builder) = 0;

  //! Setting variable updater
  virtual void setVariableUpdater(INonLinearSystemVisitor * visitor) = 0;

  //! Setting stop crit�ria
  virtual void setStopCriteria(INonLinearStopCriteria * stop_criteria) = 0;

  //! Solving non linear system return true if ok, false if not ok
  virtual bool solve(INonLinearSystem * system) = 0;

  //! Optimized version for local litle non linear system
  virtual bool solveOpt(INonLinearSystem * system) = 0;

  //! Initializing non linear system
  virtual void start(INonLinearSystem * system) = 0;

  //! Finalizing non linear system
  virtual void end(INonLinearSystem * system) = 0;

  virtual const Status & getStatus() const = 0;
};

#endif /* ARCGEOSIM_NUMERICS_NONLINEARSOLVER_INONLINEARSOLVER_H */
