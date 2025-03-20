//#include "ArcGeoSim/Utils/Utils.h"
//
//#include <arcane/ArcaneVersion.h>
//
//
//#ifdef USE_ALIEN_V1
//#include "ALIEN/Expression/Solver/ILinearSolver.h"
//#include "ALIEN/Vector.h"
//#include "ALIEN/Externals/Accessors/ItemVectorAccessor.h"
//#endif
//#ifdef USE_ALIEN_V0
//#include "ALIEN/BaseObjects/Space.h"
//#include "ALIEN/BaseObjects/Matrix.h"
//#include "ALIEN/BaseObjects/Vector.h"
//#include "ALIEN/Algo/ILinearSolver.h"
//#include "ALIEN/Algo/BaseLinearSolver.h"
//#include "ALIEN/Algo/ILinearAlgebra.h"
//#include "ALIEN/Utils/SolverStats/SolverStat.h"
//#include "ALIEN/Utils/SolverStats/SolverStater.h"
//#include "ALIEN/Handlers/Accessor/VectorAccessor.h"
//#endif
//#include "ErrorEstimates/IStopCriteriaEstimator.h"
//
//#include "ErrorEstimates/AdaptiveLinearSolver/AdaptativeLinearSolverService.h"
//
//
//using namespace Arcane;
//
///*---------------------------------------------------------------------------*/
///*---------------------------------------------------------------------------*/
//
//AdaptativeLinearSolver::
//AdaptativeLinearSolver(const ServiceBuildInfo & sbi)
// : ArcaneAdaptativeLinearSolverObject(sbi)
// , IStopCriteriaEstimatorModel()
// , m_is_initialized(false)
// , m_solver(NULL)
// , m_stop_criteria_estimator(NULL)
//, m_max_iter(0)
//, m_step_nb_iter(0)
//, m_rtol(0)
//{
//  ;
//}
//
//
///*---------------------------------------------------------------------------*/
//
//AdaptativeLinearSolver::
//~AdaptativeLinearSolver()
//{
//  ;
//}
//
///*---------------------------------------------------------------------------*/
//
//void AdaptativeLinearSolver::init()
//{
//  if(m_is_initialized) return ;
//  m_stater.reset();
//  m_stater.startInitializationMeasure();
//
////#ifdef USE_ALIEN_V0
////  Alien::ILinearSolver* solver = options()->solver() ;
////  solver->init() ;
////  m_solver = dynamic_cast<Alien::BaseLinearSolver*>(solver) ;
////  if(m_solver==NULL)
////    fatal()<<"Only Parametrisable Linear Solver are allowed" ;
////#endif
////#ifdef USE_ALIEN_V1
////  m_solver =  options()->solver() ;
////  m_solver->init();
////#endif
//  m_solver =  options()->solver() ;
//  m_solver->init();
//  m_max_iter = options()->numIterationsMax() ;
//  m_rtol = options()->stopCriteriaValue() ;
//  m_gammaAlg = options()->stopCriteriaParameter() ;
//  m_step_nb_iter = options()->numIterationsByStep() ;
//  m_is_initialized = true ;
//  m_stater.stopInitializationMeasure();
//}
//
///*---------------------------------------------------------------------------*/
//
//void AdaptativeLinearSolver::end()
//{
//  m_solver->end() ;
//}
//
///*---------------------------------------------------------------------------*/
//
//
//bool
//AdaptativeLinearSolver::
//solve(MatrixType const& A, VectorType const& b, VectorType& x)
//{
//  m_stater.startSolveMeasure();
//  m_status.iteration_count = 0 ;
//  Real sp_estimate = 1.;//m_stop_criteria_estimator->GetGlobalSpatialEstimator();
//  cout<<"IFPSOLVER Print not useful"<<"\n";//, sp_estimate ="<<sp_estimate<<"\n";
//  if(sp_estimate < 1e-14){
//    bool conv = m_solver->solve(A,b,x) ;
//    Status const& step_status = m_solver->getStatus();
//    m_status.iteration_count += step_status.iteration_count ;
//    if(conv)
//    {
//      m_status.succeeded = true ;
//      m_status.residual = step_status.residual ;
//      m_stater.stopSolveMeasure(m_status);
//      return true ;
//    }
//    else{
//      m_status.succeeded = false ;
//      m_status.error = step_status.error ;
//      m_stater.stopSolveMeasure(m_status);
//      return false ;
//    }
//  }
//  else{
////#ifdef USE_ALIEN_V0
////    m_solver->setParameter<int>("max-it",m_step_nb_iter) ;
////    m_solver->setParameter<double>("rtol",m_rtol) ;
////    m_solver->notifyParamChangesObserver();
////#endif
////#ifdef USE_ALIEN_V1
////    throw Arcane::FatalErrorException("TODO");
////#endif
//    m_solver->setMaxIteration(m_step_nb_iter) ;
////    m_solver->setParameter<double>("rtol",m_rtol) ;
//    m_status.iteration_count = 0 ;
//    m_status.error = 0 ;
//    while(m_status.iteration_count<m_max_iter)
//    {
//      bool conv = m_solver->solve(A,b,x) ;
//      Status const& step_status = m_solver->getStatus() ;
//      m_status.iteration_count += step_status.iteration_count ;
//      if(conv)
//      {
//        m_status.succeeded = true ;
//        m_status.residual = step_status.residual ;
//        m_stater.stopSolveMeasure(m_status);
//        return true ;
//      }
//      else
//      {
//        if(step_status.iteration_count>=m_step_nb_iter)
//        {
//          bool stop = 0.;//m_stop_criteria_estimator->computeLinearStopCriteria(x, m_gammaAlg) ;
//          if(stop)
//          {
//            //       ********           Print Estimators           ***********//
//
////            m_solver->setParameter<int>("max-it",1) ;
////            m_solver->notifyParamChangesObserver();
////            VectorType vec_sp;
////            VectorType vec_alg;
////            VectorType vec_not_alg;
////            VectorType vec_not_sp;
////            VectorType vec_not_b;
////            VectorType vec_b;
////            m_stop_criteria_estimator->copyBinVecB(b,vec_b);
////            cout<<"IFPSOLVER Print A et b in second member\n";
////            m_stop_criteria_estimator->GetEstimatorsToPrint(vec_sp, vec_alg, vec_not_sp, vec_not_alg);
////            m_stop_criteria_estimator->copyBinVecB(vec_not_sp,vec_not_b);
////            bool printEst = m_solver->solve(A,vec_not_b,vec_b) ;
////            cout<<"IFPSOLVER Print estimators begin\n";
////            cout<<"IFPSOLVER Print ALG in second member\n";
////            printEst = m_solver->solve(A,vec_not_alg,vec_alg) ;
////            cout<<"IFPSOLVER Print SP in second member\n";
////            printEst = m_solver->solve(A,vec_not_sp,vec_sp) ;
////            cout<<"IFPSOLVER Print estimators end \n";
//
//            // ***************************************************************//
//            m_status.succeeded = true ;
//            m_status.residual = step_status.residual ;
//            m_stater.stopSolveMeasure(m_status);
//            return true ;
//          }
//        }
//        else
//        {
//          m_status.succeeded = false ;
//          m_status.error = step_status.error ;
//          m_stater.stopSolveMeasure(m_status);
//          return false ;
//        }
//      }
//    }
//  }
//  m_stater.stopSolveMeasure(m_status);
//  return false ;
//}
//
//
//const Alien::ILinearSolver::Status&
//AdaptativeLinearSolver::
//getStatus() const
//{
//  return m_status;
//}
//
///*---------------------------------------------------------------------------*/
//
//ARCANE_REGISTER_SERVICE_ADAPTATIVELINEARSOLVER(AdaptativeSolver,AdaptativeLinearSolver);
