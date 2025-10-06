// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "EstimatorService.h"
#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include <arcane/utils/Math.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/ITimeLoopMng.h>
#include <arcane/Timer.h>
#include <arcane/ArcaneTypes.h>
#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include <arcane/MathUtils.h>
#include "TypesEstimator.h"

using namespace Arcane;

//=======================================================================================
// Destructeur
//=======================================================================================

EstimatorService::
~EstimatorService()
{

}
void EstimatorService::init(IMesh* mesh)
{
  IAppServiceMng * app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());
  m_geometry_service = app_service_mng->find<IGeometryMng>(true);
  m_geometry_service->addItemGroupProperty(mesh->allFaces(),IGeometryProperty::PCenter,IGeometryProperty::PVariable);
  m_geometry_service->addItemGroupProperty(mesh->allCells(),IGeometryProperty::PCenter,IGeometryProperty::PVariable);
  /*
  Int32Array item_lids;
  item_lids.reserve(m_mesh->outerFaces().size()) ;
  std::set<Integer> s ;
  ENUMERATE_FACE(iface,m_mesh->outerFaces())
  {
    Cell const& cell = iface->boundaryCell() ;
    if(cell.isOwn())
    {
      Integer lid = cell->localId() ;
      std::pair<std::set<Integer>::iterator,bool> iter = s.insert(lid) ;
      if(iter.second)
      {
        item_lids.add(lid) ;
      }
    }
  }
  IItemFamily * cell_family = mesh->cellFamily();
  // On cr�e le groupe de face des conditions limites
  m_boundary_cell_group = cell_family->createGroup("TwoPtsEstimatorBoundaryCellGroup",item_lids);

  m_geometry_service->addItemGroupProperty(mesh->allActiveFaces(),IGeometryProperty::PNormal,IGeometryProperty::PVariable);
  */
}

void EstimatorService::registerData(IMeshAdapter* adapter)
{
  m_mesh_adapter = adapter ;
  m_mesh_adapter->addObserver(&m_mesh_observer) ;
  //m_mesh_adapter->registerUnrefinableCellGroup(m_boundary_cell_group) ;
}

void EstimatorService::update(IMesh* mesh)
{
  if(m_mesh_observer.hasChanged()||m_first_update)
  {
    CellGroup cells = mesh->allCells();
    FaceGroup faces = mesh->allFaces();

    const IGeometryMng::Real3Variable& f_centers = m_geometry_service->getReal3VariableProperty(faces,
        IGeometryProperty::PCenter);
    const IGeometryMng::Real3Variable& c_centers = m_geometry_service->getReal3VariableProperty(cells,
        IGeometryProperty::PCenter);

    CellGroup group ;
    if(m_first_update)
    {
      group = cells ;
      m_first_update = false ;
    }
    else
      group = m_mesh_adapter->getJustAddedCells() ;

    ENUMERATE_CELL(icell,group)
    {
      Real3 xC = c_centers[icell] ;
      Real hsize = 0 ;
      for(FaceEnumerator iface(icell->faces());iface.hasNext();++iface)
      {
        Real3 CF = xC - f_centers[*iface] ;
        hsize = std::max(hsize,CF.abs()) ;
      }
      m_h_size[icell] = 2*hsize ;
    }

    m_mesh_observer.reset() ;
  }
}
//==========================================================================================
// D�riv�e (en espace, en temps ou mixte selon le type de l'estimateur lu dans le .arc)
//==========================================================================================
void
EstimatorService ::
computeDerivative(const VariableCellReal& variable,const VariableCellReal& variable_tn,Real dt,VariableCellReal & variable_result,IMesh *mesh)
{
  if(m_mesh_observer.hasChanged()||m_first_update) update(mesh) ;
  
  TypesEstimator::eTypesEstimator estimator_type = options()->estimatorType();

  switch (estimator_type)
  {
    case (TypesEstimator::Dx):
      firstOrderSpatialDerivative(variable, variable_result, mesh);
      break;
    case (TypesEstimator::Dt):
      firstOrderTimeDerivative(variable, variable_tn, dt, variable_result, mesh);
      break;
    case (TypesEstimator::DtDx):
      mixedDerivative(variable, variable_tn, dt, variable_result, mesh);
      break;
    default:
      cout << "This case does not exist ! \n";
  }
}

