#include <arcane/ArcaneVersion.h>
#include "Utils/Utils.h"
#include "Numerics/LinearSolver/ILinearSystemVisitor.h"
#include "Numerics/LinearSolver/ILinearSystemBuilder.h"
#include "Numerics/LinearSolver/ILinearSystem.h"
#include "Numerics/LinearSolver/ILinearSolver.h"

#include "SuperLUInternal.h"
#include "SuperLULinearSystem.h"
#include "SuperLUOptionTypes.h"
#include "SuperLUSolver.h"

extern "C" {
#include "slu_ddefs.h"
}
using namespace Arcane;

/*---------------------------------------------------------------------------*/

SuperLULinearSolver::
~SuperLULinearSolver()
{
  if (m_system) 
  {
    delete m_system;
  }
}

/*---------------------------------------------------------------------------*/

void 
SuperLULinearSolver::
init()
{
  if (m_system) 
  {
    delete m_system;
    m_system = NULL;
  }
  
  updateLinearSystem() ;
  m_builder->connect(m_system);
  m_builder->init();
  m_verbose = options()->verbose() ;
  m_col_major = options()->colMajor() ;
  m_argc = options()->arg.size() ;
  if(m_argc>0)
  {
    m_argv = new char*[m_argc] ;
    for(Integer i=0;i<m_argc;++i)
      m_argv[i] = (char*) options()->arg[i].localstr() ;
  }
  m_system_is_built = false ;
  m_system_is_locked = false ;
}

/*---------------------------------------------------------------------------*/

void 
SuperLULinearSolver::
start()
{
  updateLinearSystem() ;
  m_system->start();
}

/*---------------------------------------------------------------------------*/

void 
SuperLULinearSolver::
end()
{
  if(m_system)
    delete m_system ;
  m_system=NULL;
  m_system_is_built = false ;
  m_system_is_locked = false ;
}

/*---------------------------------------------------------------------------*/

void 
SuperLULinearSolver::
updateLinearSystem()
{
  if (m_system==NULL) 
  {
    m_system = new SuperLULinearSystem(this);
    m_system->init();
  }
}

/*---------------------------------------------------------------------------*/

ILinearSystem * 
SuperLULinearSolver::
getLinearSystem()
{
  updateLinearSystem();
  
  return m_system ;
}

/*---------------------------------------------------------------------------*/

bool 
SuperLULinearSolver::
buildLinearSystem(Integer next)
{
  info()<<"SuperLU build Linear system"<<m_system<<" "<<m_builder;
  m_system->m_next_build_stage = next;
  bool flag = m_system->accept(m_builder) ;
  m_system_is_built = true ;
  m_system_is_locked = false ;
  return flag ;
}

bool
SuperLULinearSolver::
buildLinearSystem(ILinearSystem*  system,Integer next)
{
  m_system = dynamic_cast<SuperLULinearSystem*>(system) ;
  return buildLinearSystem(next) ;
}

/*---------------------------------------------------------------------------*/

bool 
SuperLULinearSolver::
getSolution()
{
  return m_builder->commitSolution(m_system) ;
}

/*---------------------------------------------------------------------------*/

bool SuperLULinearSolver::solve()
{
  if(m_col_major)
    return _solveColMajor() ;
  else
    return _solveRowMajor() ;
}

bool SuperLULinearSolver::_solveRowMajor()
{
  superlu_options_t options;
  SuperLUStat_t stat;

  Integer info ;
  int      *perm_r; /* row permutations from partial pivoting */
  int      *perm_c; /* column permutation vector */

  /* Create matrix A in the format expected by SuperLU. */
  dCreate_CompRow_Matrix(&m_system->m_internal->m_A,
                         m_system->m_internal->m_nrow,
                         m_system->m_internal->m_ncol,
                         m_system->m_internal->m_nnz,
                         m_system->m_internal->m_a,
                         m_system->m_internal->m_asub,
                         m_system->m_internal->m_xa,
                         SLU_NC,
                         SLU_D,
                         SLU_GE);

  dCreate_Dense_Matrix(&m_system->m_internal->m_B,
                       m_system->m_internal->m_nrow,
                       1,
                       m_system->m_internal->m_rhs,
                       m_system->m_internal->m_nrow,
                       SLU_DN,
                       SLU_D,
                       SLU_GE);

  /* Set the default input options. */
  set_default_options(&options);
  options.ColPerm = NATURAL;

  /* Initialize the statistics variables. */
  StatInit(&stat);

  int m = m_system->m_internal->m_nrow ;
  int n = m_system->m_internal->m_ncol ;

  if ( !(perm_r = intMalloc(m)) ) ABORT("Malloc fails for perm_r[].");
  if ( !(perm_c = intMalloc(n)) ) ABORT("Malloc fails for perm_c[].");

  dgssv(&options,
        &m_system->m_internal->m_A,
        perm_c,
        perm_r,
        &m_system->m_internal->m_L,
        &m_system->m_internal->m_U,
        &m_system->m_internal->m_B,
        &stat,
        &info);

  SUPERLU_FREE (perm_r);
  SUPERLU_FREE (perm_c);
  Destroy_SuperNode_Matrix(&m_system->m_internal->m_L);
  Destroy_CompCol_Matrix(&m_system->m_internal->m_U);

  StatFree(&stat);

  return true ;
}

