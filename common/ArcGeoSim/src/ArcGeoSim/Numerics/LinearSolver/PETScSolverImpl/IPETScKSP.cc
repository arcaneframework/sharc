#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/String.h>

#include "ArcGeoSim/Numerics/LinearSolver/PETScSolverImpl/IPETScKSP.h"
#include "petsc.h"

/*---------------------------------------------------------------------------*/

void 
IPETScKSP::
checkError(const String & msg, int ierr) const
{
  if (ierr != 0)
    {
      const char * text;
      char * specific;
      PetscErrorMessage(ierr,&text,&specific);
      traceMng()->fatal() << msg << " failed : " << text << " / " << specific << "[code=" << ierr << "]";
    }
}

/*---------------------------------------------------------------------------*/
