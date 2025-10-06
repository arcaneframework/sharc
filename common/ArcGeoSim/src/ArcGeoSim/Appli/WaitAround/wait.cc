// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#define MPICH_SKIP_MPICXX 1

#include <mpi.h>

#include <arcane/utils/FatalErrorException.h>

extern "C" {
  // Wrapper around MPI_Waitall due to Intel Mpi implementation which fails when count is 0
  int MPI_Waitall(int count, MPI_Request array_of_requests[],
                  MPI_Status array_of_statuses[]) {
    if (count == 0) 
      {
        return MPI_SUCCESS;
      }
    else
      {
        return PMPI_Waitall(count,array_of_requests,array_of_statuses);
        //      throw Arcane::FatalErrorException(A_FUNCINFO,"MPI_Waitall work around fails when count != 0");
        //      return MPI_ERR_IN_STATUS;
      }
  } 
}
