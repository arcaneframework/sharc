// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#ifndef COMMON_ARCGEOSIM_SRC_ARCGEOSIM_MESH_ACCESSORTOOLSMNG_IMESHACCESSORMNG_H_
#define COMMON_ARCGEOSIM_SRC_ARCGEOSIM_MESH_ACCESSORTOOLSMNG_IMESHACCESSORMNG_H_

class MeshCellAccessor ;
class RegularGrid ;
class MeshTools ;

namespace ArcGeoSim
{
  class IMeshAccessorToolsMng
  {
  public:
    IMeshAccessorToolsMng(){}
    virtual ~IMeshAccessorToolsMng(){}

    virtual void init() = 0 ;
    virtual MeshCellAccessor* getMeshAccessor()  = 0 ;
    virtual RegularGrid*      getRegularGrid()   = 0 ;
    virtual MeshTools*        getMeshTools()     = 0;
  };

} /* namespace ArcGeoSim */

#endif /* COMMON_ARCGEOSIM_SRC_ARCGEOSIM_MESH_ACCESSORTOOLSMNG_IMESHACCESSORMNG_H_ */
