#ifndef LOCALLINEARSYSTEMT_H_
#define LOCALLINEARSYSTEMT_H_



#include "ArcGeoSim/Numerics/NonLinearSolver/LocalDirectSolverImpl/ILinearSystem.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/LocalDirectSolverImpl/ILinearSystemBuilder.h"
#include "ArcGeoSim/Numerics/NonLinearSolver/LocalDirectSolverImpl/ILinearSystemVisitor.h"

template<typename MatrixT, typename VectorT>
class LinearSystemT : public ILinearSystem
{
public :
  typedef MatrixT                                                     MatrixType ;
  typedef VectorT                                                     VectorType ;
  typedef LinearSystemT<MatrixType,VectorType> ThisType ;
  typedef VisitorT<ThisType>                                   VisitorType ;

  LinearSystemT()
  : m_matrix(NULL)
  , m_x(NULL)
  , m_rhs(NULL)
  {}

  LinearSystemT(MatrixType* matrix,
                              VectorType* x,
                              VectorType* rhs)
  : m_matrix(matrix)
  , m_x(x)
  , m_rhs(rhs)
  {}

  ILinearSystem* asILinearSystem() {
    return this ;
  }
  //! initialise le system lin�aire
  void init() {} ;

  //! permet d'ajouter des op�rateurs au l'interface
  bool accept(ILinearSystemVisitor* visitor)
  {
    VisitorType* ptr = dynamic_cast<VisitorType*>(visitor) ;
    if(ptr)
      return ptr->visit(this) ;
    else
      return visitor->visit(asILinearSystem()) ;
  }

  //! d�mare une �tape de r�solution de syst�me lin�aire
  void start() {}

  //! finalise une �tape de r�solution et lib�re les objets interm�diaires
  void end() {}

  //! retourne le nom du type de syst�me
  virtual const char * name() const { return "LinearSystemT" ; }

  MatrixType* getMatrix() { return m_matrix ; }
  VectorType* getX() { return m_x ; }
  VectorType* getRhs() { return m_rhs ; }

  void setMatrix(MatrixType* matrix) { m_matrix = matrix ; }
  void setX(VectorType* x) { m_x = x ; }
  void setRhs(VectorType* rhs) { m_rhs = rhs ; }
private :
  MatrixType* m_matrix ;
  VectorType* m_x ;
  VectorType* m_rhs ;
};

template<typename MatrixT, typename VectorT>
class LinearSystemBuilderT
    : public ILinearSystemBuilder
    , public VisitorT< LinearSystemT<MatrixT,VectorT> >
{
public :
  typedef MatrixT                                                     MatrixType ;
  typedef VectorT                                                     VectorType ;
  typedef LinearSystemT<MatrixType,VectorType> LinearSystemType ;

  LinearSystemBuilderT(MatrixType* matrix,VectorType* rhs,VectorType* x)
  : ILinearSystemBuilder()
  , m_matrix(matrix)
  , m_rhs(rhs)
  , m_x(x)
  {
  }


  virtual ~LinearSystemBuilderT() {}

  void init() {}
  void start() {}
  void freeData() {}
  void end() {}

  Integer connect(LinearSystemType* system)
  {
    system->setMatrix(m_matrix) ;
    system->setRhs(m_rhs) ;
    system->setX(m_x) ;
    return 0;
  }

  bool commitSolution(ILinearSystem* system)
  { return true; }

  bool commitSolution(LinearSystemType* system)
  { return true; }

  Integer visit(LinearSystemType* system)
  { return connect(system) ; }

private :
  MatrixType* m_matrix ;
  VectorType* m_rhs ;
  VectorType* m_x ;
};

#endif /*LOCALLINEARSYSTEMT_H_*/
