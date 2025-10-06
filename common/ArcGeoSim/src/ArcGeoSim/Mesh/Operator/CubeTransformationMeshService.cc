// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include <arcane/IMeshSubMeshTransition.h>
#include "arcane/BasicService.h"
#include "ArcGeoSim/Utils/Utils.h"

#include "CubeTransformationMeshService.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Appli/IAppServiceMng.h"

#include "ArcGeoSim/Utils/ItemGroupBuilder.h"
#include <stdlib.h>
#include <vector>

using namespace Arcane;


void CubeTransformationMeshService::ApplyOperator() {
  Integer nx = options()->nx() ;
  Integer ny = options()->ny() ;
  Integer nz = options()->nz() ;
  Integer dim =2 ;
  Integer nb_nodes = (nx+1)*(ny+1) ;
  if(nz>0)
  {
    dim = 3 ;
    nb_nodes *= (nz+1) ;
  }
  std::vector<Real> factor(nb_nodes*dim) ;
  std::ifstream fin(options()->file().localstr()) ;
  Integer count = 0 ;
  for(Integer i=0;i<nb_nodes;++i)
  {
    for(Integer k=0;k<dim;++k)
    {
      Real val ;
      fin>>val>>std::ws;
      factor[count++] = val ;
    }
  }

  VariableNodeReal3 & coords = PRIMARYMESH_CAST(mesh())->nodesCoordinates();
  ENUMERATE_NODE(inode,ownNodes())
  {
     Int64 uid = inode->uniqueId() ;
     Real3& coord_node  = coords[inode];
     for(Integer i=0;i<dim;++i)
       coord_node[i] *= factor[dim*uid+i] ;
  }
  
  coords.synchronize();
}

ARCANE_REGISTER_SERVICE_CUBETRANSFORMATIONMESH(CubeTransformationMesh,CubeTransformationMeshService);

