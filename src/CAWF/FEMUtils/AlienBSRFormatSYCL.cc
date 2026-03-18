// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* AlienBSRFormat.cc                                                (C) 2022-2025 */
/*                                                                           */
/* Matrix format using Block Sparse Row.                                     */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/utils/ArgumentException.h>

#include <arcane/core/IIndexedIncrementalItemConnectivityMng.h>
#include <arcane/core/IIndexedIncrementalItemConnectivity.h>
#include <arcane/core/MeshUtils.h>

#include <arcane/accelerator/RunCommandLoop.h>
#include <arcane/accelerator/GenericSorter.h>
#include <arcane/accelerator/Scan.h>
#include <alien/arcane_tools/accessors/ItemVectorAccessor.h>
#include <alien/core/block/VBlock.h>

#include <alien/arcane_tools/IIndexManager.h>
#include <alien/arcane_tools/indexManager/BasicIndexManager.h>
#include <alien/arcane_tools/indexManager/SimpleAbstractFamily.h>
#include <alien/arcane_tools/distribution/DistributionFabric.h>
#include <alien/arcane_tools/indexSet/IndexSetFabric.h>
#include <alien/arcane_tools/data/Space.h>

#include <alien/handlers/scalar/sycl/VectorAccessorT.h>
#include <alien/handlers/scalar/sycl/MatrixProfiler.h>
#include <alien/handlers/scalar/sycl/ProfiledMatrixBuilderT.h>
#include <alien/handlers/scalar/sycl/CombineProfiledMatrixBuilderT.h>

#include <alien/handlers/scalar/sycl/CombineProfiledMatrixBuilderImplT.h>
#include <alien/handlers/scalar/sycl/VectorAccessorImplT.h>
#include <alien/handlers/scalar/sycl/ProfiledMatrixBuilderImplT.h>

#include <alien/kernels/simple_csr/algebra/SimpleCSRLinearAlgebra.h>
#include <alien/kernels/simple_csr/algebra/SimpleCSRInternalLinearAlgebra.h>

#include <alien/ref/AlienRefSemantic.h>
#include <alien/ref/AlienImportExport.h>

#include <alien/kernels/redistributor/Redistributor.h>
#include <alien/ref/data/scalar/RedistributedVector.h>
#include <alien/ref/data/scalar/RedistributedMatrix.h>
#include <alien/ref/import_export/MatrixMarketSystemWriter.h>

#include <alien/expression/solver/SolverStater.h>
#include <alien/AlienLegacyConfig.h>

#include "AlienBSRFormat.h"

#include "DoFLinearSystem.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::FemUtils
{

void AlienBSRFormat::
computeProfile(const std::function<void(Alien::SYCL::MatrixProfiler&,ConstArrayView<Integer>)>& compute_matrix_profile)
{
    Alien::SYCL::MatrixProfiler profiler(m_matrixA);
    compute_matrix_profile(profiler,m_allUIndex.constView()) ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Arcane::FemUtils

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
