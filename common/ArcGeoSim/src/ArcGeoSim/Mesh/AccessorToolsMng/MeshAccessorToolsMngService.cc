// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/IMesh.h>
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshCellAccessor.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/RegularGrid.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshTools.h"

#include <arcane/utils/ITraceMng.h>

#include "IMeshAccessorToolsMng.h"
#include "MeshAccessorToolsMng_axl.h"


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class MeshAccessorToolsMngService : public ArcaneMeshAccessorToolsMngObject
{
public:

  /** Constructeur de la classe */
  MeshAccessorToolsMngService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneMeshAccessorToolsMngObject(sbi)
  {}

  /** Destructeur de la classe */
  ~MeshAccessorToolsMngService()
  {}

public:
  void init() ;

  MeshCellAccessor* getMeshAccessor() ;

  RegularGrid*      getRegularGrid() ;

  MeshTools*        getMeshTools();

private:

  std::unique_ptr<MeshCellAccessor> m_mesh_accessor ;

  std::unique_ptr<RegularGrid>      m_regular_grid ;

  std::unique_ptr<MeshTools>        m_mesh_tools ;

  bool m_initialized = false ;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void MeshAccessorToolsMngService::init()
{
  m_initialized = true ;
}

MeshCellAccessor* MeshAccessorToolsMngService::getMeshAccessor()
{
  if(m_mesh_accessor.get()==nullptr)
    m_mesh_accessor.reset(new MeshCellAccessor(mesh())) ;
  return m_mesh_accessor.get() ;
}

RegularGrid* MeshAccessorToolsMngService::getRegularGrid()
{
  if(m_regular_grid.get()==nullptr)
  {
    OrthonormalBoundingBox bbox = getMeshAccessor()->getBoundingBox();

    switch(options()->gridType())
    {
      case RegularGrid::BASIC :
        m_regular_grid.reset(new RegularGrid(getMeshAccessor(),
          options()->nx(),options()->ny(),options()->nz(),
          bbox,
          options()->nbPointsPerCell(),
          options()->epsilon(),
          options()->thresholdAugmentation()));
        break ;
      default :
        m_regular_grid.reset(new RegularGrid(getMeshAccessor(),
          options()->gridType(),
          bbox,
          options()->nbPointsPerCell(),
          options()->epsilon(),
          options()->thresholdAugmentation()));

    }
    m_regular_grid->insertAll(mesh());
  }
  return m_regular_grid.get() ;
}

MeshTools* MeshAccessorToolsMngService::getMeshTools()
{
  if(m_mesh_tools.get()==nullptr)
    m_mesh_tools.reset(new MeshTools(mesh())) ;
  return m_mesh_tools.get() ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_MESHACCESSORTOOLSMNG(MeshAccessorToolsMng, MeshAccessorToolsMngService);
