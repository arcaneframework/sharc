#ifndef EIGENLUSOLVER_H_
#define EIGENLUSOLVER_H_



#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSolver.h"
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSystem.h"
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSystemBuilder.h"
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/ILinearSystemVisitor.h"
#include "ArcGeoSim/Numerics/LinearSolver/LocalDirectSolverImpl/LocalLinearSystemT.h"



template<int N>
class EigenLUSolver : public LocalDirectSolverNamespace::ILinearSolver
{
  public:

  typedef Eigen::Matrix<Real,N,N,Eigen::RowMajor>                    MatrixType ;
  typedef Eigen::Matrix<Real,N,1>                                    VectorType ;

  typedef LinearSystemT<MatrixType,VectorType>                       LinearSystemType ;

  //! Constructeur de la classe
  EigenLUSolver()
  : ILinearSolver()
  , m_system(NULL)
  , m_system_is_built(false)
  , m_system_is_locked(false)
  , m_builder(NULL)
  {}

  //! Destructeur de la classe
  virtual ~EigenLUSolver() {}

public:
  String getName() const {
    return "EigenLUSolver" ;
  }

  //! Initialisation
  void init() {}

  //! Associe un build à ce solveur
  void setLinearSystemBuilder(ILinearSystemBuilder * builder)
  { m_builder = builder ; }

  //! Retourne le système linéaire associé à ce solveur
  ILinearSystem* getLinearSystem() { return m_system ; }

  //! Construit le system lineaire
  bool buildLinearSystem(Integer nextBuildStage=BuildType::eBuildMatrix|BuildType::eBuildRhs)
  {
    bool flag = (m_builder?m_system->accept(m_builder):true) ;
    m_system_is_built = true ;
    m_system_is_locked = false ;
    return flag ;
  }

  bool buildLinearSystem(ILinearSystem*  system,Integer nextBuildStage=BuildType::eBuildMatrix|BuildType::eBuildRhs)
  {
    bool flag = system->accept(m_builder) ;
    m_system_is_built = true ;
    m_system_is_locked = false ;
    return flag ;
  }


  //! @name Etapes d'une résolution
  //@{

  //! Début de boucle locale (avant solve)
  void start() {
    delete m_system ;
    m_system = new LinearSystemType() ;
  }
  //! Fin de boucle locale (après solve)
  void end() {
    delete m_system ;
    m_system = NULL ;
    m_system_is_built = false ;
    m_system_is_locked = false ;
  }

  //! Résolution du system lineaire associé
  bool solve() {

    if(!m_system_is_built)
    {
      cerr<<"Linear system is not built, buildLinearSystem shoud be called first"<<endl ;
      return  false ;
    }
    if(m_system_is_locked)
    {
      cerr<<"linear system has already be solved once and has not been modified since" ;
      return false ;
    }
    *(m_system->getX()) = m_system->getMatrix()->lu().solve(*m_system->getRhs()) ;
    m_status.succeeded = true ;
    m_status.error = 0 ;
    m_status.iteration_count = 0 ;
    m_status.residual = 0. ;
    m_system_is_locked = true ;
    return true ;
  }

  //! Etat final après résolution
  const ILinearSolver::Status & getStatus() const { return m_status ; }

  //! Applique la procédure d'extraction de la solution
  bool getSolution() {
    if(m_builder)
      return m_builder->commitSolution(m_system) ;
    else
      return false ;
  }

  void setNullSpaceConstantOption(bool flag) {
    cout<<"Null Space Constant Option not yet implemented"<<endl ;
  }
private :
  LinearSystemType* m_system ;

  //! flag pour verifier si le système a été construit
  bool m_system_is_built ;

  //! flag pour empecher de resoudre le meme système deux fois
  bool m_system_is_locked ;
  ILinearSystemBuilder* m_builder ;
  ILinearSolver::Status m_status ;
};
#endif /*EIGENLUSOLVER_H_*/