//==========================================================================================
// D�riv�e en espace
//==========================================================================================
void 
EstimatorService ::
firstOrderSpatialDerivative(const  VariableCellReal& variable,VariableCellReal & variable_in_space, IMesh *mesh)  // variable ---> saturation,pression...etc
{

  IAppServiceMng * app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());
  m_geometry_service= app_service_mng->find<IGeometryMng>(true);

  // On r�cup�re les centres des faces et des cellules

  const IGeometryMng::Real3Variable& f_centers = m_geometry_service->getReal3VariableProperty(mesh->allFaces(), IGeometryProperty::PCenter);
  const IGeometryMng::Real3Variable& c_centers  = m_geometry_service->getReal3VariableProperty(mesh->allCells(), IGeometryProperty::PCenter);

  //const IGeometryMng::RealVariable& c_measures = m_geometry_service->getRealVariableProperty(m_cells, IGeometryProperty::PMeasure);
  //const IGeometryMng::RealVariable& f_measures = m_geometry_service->getRealVariableProperty(m_faces, IGeometryProperty::PMeasure);
  // D�riv�e en espace
  variable_in_space.fill(0.);
  m_counter.fill(0);

  ENUMERATE_FACE(iFace,mesh->allActiveFaces())
  {
    const Arcane::Face& face = *iFace ;
    const Cell&  bcell= face.backCell();
    const Cell&  fcell = face.frontCell();

    // Les centres des mailles
    if(!face.isSubDomainBoundary())
    {
      const Real3 & C0 = c_centers[bcell];
      const Real3 & C1 = c_centers[fcell];
      const Real3 & CF = f_centers[face];

      // La difference entre les centres
      Real3 s0F = CF - C0;
      Real3 s1F = CF - C1;
      //Real3 s01 = C1 - C0;

      // La distance
      const Real d0F = math::abs(math::scaMul(s0F,s0F));
      const Real d1F = math::abs(math::scaMul(s1F,s1F));
      Real d01 = math::sqrt(d0F+d1F);

      // Le calcul de la d�riv�e

      variable_in_space[bcell]+=math::abs(variable[bcell] - variable[fcell])/d01 ;
      variable_in_space[fcell]+=math::abs(variable[fcell] - variable[bcell])/d01 ;
      m_counter[bcell]++;
      m_counter[fcell]++;
      if (variable_in_space[bcell]>10)
        cout << "vb "<<variable[bcell]<<" vf "<<variable[bcell]<< " d01 "<<d01<< " h "<< m_h_size[bcell]<<" count "<<m_counter[bcell] << endl;
      if (variable_in_space[fcell]>10)
        cout << "vb "<<variable[bcell]<<" vf "<<variable[bcell]<< " d01 "<<d01<< " h "<< m_h_size[fcell]<<" count "<<m_counter[fcell] << endl;
    }
    else
    {
      if(bcell.localId() != -1)
        m_counter[bcell]++;
      else if(fcell.localId() != -1)
        m_counter[fcell]++;
    }
  }

  ENUMERATE_CELL(icell,mesh->allActiveCells())
  {
      variable_in_space[icell] *= m_h_size[icell]/m_counter[icell];
      if (variable_in_space[icell]>10)
        cout << "e "<<variable_in_space[icell]<<"vb "<<variable[icell]<<" v "<< " h "<< m_h_size[icell]<<" count "<<m_counter[icell] << endl;
 }


}

//================================================================================================
// D�riv�e en temps
//================================================================================================
void
EstimatorService::firstOrderTimeDerivative(const VariableCellReal& variable,const VariableCellReal& variable_tn,Real dt,VariableCellReal & variable_dt,IMesh *mesh)
{

	ENUMERATE_CELL(icell,mesh->allActiveCells())
       {
           variable_dt[icell]= math::abs(variable[icell] - variable_tn[icell])/dt;
       }

}


//===============================================================================================
//La D�riv�e Mixte en Espace et en Temps
//===============================================================================================
void EstimatorService::mixedDerivative(const VariableCellReal& variable,const VariableCellReal& variable_tn,Real dt,VariableCellReal & variable_mixed,IMesh *mesh)
{
  firstOrderSpatialDerivative(variable,variable_mixed,mesh) ;  // 1 - On d�rive la variable � l'instant pr�c�dent en espace

  VariableCellReal tmp = VariableBuildInfo(mesh,"tmp"); //2- On derive la variable � l'instant tn en espace
  firstOrderSpatialDerivative(variable_tn,tmp,mesh);

  firstOrderTimeDerivative(variable_mixed,tmp,dt,variable_mixed,mesh);   //3- Enfin la d�rive en temps --> la deriv�e seconde

}

//=================================================================================================
//	La NORME
//=================================================================================================

Real EstimatorService::normL2(IMesh *mesh,const VariableCellReal & estimateur)
{

  Real tmp = 0.;

  ENUMERATE_CELL(icell,mesh->allActiveCells())
  {
	  tmp += (estimateur[icell]*estimateur[icell]);
  }

  tmp=math::sqrt(tmp);
  return tmp ;

}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
Real
EstimatorService::
_computeCell3DDiameter(const Cell& cell,const IGeometryMng::RealVariable& c_measures,const IGeometryMng::RealVariable& f_measures) {


    const Real cell_measure = c_measures[cell];
    Real min_face_measure = std::numeric_limits<Real>::max();
    Real max_face_measure = -std::numeric_limits<Real>::max();

    for(FaceEnumerator iface(cell.faces()); iface.hasNext(); ++iface) {
      const Face& face = *iface;
      min_face_measure = std::min(min_face_measure, f_measures[face]);
      max_face_measure = std::max(max_face_measure, f_measures[face]);
    }

    const Real hmin = 3*cell_measure / max_face_measure;
    const Real hmax = 3*cell_measure / min_face_measure;

  return std::max(1/hmin,hmax);
}
//==============================================
// L'enregistrement du service
//==============================================
ARCANE_REGISTER_SERVICE_ESTIMATOR(Estimator,EstimatorService);

