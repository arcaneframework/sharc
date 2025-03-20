#ifndef HYPRESOLVERIMPL_H
#define HYPRESOLVERIMPL_H

#include "ArcGeoSim/Numerics/LinearAlgebra2/HypreSolverImpl/HyprePrecomp.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/HypreSolverImpl/HypreLinearAlgebra.h"

#include "ArcGeoSim/Numerics/LinearAlgebra2/Space.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Matrix.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Vector.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/ILinearSolver.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/ILinearAlgebra.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Impl/SolverStater.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Impl/MultiVectorImpl.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Impl/MultiMatrixImpl.h"

//
//#include "ArcGeoSim/Numerics/LinearSolver/HypreSolverImpl/HypreInternal.h"

#include "HypreSolver_axl.h"

/**
 * Interface du service de résolution de système linéaire
 */
 
using namespace Arcane;

class HypreLinearSolver
 : public ArcaneHypreSolverObject
{
public:
  typedef Alien::ILinearSolver::Status Status ;
  typedef Alien::Matrix MatrixType ;
  typedef Alien::Vector VectorType ;
  
  typedef Alien::HypreInternal::MatrixInternal::matrix_type  HypreMatrixType ;
  typedef Alien::HypreInternal::VectorInternal::vector_type  HypreVectorType ;
  
  typedef Alien::Space Space ;
  typedef Alien::ILinearAlgebra ILinearAlgebra ;
  
  typedef Alien::SolverStat      SolverStat ;
  typedef Alien::SolverStater    SolverStater ;
  
  /** Constructeur de la classe */
  HypreLinearSolver(const ServiceBuildInfo & sbi);
  
  /** Destructeur de la classe */
  virtual ~HypreLinearSolver();
  
public:
  //! Initialisation
  void init();

  void end();
  /////////////////////////////////////////////////////////////////////////////
  //
  // NEW INTERFACE
  //
  String getBackEndName() const {
    return "hypre" ;
  }

  //! Résolution du système linéaire
  bool solve(MatrixType const& A, VectorType const& b, VectorType& x) ;

  //! Indicateur de support de résolution parallèle
  bool hasParallelSupport() const
  {
    return true;
  }

  boost::shared_ptr<ILinearAlgebra> algebra(const Space&) const ;

  //! Etat du solveur
  const Status & getStatus() const ;

  const SolverStat & getSolverStat() const { return m_stat; }

 private:

  bool _solve(HypreMatrixType const& ij_matrix,HypreVectorType const& bij_vector,
              HypreVectorType& xij_vector) ;

private:
  Status m_status;

  Real m_init_time ;
  Real m_total_build_time ;
  Real m_total_solve_time ;
  Integer m_solve_num ;
  Integer m_total_iter_num ;

  SolverStat   m_stat ;
  SolverStater m_stater;
private:
  void checkError(const String & msg, int ierr, int skipError = 0) const;
};

#endif /* HYPRESOLVERIMPL_H */
