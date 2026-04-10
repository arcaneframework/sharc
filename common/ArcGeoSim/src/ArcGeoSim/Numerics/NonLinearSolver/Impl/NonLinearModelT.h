#ifndef NONLINEARMODELT_H
#define NONLINEARMODELT_H
/* Author : gratienj at Mon Aug 25 2008
 * Interface for NonLinearModel
 */

#include "ArcGeoSim/Numerics/Utils/Norme.h"
#ifdef USE_ALIEN_V0
#include "ALIEN/Algo/ILinearSolver.h"
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
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSolver.h"
#endif

#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearModel.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearSystem.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearSystemBuilder.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearSystemVisitor.h"


#include "ArcGeoSim/Numerics/NonLinearSolver/Impl/NonLinearSystemT.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/Impl/NonLinearSystemBuilderT.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/Impl/VariableUpdaterT.h"

#if (ARCANE_VERSION >= 40105)
#include "alien/local_direct_solvers/ILinearSystemBuilder.h"
#else
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSystemBuilder.h"
#endif
//template<typename ModelType> class NonLinearSystemT ;
class INonLinearSystemBuilder;
//class ILinearSolver;

class BaseNonLinearModel
: public INonLinearModel
{
 public:
  typedef NonLinearSystemT<BaseNonLinearModel> NonLinearSystemType ;
  typedef ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L0> NormeL0 ;
  typedef ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L1> NormeL1 ;
  typedef ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L2> NormeL2 ;

#if (ARCANE_VERSION >= 40105)
  using ILinearSystemBuilderType = Alien::ILinearSystemBuilder;
  using IDirectLinearSolverType  = Alien::LocalDirectSolverNamespace::ILinearSolver ;
#else
  using ILinearSystemBuilderType = ILinearSystemBuilder;
  using IDirectLinearSolverType  = LocalDirectSolverNamespace::ILinearSolver ;
#endif

  BaseNonLinearModel() {
    m_non_linear_system = new NonLinearSystemType(this) ;
    m_non_linear_system_builder = new NonLinearSystemBuilderT<NonLinearSystemType>();
    m_variable_updater = new VariableUpdaterT<NonLinearSystemType>() ;
  }

  virtual ~BaseNonLinearModel() {}

  INonLinearSystem* getNonLinearSystem() {
    return m_non_linear_system ;
  }

  INonLinearSystemBuilder* getNonLinearSystemBuilder() {
    return m_non_linear_system_builder ;
  }
  virtual INonLinearSystemVisitor* getVariableUpdater() {
    return m_variable_updater ;
  }

  virtual void setLinearSolver(IDirectLinearSolverType* solver) = 0 ;

  virtual void setLinearSolver(Alien::ILinearSolver* solver) = 0 ;

  virtual ILinearSystemBuilderType* getLinearSystemBuilder(){
    return nullptr ;
  }

  virtual Integer buildLinearizedSystem() = 0 ;
  virtual Integer updateVariable() = 0 ;

  virtual Real getNewtonResidual(NormeL0& norme) = 0 ;
  virtual Real getNewtonResidual(NormeL1& norme) = 0 ;
  virtual Real getNewtonResidual(NormeL2& norme) = 0 ;
  virtual Integer getNumericalError() = 0 ;
 protected :
  NonLinearSystemType*      m_non_linear_system         = nullptr ;
  INonLinearSystemBuilder*  m_non_linear_system_builder = nullptr ;
  INonLinearSystemVisitor*  m_variable_updater          = nullptr;
} ;



template< typename ModelT>
class BaseNonLinearModelT
:  public BaseNonLinearModel
{
 public:

  typedef ModelT                                                      ModelType ;
  typedef BaseNonLinearModelT<ModelType>        ThisType ;

  /**
   * Types to manage input and output data, pre and post operators
   */
  typedef VisitorT<ModelType>                        Visitor ;




  /** Constructeur de la classe */
 BaseNonLinearModelT(ModelType * model,
		     String name)
   : BaseNonLinearModel()
    , m_initialized(false)
    , m_name(name)
    , m_model(model)
    {
    }

  /** Destructeur de la classe */
  virtual ~BaseNonLinearModelT() {}

 public:
  String getName() const {
    return m_name ;
  }

  void setLinearSolver(IDirectLinearSolverType* solver) {
    m_linear_solver = solver ;
  }


  void setLinearSolver(Alien::ILinearSolver* solver) {
    m_linear_solver2 = solver ;
  }

#ifdef USE_LINEARSOLVER_V1
  ILinearSystemBuilderType* getLinearSystemBuilder()  {
    return nullptr ;
  }
#endif

  Integer buildLinearizedSystem() {
    return m_model->buildLinearizedSystem() ;
  }

  Integer updateVariable() {
    return m_model->updateVariable() ;
  }

  Real getNewtonResidual(ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L0>& norme) {
    return m_model->getNewtonResidual(norme) ;
  }

  Real getNewtonResidual(ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L1>& norme) {
    return m_model->getNewtonResidual(norme) ;
  }

  Real getNewtonResidual(ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L2>& norme) {
    return m_model->getNewtonResidual(norme) ;
  }

  Integer getNumericalError() {
    return m_error ;
  }

 protected :
  bool m_initialized = false;
  String m_name ;

  ModelType*               m_model          = nullptr;
  IDirectLinearSolverType* m_linear_solver  = nullptr;
  Alien::ILinearSolver*    m_linear_solver2 = nullptr;
  Integer                  m_error          = 0;
};

#endif