bool SuperLULinearSolver::_solveColMajor()
{
  char           equed[1];
  yes_no_t       equil;
  trans_t        trans;
  // NCformat       *Astore;
  // NCformat       *Ustore;
  // SCformat       *Lstore;
  SuperMatrix    X;

  superlu_options_t options;
  SuperLUStat_t stat;
  Integer info ;

  int            *perm_r; /* row permutations from partial pivoting */
  int            *perm_c; /* column permutation vector */
  int            *etree;
  void           *work = NULL;
  int            lwork, nrhs ;
  double         *rhsx ;
  double         *R, *C;
  double         *ferr, *berr;
  double         u, rpg, rcond;
  mem_usage_t    mem_usage;
  //extern void  parse_command_line();

  /* Defaults */
  lwork = 0;
  nrhs = 1;
  equil = YES;
  u     = 1.0;
  trans = TRANS;

  /* Set the default input options:
  options.Fact = DOFACT;
      options.Equil = YES;
      options.ColPerm = COLAMD;
  options.DiagPivotThresh = 1.0;
      options.Trans = NOTRANS;
      options.IterRefine = NOREFINE;
      options.SymmetricMode = NO;
      options.PivotGrowth = NO;
      options.ConditionNumber = NO;
      options.PrintStat = YES;
  */
  set_default_options(&options);

  /* Can use command line input to modify the defaults. */
  _parse_command_line(m_argc, m_argv, &lwork, &u, &equil, &trans);
  options.Equil = equil;
  options.DiagPivotThresh = u;
  options.Trans = trans;

  /* Add more functionalities that the defaults. */
  options.PivotGrowth = YES;    /* Compute reciprocal pivot growth */
  options.ConditionNumber = YES;/* Compute reciprocal condition number */
  options.IterRefine = DOUBLE;  /* Perform double-precision refinement */

  if ( lwork > 0 )
  {
    work = SUPERLU_MALLOC(lwork);
    if ( !work )
    {
      ABORT("DLINSOLX: cannot allocate work[]");
    }
  }

  int m = m_system->m_internal->m_nrow ;
  int n = m_system->m_internal->m_ncol ;
  dCreate_CompCol_Matrix(&m_system->m_internal->m_A,
                         m_system->m_internal->m_nrow,
                         m_system->m_internal->m_ncol,
                         m_system->m_internal->m_nnz,
                         m_system->m_internal->m_a,
                         m_system->m_internal->m_asub,
                         m_system->m_internal->m_xa,
                         SLU_NC,
                         SLU_D,
                         SLU_GE);
  //Astore = (NCformat *) m_system->m_internal->m_A.Store;

  //if ( !(rhsb = doubleMalloc(m * nrhs)) ) ABORT("Malloc fails for rhsb[].");
  if ( !(rhsx = doubleMalloc(m * nrhs)) ) ABORT("Malloc fails for rhsx[].");
  dCreate_Dense_Matrix(&m_system->m_internal->m_B,
                       m_system->m_internal->m_nrow,
                       nrhs,
                       m_system->m_internal->m_rhs,
                       m_system->m_internal->m_nrow,
                       SLU_DN,
                       SLU_D,
                       SLU_GE);
  dCreate_Dense_Matrix(&X,
                       m_system->m_internal->m_nrow,
                       nrhs,
                       rhsx,
                       m_system->m_internal->m_nrow,
                       SLU_DN,
                       SLU_D,
                       SLU_GE);
  /*
  xact = doubleMalloc(n * nrhs);
  ldx = n;
  dGenXtrue(n, nrhs, xact, ldx);
  dFillRHS(trans,
           nrhs,
           xact,
           ldx,
           &m_system->m_internal->m_A,
           &m_system->m_internal->m_B);*/

  if ( !(etree = intMalloc(n)) ) ABORT("Malloc fails for etree[].");
  if ( !(perm_r = intMalloc(m)) ) ABORT("Malloc fails for perm_r[].");
  if ( !(perm_c = intMalloc(n)) ) ABORT("Malloc fails for perm_c[].");
  if ( !(R = (double *) SUPERLU_MALLOC(m_system->m_internal->m_A.nrow * sizeof(double))) )
      ABORT("SUPERLU_MALLOC fails for R[].");
  if ( !(C = (double *) SUPERLU_MALLOC(m_system->m_internal->m_A.ncol * sizeof(double))) )
      ABORT("SUPERLU_MALLOC fails for C[].");
  if ( !(ferr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) )
      ABORT("SUPERLU_MALLOC fails for ferr[].");
  if ( !(berr = (double *) SUPERLU_MALLOC(nrhs * sizeof(double))) )
      ABORT("SUPERLU_MALLOC fails for berr[].");


  /* Initialize the statistics variables. */
  StatInit(&stat);

  /* Solve the system and compute the condition number
     and error bounds using dgssvx.      */

  dgssvx(&options,
         &m_system->m_internal->m_A,
         perm_c,
         perm_r,
         etree,
         equed,
         R,
         C,
         &m_system->m_internal->m_L,
         &m_system->m_internal->m_U,
         work,
         lwork,
         &m_system->m_internal->m_B,
         &X,
         &rpg,
         &rcond,
         ferr,
         berr,
         &mem_usage,
         &stat,
         &info);

  if ( info == 0 || info == n+1 )
  {
      /* This is how you could access the solution matrix. */
      double *sol = (double*) ((DNformat*) X.Store)->nzval;
      for(Integer i=0;i<m_system->m_internal->m_nrow;++i)
        m_system->m_internal->m_rhs[i] = sol[i] ;
      if(m_verbose)
      {
        if ( options.PivotGrowth == YES )
          printf("Recip. pivot growth = %e\n", rpg);
        if ( options.ConditionNumber == YES )
          printf("Recip. condition number = %e\n", rcond);
        if ( options.IterRefine != NOREFINE )
        {
           printf("Iterative Refinement:\n");
           printf("%8s%8s%16s%16s\n", "rhs", "Steps", "FERR", "BERR");
           for (int i = 0; i < nrhs; ++i)
             printf("%8d%8d%16e%16e\n", i+1, stat.RefineSteps, ferr[i], berr[i]);
        }
      }
      //Lstore = (SCformat *) m_system->m_internal->m_L.Store;
      //Ustore = (NCformat *) m_system->m_internal->m_U.Store;
  }

  if ( options.PrintStat ) StatPrint(&stat);
  StatFree(&stat);

  //SUPERLU_FREE (rhsb);
  SUPERLU_FREE (rhsx);
  //SUPERLU_FREE (xact);
  SUPERLU_FREE (etree);
  SUPERLU_FREE (perm_r);
  SUPERLU_FREE (perm_c);
  SUPERLU_FREE (R);
  SUPERLU_FREE (C);
  SUPERLU_FREE (ferr);
  SUPERLU_FREE (berr);
  Destroy_SuperMatrix_Store(&X);
  if ( lwork >= 0 )
  {
      Destroy_SuperNode_Matrix(&m_system->m_internal->m_L);
      Destroy_CompCol_Matrix(&m_system->m_internal->m_U);
  }

  return true ; 
}

void
SuperLULinearSolver::
_parse_command_line(int argc,
                   char *argv[],
                   int *lwork,
                   double *u,
                   yes_no_t *equil,
                   trans_t *trans )
{
    int c;
    extern char *optarg;

    while ( (c = getopt(argc, argv, "hl:w:r:u:f:t:p:e:")) != EOF ) {
    switch (c) {
      case 'h':
        printf("Options:\n");
        printf("\t-l <int> - length of work[*] array\n");
        printf("\t-u <int> - pivoting threshold\n");
        printf("\t-e <0 or 1> - equilibrate or not\n");
        printf("\t-t <0 or 1> - solve transposed system or not\n");
        exit(1);
        break;
      case 'l': *lwork = atoi(optarg);
                break;
      case 'u': *u = atof(optarg);
                break;
      case 'e': *equil = (yes_no_t) atoi(optarg);
                break;
      case 't': *trans = (trans_t) atoi(optarg);
                break;
    }
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_SUPERLUSOLVER(SuperLUSolver,SuperLULinearSolver);
