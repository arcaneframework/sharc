#ifndef SUPERLUSOLVERIMPL_H
#define SUPERSOLVERIMPL_H

#include "SuperLUOptionTypes.h"
#include "SuperLUSolver_axl.h"

/**
 * Interface du service de résolution de système linéaire
 */
 
class SuperLULinearSystem ;
class SuperLULinearSystemBuilder ;

class SuperLULinearSolver :
  public ArcaneSuperLUSolverObject
{
public:
  /** Constructeur de la classe */
  SuperLULinearSolver(const ServiceBuildInfo & sbi)
    : ArcaneSuperLUSolverObject(sbi)
    , m_system_is_built(false)
    , m_system_is_locked(false)
    {
      m_system = NULL;
      m_builder = NULL;
    }
  
  /** Destructeur de la classe */
  virtual ~SuperLULinearSolver() ;
  friend class SuperLULinearSystem ;
public:

  //! Définition du constructeur du système linéaire
  void setLinearSystemBuilder(ILinearSystemBuilder * builder)
  {
    m_builder = builder ;
  }

  //! Initialisation
  void init() ;

  void start() ;

  //! Résolution du système linéaire
  bool solve() ;

  String getName() const { return "superlu"; }
  
  //! construit le system lineaire
  bool buildLinearSystem(Integer next) ;
  bool buildLinearSystem(ILinearSystem* system,Integer next) ;
  
  //! recupere la solution
  bool getSolution() ;
  
  void end() ;

  //! Etat du solveur
  const Status & getStatus() const { return m_status; }
  
  ILinearSystem* getLinearSystem() ;

  void setNullSpaceConstantOption(bool flag) {
    warning()<<"Null Space Constant Option not yet implemented" ;
  }

private:
  bool _solveRowMajor() ;
  bool _solveColMajor() ;
  bool m_col_major ;

  void updateLinearSystem();

  void _parse_command_line(int argc,
                     char *argv[],
                     int *lwork,
                     double *u,
                     yes_no_t *equil,
                     trans_t *trans ) ;
  int    m_argc ;
  char** m_argv ;
  bool   m_verbose ;


private:
  //! Structure interne du solveur
  /* Pour l'instant le système est embarqué dans le solveur */
  SuperLULinearSystem * m_system;
  
  //! flag pour verifier si le système a été construit
  bool m_system_is_built ;

  //! flag pour empecher de resoudre le meme système deux fois
  bool m_system_is_locked ;

  //! Constructeur du système linéaire
  ILinearSystemBuilder * m_builder;

  Status m_status;
  
private:
};

#endif /* SUPERLUSOLVERIMPL_H */
