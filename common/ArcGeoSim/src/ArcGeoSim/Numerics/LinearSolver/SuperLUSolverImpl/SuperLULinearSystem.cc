#include "Utils/Utils.h"

#include "Numerics/LinearSolver/ILinearSystemVisitor.h"
#include "Numerics/LinearSolver/ILinearSystemBuilder.h"
#include "Numerics/LinearSolver/ILinearSystem.h"
#include "Numerics/LinearSolver/ILinearSolver.h"


#include "SuperLUOptionTypes.h"
#include "SuperLUInternal.h"
#include "SuperLULinearSystem.h"
#include "SuperLUSolver.h"

SuperLULinearSystem::
SuperLULinearSystem(SuperLULinearSolver* solver)
: m_solver(solver)
, m_internal(NULL)
{
}

bool 
SuperLULinearSystem::
initMatrix(const int ilower, const int iupper,
           const int jlower, const int jupper,
           const Arcane::ConstArrayView<Integer> & lineSizes)
{ 
  Integer nrow = iupper+1-ilower ;
  Integer ncol = jupper+1-jlower ;
  m_internal = new SuperLUInternal(nrow,ncol,lineSizes) ;

  return true ;
}


bool
SuperLULinearSystem::
setMatrixValues(const int nrow, const int * rows,
                const int * ncols, const int * cols,
                const Real * values)
{
  Integer off = 0 ;
  for (int i= 0; i < nrow; i++)
    for (int j= 0; j < ncols[i]; j++)
    {
      Integer offset = m_internal->m_xa[rows[i]] ;
      m_internal->m_asub[offset+j] = cols[off] ;
      m_internal->m_a[offset+j] = values[off] ;
      ++off ;
    }
  return true ;
}


bool
SuperLULinearSystem::
setInitValues(const int nrow, const int * rows,
              const Real * values)
{
  for (int i= 0; i < nrow; i++)
    m_internal->m_x[rows[i]] = values[i] ;
  return true ;
}

bool
SuperLULinearSystem::
setRHSValues(const int nrow, const int * rows,
                 const Real * values)
{
  for (int i= 0; i < nrow; i++)
    m_internal->m_rhs[rows[i]] = values[i] ;
  return true ;
}

bool 
SuperLULinearSystem::
getSolutionValues(const int nrow, const int * rows,
                  Real * values)
{
  for (int i= 0; i < nrow; i++)
    values[i] = m_internal->m_rhs[rows[i]] ;
    return true ;
}
bool 
SuperLULinearSystem::assemble(Integer next)
{ 
  return true ;
}


ISubDomain * 
SuperLULinearSystem::
getSubDomain() const
{ 
  return m_solver->subDomain();
}

bool
SuperLULinearSystem::
exportToFile(const String& file, Real eps)
{
  // Buffer de construction
  Integer icount = 0 ;
  ofstream foutput(file.localstr()) ;
  foutput<<"Matrix ("<<m_internal->m_nrow<<","<<m_internal->m_ncol<<") = "<<endl;
  for(Integer irow=0;irow<m_internal->m_nrow;++irow) {
    int row = irow ;
    int ncols = m_internal->m_xa[irow+1]-m_internal->m_xa[irow] ;
    foutput<<"Row("<<row<<") : ";
    for(Integer k=0;k<ncols;++k)
      {
        if(math::abs(m_internal->m_a[icount])>=eps)
          foutput<<"("<<FORMATW(4)<<m_internal->m_asub[icount]<<","<<FORMATS(10,5)<<m_internal->m_a[icount]<<");";
        ++icount ;
      }
    foutput<<endl;
  }
  foutput<<"Vector ("<<m_internal->m_nrow<<")"<<endl;
  for(Integer i=0;i<m_internal->m_nrow;++i)
  {
    foutput<<"B("<<FORMATW(4)<<i<<")="<<FORMATS(10,5)<<m_internal->m_rhs[i]<<endl;
  }
  return true ;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
