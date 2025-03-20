#include "ArcGeoSim/Utils/Utils.h"

#define MPICH_SKIP_MPICXX 1
#include "mpi.h"

#include <HYPRE_utilities.h>
#include <HYPRE.h>
#include <HYPRE_parcsr_mv.h>

#include <HYPRE_IJ_mv.h>
#include <HYPRE_parcsr_ls.h>
#include <HYPRE_parcsr_mv.h>

#include "ArcGeoSim/Numerics/LinearSolver/ILinearSystemVisitor.h"
#include "ArcGeoSim/Numerics/LinearSolver/ILinearSystemBuilder.h"
#include "ArcGeoSim/Numerics/LinearSolver/ILinearSystem.h"
#include "ArcGeoSim/Numerics/LinearSolver/ILinearSolver.h"


#include "HypreInternal.h"
#include "HypreLinearSystem.h"
#include "HypreLinearSolver.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/

HypreLinearSystem::
HypreLinearSystem(HypreLinearSolver* solver) 
  : m_solver(solver)
{
  m_internal = new HypreInternal();
}

/*---------------------------------------------------------------------------*/

HypreLinearSystem::
~HypreLinearSystem() 
{
  if (m_internal->m_ij_matrix)
    HYPRE_IJMatrixDestroy(m_internal->m_ij_matrix);
  if (m_internal->m_bij_vector)
    HYPRE_IJVectorDestroy(m_internal->m_bij_vector);
  if (m_internal->m_xij_vector)
    HYPRE_IJVectorDestroy(m_internal->m_xij_vector);
  delete m_internal;
}
  
/*---------------------------------------------------------------------------*/

bool 
HypreLinearSystem::
initMatrix( const int ilower, const int iupper,
            const int jlower, const int jupper,
            const ConstArrayView<Integer> & lineSizes)
{
    int ierr; // code d'erreur de retour

    // -- Matrix --
    ierr  = HYPRE_IJMatrixCreate(MPI_COMM_WORLD, ilower, iupper, jlower, jupper, &m_internal->m_ij_matrix);
    ierr += HYPRE_IJMatrixSetObjectType(m_internal->m_ij_matrix, HYPRE_PARCSR);
    ierr += HYPRE_IJMatrixInitialize(m_internal->m_ij_matrix);
    ierr += HYPRE_IJMatrixSetRowSizes(m_internal->m_ij_matrix,lineSizes.unguardedBasePointer());
    
    // -- B Vector --
    ierr += HYPRE_IJVectorCreate(MPI_COMM_WORLD, jlower, jupper, &m_internal->m_bij_vector);
    ierr += HYPRE_IJVectorSetObjectType(m_internal->m_bij_vector, HYPRE_PARCSR);
    ierr += HYPRE_IJVectorInitialize(m_internal->m_bij_vector);

    // -- X Vector --
    ierr += HYPRE_IJVectorCreate(MPI_COMM_WORLD, jlower, jupper, &m_internal->m_xij_vector);
    ierr += HYPRE_IJVectorSetObjectType(m_internal->m_xij_vector, HYPRE_PARCSR);
    ierr += HYPRE_IJVectorInitialize(m_internal->m_xij_vector);

    return (ierr == 0);
}

/*---------------------------------------------------------------------------*/

bool  
HypreLinearSystem::
addMatrixValues(const int nrow, const int * rows,
                       const int * ncols, const int * cols,
                       const Real * values) 
{
    int ierr = HYPRE_IJMatrixAddToValues(m_internal->m_ij_matrix,
                                         nrow, const_cast<int*>(ncols), 
                                         rows, cols,
                                         values);
    return (ierr == 0);
}

/*---------------------------------------------------------------------------*/

bool   
HypreLinearSystem::
setMatrixValues(const int nrow, const int * rows,
                       const int * ncols, const int * cols,
                       const Real * values) 
{
    int ierr = HYPRE_IJMatrixSetValues(m_internal->m_ij_matrix,
                                       nrow, const_cast<int*>(ncols), 
                                       rows, cols,
                                       values);
    return (ierr == 0);
}

/*---------------------------------------------------------------------------*/

bool   
HypreLinearSystem::
addRHSValues(const int nrow, const int * rows,
                   const Real * values) 
{
    int ierr = HYPRE_IJVectorAddToValues(m_internal->m_bij_vector,
                                         nrow, // nb de valeurs
                                         rows,
                                         values);
    return (ierr == 0);
}

/*---------------------------------------------------------------------------*/

bool   
HypreLinearSystem::
setRHSValues(const int nrow, const int * rows,
                   const Real * values) 
{
    int ierr = HYPRE_IJVectorSetValues(m_internal->m_bij_vector,
                                       nrow, // nb de valeurs
                                       rows,
                                       values);
    return (ierr == 0);
}

/*---------------------------------------------------------------------------*/

bool   
HypreLinearSystem::
setInitValues(const int nrow, const int * rows,
                    const Real * values) 
{
    int ierr = HYPRE_IJVectorSetValues(m_internal->m_xij_vector,
                                       nrow, // nb de valeurs
                                       rows,
                                       values);
    return (ierr == 0);
}
  
/*---------------------------------------------------------------------------*/

bool  
HypreLinearSystem::
assemble(Integer nextBuildStage ) 
{
    int ierr=0;
    if ((nextBuildStage & BuildType::eBuildMatrix) != 0) {
      ierr  = HYPRE_IJMatrixAssemble(m_internal->m_ij_matrix);
    }
    if ((nextBuildStage & BuildType::eBuildRhs) ){  
      ierr += HYPRE_IJVectorAssemble(m_internal->m_bij_vector);
      ierr += HYPRE_IJVectorAssemble(m_internal->m_xij_vector);
    }
    //HYPRE_IJMatrixPrint(m_internal->m_ij_matrix,"toto_matrix");     
    return (ierr == 0);
}

/*---------------------------------------------------------------------------*/

bool   
HypreLinearSystem::
getSolutionValues(const int nrow, const int * rows,
		  Real * values)
{
    int ierr;
    ierr = HYPRE_IJVectorGetValues(m_internal->m_xij_vector,
                                   nrow,
                                   rows,
                                   values);
    return (ierr == 0);
}

/*---------------------------------------------------------------------------*/

ISubDomain *
HypreLinearSystem::
getSubDomain() const
{ 
  return m_solver->subDomain(); 
}
/*---------------------------------------------------------------------------*/

Integer
HypreLinearSystem::
getNextBuildStage() const
{
  return m_internal->m_next_build_stage;
  // renvoie la valeur de la prochaine etape du BuildLinearSystem : rien, matrice et ou rhs 
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


