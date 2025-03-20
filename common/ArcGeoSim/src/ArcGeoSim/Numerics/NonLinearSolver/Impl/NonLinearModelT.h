#ifndef NONLINEARMODELT_H
#define NONLINEARMODELT_H
/* Author : gratienj at Mon Aug 25 2008
 * Interface for NonLinearModel
 */

#include "ArcGeoSim/Numerics/Utils/Norme.h"
#ifdef USE_ALIEN_ARCGEOSIM
#include "ArcGeoSim/Numerics/LinearAlgebra2/ILinearSolver.h"
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSolver.h"
//using namespace Alien;
#endif
#ifdef USE_ALIEN_V0
#include "ALIEN/Algo/ILinearSolver.h"
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSolver.h"
#endif
#ifdef USE_ALIEN_V1
#include <ALIEN/Expression/Solver/ILinearSolver.h>
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSolver.h"
#endif
#ifdef USE_ALIEN_V2
#include <alien/expression/solver/ILinearSolver.h>
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSolver.h"
#endif
#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearModel.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearSystem.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearSystemBuilder.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/INonLinearSystemVisitor.h"


#include "ArcGeoSim/Numerics/NonLinearSolver/Impl/NonLinearSystemT.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/Impl/NonLinearSystemBuilderT.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/Impl/VariableUpdaterT.h"

#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSystemBuilder.h"
//template<typename ModelType> class NonLinearSystemT ;
class INonLinearSystemBuilder;
class ILinearSystemBuilder;
//class ILinearSolver;

class BaseNonLinearModel 
: public INonLinearModel
{
 public:
  typedef NonLinearSystemT<BaseNonLinearModel> NonLinearSystemType ;
  typedef ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L0> NormeL0 ;
  typedef ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L1> NormeL1 ;
  typedef ArcGeoSim::Numerics::Norme<ArcGeoSim::Numerics::L2> NormeL2 ;

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

#ifdef USE_ALIEN_ARCGEOSIM
  virtual void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) = 0 ;  
#endif
#ifdef USE_ALIEN_V0
  virtual void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) = 0 ;  
#endif
#ifdef USE_ALIEN_V1
  virtual void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) = 0 ;  
#endif
#ifdef USE_ALIEN_V2
  virtual void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) = 0 ;
#endif
  virtual void setLinearSolver(Alien::ILinearSolver* solver) = 0 ;

  virtual ILinearSystemBuilder* getLinearSystemBuilder(){
    return NULL ;
  } 
  
  virtual Integer buildLinearizedSystem() = 0 ;
  virtual Integer updateVariable() = 0 ;
  
  virtual Real getNewtonResidual(NormeL0& norme) = 0 ;
  virtual Real getNewtonResidual(NormeL1& norme) = 0 ;
  virtual Real getNewtonResidual(NormeL2& norme) = 0 ;
  virtual Integer getNumericalError() = 0 ;
 protected :
  NonLinearSystemType*      m_non_linear_system  ;
  INonLinearSystemBuilder*  m_non_linear_system_builder  ;
  INonLinearSystemVisitor*  m_variable_updater ;
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
#ifdef USE_ALIEN_ARCGEOSIM
    , m_linear_solver(NULL)
#endif
#ifdef USE_ALIEN_V0
    , m_linear_solver(NULL)
#endif
#ifdef USE_ALIEN_V1
    , m_linear_solver(NULL)
#endif
#ifdef USE_ALIEN_V2
    , m_linear_solver(NULL)
#endif
    , m_linear_solver2(NULL)
    , m_error(0)
    {
    }

  /** Destructeur de la classe */
  virtual ~BaseNonLinearModelT() {}

 public:
  String getName() const {
    return m_name ;
  }

#ifdef USE_ALIEN_ARCGEOSIM
  void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) {
    m_linear_solver = solver ;
  }
#endif

#ifdef USE_ALIEN_V0
  void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) {
    m_linear_solver = solver ;
  }
#endif
#ifdef USE_ALIEN_V1
  void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) {
    m_linear_solver = solver ;
  }
#endif
#ifdef USE_ALIEN_V2
  void setLinearSolver(LocalDirectSolverNamespace::ILinearSolver* solver) {
    m_linear_solver = solver ;
  }
#endif

  void setLinearSolver(Alien::ILinearSolver* solver) {
    m_linear_solver2 = solver ;
  }

#ifdef USE_LINEARSOLVER_V1
  ILinearSystemBuilder* getLinearSystemBuilder()  {
    return NULL ;
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
  bool m_initialized ;
  String m_name ;

  ModelType*     m_model ;
#ifdef USE_ALIEN_ARCGEOSIM
  LocalDirectSolverNamespace::ILinearSolver* m_linear_solver;
#endif
#ifdef USE_ALIEN_V0
  LocalDirectSolverNamespace::ILinearSolver* m_linear_solver;
#endif
#ifdef USE_ALIEN_V1
  LocalDirectSolverNamespace::ILinearSolver* m_linear_solver;
#endif
#ifdef USE_ALIEN_V2
  LocalDirectSolverNamespace::ILinearSolver* m_linear_solver;
#endif
  Alien::ILinearSolver* m_linear_solver2;
  Integer             m_error ;
};

#endif
