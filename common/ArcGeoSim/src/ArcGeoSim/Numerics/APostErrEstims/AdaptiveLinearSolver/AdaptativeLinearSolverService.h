#ifndef ADAPTATIVESOLVERIMPL_H
#define ADAPTATIVESOLVERIMPL_H


#ifdef USE_ALIEN_ARCGEOSIM
#include "ArcGeoSim/Numerics/LinearAlgebra2/ILinearSolver.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/IIndexManager.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Builder/DirectMatrixBuilder.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Utils/AsciiDumper.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Impl/SolverStater.h"
#endif
#ifdef USE_ALIEN_V1
#include <ALIEN/Alien-IFPEN.h>
#include <ALIEN/Kernels/PETSc/IO/AsciiDumper.h>
#include <ALIEN/Expression/Solver/SolverStats/SolverStater.h>
#endif

#include "AdaptativeLinearSolver_axl.h"

/**
 * Interface du service de r�solution de syst�me lin�aire
 */
 
class AdaptativeLinearSolver
 : public ArcaneAdaptativeLinearSolverObject
 , public IStopCriteriaEstimatorModel
{
public:
  typedef Alien::ILinearSolver::Status Status ;
#ifdef USE_ALIEN_V1
  typedef Alien::Matrix MatrixType ;
  typedef Alien::Vector VectorType ;
#else
  typedef Alien::Matrix MatrixType ;
  typedef Alien::Vector VectorType ;
#endif
  
  
  typedef Alien::Space Space ;
  typedef Alien::ILinearAlgebra ILinearAlgebra ;
  
  typedef Alien::SolverStat      SolverStat ;
  
  /** Constructeur de la classe */
  AdaptativeLinearSolver(const Arcane::ServiceBuildInfo & sbi);
  
  /** Destructeur de la classe */
  virtual ~AdaptativeLinearSolver();
  
public:
  //! Initialisation
  void init();

  void end();

  /////////////////////////////////////////////////////////////////////////////
  //
  // NEW INTERFACE
  //
  String getBackEndName() const {
    return "none" ;
  }

  //! R�solution du syst�me lin�aire
  bool solve(MatrixType const& A, VectorType const& b, VectorType& x) ;
#ifdef USE_ALIEN_V1
  bool solve(Alien::IMatrix const& A, Alien::IVector const& b, Alien::IVector& x) {
    return true ;
  }
#endif


  //! Indicateur de support de r�solution parall�le
  bool hasParallelSupport() const
  {
    return m_solver->hasParallelSupport();
  }

  boost::shared_ptr<ILinearAlgebra> algebra(const Space&) const {
    return boost::shared_ptr<ILinearAlgebra>() ;
  }

  //! Etat du solveur
  const Status & getStatus() const ;

  const SolverStat & getSolverStat() const { return m_stater; }

  void setMaxIteration(const Integer max_it){}

 void setStopCriteriaEstimator(IStopCriteriaEstimator* estimator) {
    m_stop_criteria_estimator = estimator ;
  }

  std::shared_ptr<Alien::ILinearAlgebra> algebra() const {
    return nullptr ;
  }
  void setNullSpaceConstantOption(bool) {}


  private:
    bool                                         m_is_initialized ;
#ifdef USE_ALIEN_V0
    Alien::BaseLinearSolver*         m_solver ;
#endif
#ifdef USE_ALIEN_V1
    Alien::ILinearSolver*         m_solver ;
#else
    Alien::ILinearSolver*         m_solver ;
#endif

    IStopCriteriaEstimator*           m_stop_criteria_estimator ;
    Integer                                      m_max_iter ;
    Integer                                      m_step_nb_iter ;
    Real                                          m_rtol ;
    Real                                          m_gammaAlg ;
    Status                                       m_status;
    Alien::SolverStater                   m_stater ;
  private:
};

#endif /* ADAPTATIVESOLVERIMPL_H */
