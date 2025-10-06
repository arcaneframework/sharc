#ifndef SLULINEARSYSTEM_H
#define SLULINEARSYSTEM_H

/**
 * Interface du service de résolution de système linéaire
 */

#include <arcane/ISubDomain.h>

using namespace Arcane;

class ILinearSystem ;
class ILinearSystemBuilder ;
class SuperLULinearSolver ;
class SuperLUInternal ;

class SuperLULinearSystem : public ILinearSystem
{
public:
  //!Constructeur 
  SuperLULinearSystem(SuperLULinearSolver* solver) ;

  /** Destructeur */
  virtual ~SuperLULinearSystem(){}

  //! initialise le system linéaire
  void init() {}
  
  bool connect(ILinearSystemBuilder * builder)
  {
    return builder->connect(this) ;
  }

  //! permet d'ajouter des opérateurs à l'interface
  bool accept(ILinearSystemVisitor* visitor)
  { 
    return visitor->visit(this) ;
  }
  
  //! démare une étape de résolution de système linéaire
  void start() {}
  
  //! finalise une étape de résolution et libère les objets intermédiaires
  void end() {}

  //! retourne le nom du type de système
  const char * name() const { return "SuperLU"; }
  
  bool initMatrix(const int ilower, const int iupper,
                  const int jlower, const int jupper,
                  const Arcane::ConstArrayView<Integer> & lineSizes) ;


  bool setMatrixValues(const int nrow, const int * rows,
                       const int * ncols, const int * cols,
                       const Real * values) ;

  bool setRHSValues(const int nrow, const int * rows,
                    const Real * values) ;

  bool setInitValues(const int nrow, const int * rows,
                     const Real * values) ;
  
  bool assemble(Integer nextBuildStage) ;

  bool getSolutionValues(const int nrow, const int * rows,
                         Real * values) ;
  
  ISubDomain * getSubDomain() const;

  Integer getNextBuildStage() const {
    return m_next_build_stage ;
  }

  bool exportToFile(const String& file,Real eps=0.) ;
protected:
  friend class SuperLULinearSolver;

private :
  SuperLULinearSolver* m_solver ;
  SuperLUInternal* m_internal ;
  // est ce qu'on assemble la matrice et/ou le 2nd membre
  Integer m_next_build_stage;
};

#endif /* HYPRESYSTEMIMPL_H */
