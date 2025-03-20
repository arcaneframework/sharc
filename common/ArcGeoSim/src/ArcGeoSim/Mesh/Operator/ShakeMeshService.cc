// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include <arcane/IMeshSubMeshTransition.h>
#include "arcane/BasicService.h"
#include "ArcGeoSim/Utils/Utils.h"

#include "ArcGeoSim/Numerics/Expressions/IFunctionR4vR1.h"
#include "ShakeMeshService.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Appli/IAppServiceMng.h"

#include "ArcGeoSim/Utils/ItemGroupBuilder.h"
#include <stdlib.h>

using namespace Arcane;

double 
ShakeMeshService::
_myrandom()
{
#ifdef ARCANE_OS_WIN32
  return (double)( rand() / (RAND_MAX + 1) );
#else
  return drand48();
#endif
}

void ShakeMeshService::ApplyOperator() {
	
    IFunctionR4vR1* Xu = options()->shake_function_x() ;
    IFunctionR4vR1* Yu = options()->shake_function_y() ;
    IFunctionR4vR1* Zu = options()->shake_function_z() ;
    Xu->init() ;
    Yu->init() ;
    Zu->init() ;
    // size of the noise for "the random"
    Real size_noise = options()->size_noise();
    
    
     VariableNodeReal3 & coords = PRIMARYMESH_CAST(mesh())->nodesCoordinates();
     ENUMERATE_NODE(inode,ownNodes()) {  		    
 
      const Real rx     = _myrandom() * size_noise ;
      const Real ry     = _myrandom() * size_noise ;
      const Real rz     = _myrandom() * size_noise ;
      Real3 coord_node  = coords[inode];      
      coord_node += Real3(rx,ry,rz);
     
     // New coordinates
      Real x = Xu->eval(coord_node.x,coord_node.y,coord_node.z,0);
      Real y = Yu->eval(coord_node.x,coord_node.y,coord_node.z,0);
      Real z = Zu->eval(coord_node.x,coord_node.y,coord_node.z,0);
      coords[inode] = Real3(x,y,z);
     
     }
  
       coords.synchronize();
}

ARCANE_REGISTER_SERVICE_SHAKEMESH(ShakeMesh,ShakeMeshService);

