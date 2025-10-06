
#include "ArcGeoSim/Numerics/LinearAlgebra2/MTLSolverImpl/MTLPrecomp.h"

#include "ArcGeoSim/Numerics/LinearAlgebra2/Space.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Matrix.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Vector.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/ILinearSolver.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/ILinearAlgebra.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/IIndexManager.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Impl/SolverStater.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Impl/MultiVectorImpl.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/Impl/MultiMatrixImpl.h"

#include "ArcGeoSim/Numerics/LinearSolver/MTLSolverImpl/MTLOptionTypes.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/MTLSolverImpl/MTLInternal.h"

#include "MTLLinearSolver_axl.h"

#include <boost/shared_ptr.hpp>

/*---------------------------------------------------------------------------*/

class MTLLinearSolverService
 : public ArcaneMTLLinearSolverObject
{
private:
  typedef Alien::ILinearSolver::Status Status ;
  typedef Alien::Matrix         Matrix ;
  typedef Alien::Vector         Vector ;
  typedef Alien::Space          Space ;
  typedef Alien::ILinearAlgebra ILinearAlgebra ;
  typedef Alien::IIndexManager  IIndexManager ;
  typedef Alien::SolverStat     SolverStat ;
  typedef Alien::SolverStater   SolverStater ;

  typedef Alien::MTL4Internal::MatrixInternal MatrixInternal;
  typedef Alien::MTL4Internal::VectorInternal VectorInternal;

public:
  /** Constructeur de la classe */
  MTLLinearSolverService(const Arcane::ServiceBuildInfo & sbi);

  /** Destructeur de la classe */
  virtual ~MTLLinearSolverService() ;
  friend class MTLLinearSystem ; 
public:
  //! Initialisation
  void init() ;

  //! Finalize
  void end() ;

  /////////////////////////////////////////////////////////////////////////////
  //
  // NEW INTERFACE
  //
  String getBackEndName() const { return "mtl" ; }

  //! Résolution du système linéaire
  bool solve(Matrix const& A, Vector const& b, Vector& x);

  //! Indicateur de support de résolution parallèle
  bool hasParallelSupport() const;

  //! Algèbre linéaire compatible
  boost::shared_ptr<ILinearAlgebra> algebra(const Space&) const ;

  //! Etat du solveur
  const Alien::ILinearSolver::Status & getStatus() const ;

  //! Statistiques du solveur
  SolverStater & getSolverStat() { return m_stater; }
  const SolverStat & getSolverStat() const { return m_stater; }

private:
  bool _solve(MatrixInternal::MTLMatrixType const& A, VectorInternal::MTLVectorType const& b, VectorInternal::MTLVectorType& x) ;
  void internalPrintInfo() const ;

private:
  //! Indicateur d'initialisation
  bool m_initialized ;
  
  Status m_status;

  MTLOptionTypes::eSolver m_solver_option ;
  MTLOptionTypes::ePreconditioner m_preconditioner_option ; 
  Integer m_max_iteration ;
  Real m_precision ;

  //! Statistiques d'exécution du solveur
  SolverStater m_stater;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/ISubDomain.h>
#include "ArcGeoSim/Utils/Utils.h"

#include <boost/numeric/mtl/mtl.hpp>
#include <boost/numeric/itl/itl.hpp>

#include "ArcGeoSim/Numerics/LinearAlgebra2/MTLSolverImpl/MTLLinearAlgebra.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef USE_PMTL4
mtl::par::environment * m_global_environment = NULL;
#endif /* USE_PMTL4 */

/*---------------------------------------------------------------------------*/

MTLLinearSolverService::
MTLLinearSolverService(const Arcane::ServiceBuildInfo & sbi) 
  : ArcaneMTLLinearSolverObject(sbi)
  , m_initialized(false)
  , m_solver_option(MTLOptionTypes::BiCGStab)
  , m_preconditioner_option(MTLOptionTypes::ILU0PC)
  , m_max_iteration(0)
  , m_precision(0)
{
;
}

/*---------------------------------------------------------------------------*/

MTLLinearSolverService::
~MTLLinearSolverService()
{
  ;
}

/*---------------------------------------------------------------------------*/

void 
MTLLinearSolverService::
init()
{
  m_stater.reset();
  m_stater.startInitializationMeasure();

#ifdef USE_PMTL4
  int argc = 0 ;
  char** argv = NULL ;
  if(m_global_environment==NULL)
    // m_global_environment = new mtl::par::environment(argc,argv) ;
#endif /* USE_PMTL4 */

  m_max_iteration = options()->maxIterationNum() ;
  m_precision = options()->stopCriteriaValue() ;
  m_solver_option = options()->solver() ;
  m_preconditioner_option = options()->preconditioner() ;

  m_stater.stopInitializationMeasure();

  m_initialized = true ;
}

/*---------------------------------------------------------------------------*/

const Alien::ILinearSolver::Status&
MTLLinearSolverService::
getStatus() const
{
  return m_status;
}

/*---------------------------------------------------------------------------*/

bool
MTLLinearSolverService::
solve(Matrix const& A, Vector const& b, Vector& x)
{
  m_stater.startPrepareMeasure();

  Alien::MTLMatrix const& matrix = A.impl()->get<Alien::BackEnd::tag::mtl>() ;
  Alien::MTLVector const& rhs = b.impl()->get<Alien::BackEnd::tag::mtl>() ;
  Alien::MTLVector& sol = x.impl()->get<Alien::BackEnd::tag::mtl>(true) ;
  m_stater.stopPrepareMeasure();
  return _solve(matrix.internal()->m_internal,rhs.internal()->m_internal,sol.internal()->m_internal) ;
}

/*---------------------------------------------------------------------------*/

boost::shared_ptr<MTLLinearSolverService::ILinearAlgebra>
MTLLinearSolverService::
algebra(const Alien::Space& space) const
{
  return boost::shared_ptr<ILinearAlgebra>(new Alien::MTLLinearAlgebra(space));
}

/*---------------------------------------------------------------------------*/

bool
MTLLinearSolverService::
_solve(MatrixInternal::MTLMatrixType const& matrix, 
       VectorInternal::MTLVectorType const& rhs, 
       VectorInternal::MTLVectorType & x)
{
#ifdef EXPORT
  mtl::io::matrix_market_ostream ifile("matrix.txt") ;
  ifile<<matrix;
  ifile.close();
  ofstream rhsfile("rhs.txt") ;
  rhsfile<<rhs ;
  //exit(0) ;
#endif /* EXPORT */

  try { 

  m_stater.startSolveMeasure();
  switch(m_solver_option)
  {
 case MTLOptionTypes::BiCGStab :
    {
      itl::basic_iteration<double> iter(rhs,m_max_iteration,m_precision);
      switch(m_preconditioner_option)
      {
      case MTLOptionTypes::NonePC:
        {
          itl::pc::identity<MatrixInternal::MTLMatrixType> P(matrix);
          itl::bicgstab(matrix,x,rhs,P,iter);
        }
        break ;
      case MTLOptionTypes::DiagPC:
        {
          itl::pc::diagonal<MatrixInternal::MTLMatrixType> P(matrix);
          itl::bicgstab(matrix,x,rhs,P,iter);
        }
        break ;
      case MTLOptionTypes::ILU0PC:
        {
          itl::pc::ilu_0<MatrixInternal::MTLMatrixType, float> P(matrix);
          itl::bicgstab(matrix,x,rhs,P,iter);
        }
        break ;
      case MTLOptionTypes::ILUTPC:
      case MTLOptionTypes::SSORPC:
        {
          fatal() << "Preconditioner not available";
        }
        break ;
      }
      m_status.iteration_count = iter.iterations() ;
      m_status.residual = iter.resid() ;
      m_status.succeeded = true ;
    }
    break ;
#ifdef MTL_HAS_UMFPACK
    case MTLOptionTypes::LU :
    {
      mtl::matrix::umfpack::solver<MatrixInternal::MTLMatrixType> solver(matrix);
      solver(x,rhs) ;
      m_status.succeeded = true ;
    }
    break ;
#endif /* MTL_HAS_UMFPACK */
 default :
    {
      fatal() << "Solver option not available";
    }
    break ;
  }

  } catch (mtl::logic_error & e) {
    throw Arcane::FatalErrorException(A_FUNCINFO,String::format("MTL Login Error Exception catched while solving linear system : {0}",e.what()));
  } catch (mtl::runtime_error & e) {
    throw Arcane::FatalErrorException(A_FUNCINFO,String::format("MTL Runtime Error Exception catched while solving linear system : {0}",e.what()));
  } catch (mtl::index_out_of_range & e) {
    throw Arcane::FatalErrorException(A_FUNCINFO,String::format("MTL Out of range Exception catched while solving linear system : {0}",e.what()));
  }

#ifdef EXPORT
  mtl::io::matrix_market_ostream ifile("matrix.txt") ;
  ifile<<matrix;
  ifile.close();
  ofstream rhsfile("rhs.txt") ;
  rhsfile<<rhs ;
  rhsfile.close() ;
  ofstream solfile("sol.txt") ;
  solfile<<x ;
  solfile.close() ;
  //exit(0) ;
#endif
  m_stater.stopSolveMeasure(m_status);
  return true ;
}

/*---------------------------------------------------------------------------*/

void 
MTLLinearSolverService::
end()
{
  ;
}

/*---------------------------------------------------------------------------*/

bool 
MTLLinearSolverService::
hasParallelSupport() const
{
#ifdef USE_PMTL4
  return true;
#else /* USE_PMTL4 */
  return false ;
#endif /* USE_PMTL4 */
}

/*---------------------------------------------------------------------------*/

void
MTLLinearSolverService::
internalPrintInfo() const
{
  m_stater.print(traceMng(), m_status, String::format("Linear Solver : {0}",serviceInfo()->localName()));
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_MTLLINEARSOLVER(MTLSolver,MTLLinearSolverService);
