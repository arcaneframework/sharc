// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "TwoPointsEstimatorService.h"
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

using namespace Arcane;

//=======================================================================================
// Destructeur
//=======================================================================================

TwoPointsEstimatorService::~TwoPointsEstimatorService()
{

}
void TwoPointsEstimatorService::init(IMesh* mesh)
{
  m_mesh = mesh ;

  IAppServiceMng * app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());
  m_geometry_service = app_service_mng->find<IGeometryMng>(true);
  m_geometry_service->addItemGroupProperty(mesh->allFaces(),IGeometryProperty::PCenter,IGeometryProperty::PVariable);
  m_geometry_service->addItemGroupProperty(mesh->allFaces(),IGeometryProperty::PNormal,IGeometryProperty::PVariable);
  m_geometry_service->addItemGroupProperty(mesh->allCells(),IGeometryProperty::PVolume,IGeometryProperty::PVariable);
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

void TwoPointsEstimatorService::registerData(IMeshAdapter* adapter)
{
  m_mesh_adapter = adapter ;
  m_mesh_adapter->addObserver(&m_mesh_observer) ;
  //m_mesh_adapter->registerUnrefinableCellGroup(m_boundary_cell_group) ;
}

void TwoPointsEstimatorService::update(IMesh* mesh)
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
}
//==========================================================================================
// D�riv�e (en espace, en temps ou mixte selon le type de l'estimateur lu dans le .arc)
//==========================================================================================
void TwoPointsEstimatorService::computeDerivative(
    const VariableCellReal& variable,
    const VariableCellReal& variable_tn,
    Real dt,
    VariableCellReal & variable_result,
    IMesh *mesh)
{
  if(m_mesh_observer.hasChanged()||m_first_update) update(mesh) ;
  firstOrderSpatialDerivative(variable, variable_result, mesh);
}

//==========================================================================================
// D�riv�e en espace
//==========================================================================================

void TwoPointsEstimatorService::
firstOrderSpatialDerivative( const VariableCellReal& var,
                             VariableCellReal & err,
                             IMesh *mesh) // variable ---> saturation,pression...etc
{

  // On r�cup�re les centres des faces et des cellules

  const IGeometryMng::Real3Variable& f_centers = m_geometry_service->getReal3VariableProperty(mesh->allFaces(),
      IGeometryProperty::PCenter);

  const IGeometryMng::Real3Variable& f_normals = m_geometry_service->getReal3VariableProperty(mesh->allFaces(),
      IGeometryProperty::PNormal);

  const IGeometryMng::Real3Variable& c_centers = m_geometry_service->getReal3VariableProperty(mesh->allCells(),
      IGeometryProperty::PCenter);

  const IGeometryMng::RealVariable& c_meas = m_geometry_service->getRealVariableProperty(mesh->allCells(),
      IGeometryProperty::PVolume);

  Real3 zero = Real3::zero() ;
  m_grad.fill(zero) ;
  ENUMERATE_FACE(iFace,mesh->allActiveFaces())
  {
    const Face& F = *iFace;
    if (F.isSubDomainBoundary())
    {
      // vF = vT0
      // grad[T0] += measF(vF-vT0)*nF
    }
    else
    {
      // Retrieve information
      const Cell& T0 = F.backCell();
      const Cell& T1 = F.frontCell();

      const Real3& nF = f_normals[F];
      Real nF_norm = nF.abs() ;
      Real3 nF_uni = nF/ nF_norm ;

      const Real3& C0 = c_centers[T0];
      const Real3& C1 = c_centers[T1];
      const Real3& CF = f_centers[F];

      Real3 s0F = CF - C0;
      Real3 s1F = C1 - CF;

      Real d0F = math::abs(math::scaMul(s0F,nF_uni));
      Real C0F_n2 = math::scaMul(s0F,s0F) ;
      Real t0  = d0F/C0F_n2;

      Real d1F = math::abs(math::scaMul(s1F,nF_uni));
      Real C1F_n2 = math::scaMul(s1F,s1F) ;
      Real t1  = d1F/C1F_n2;

      Real vT0 = var[T0] ;
      Real vT1 = var[T1] ;
      Real vF = (t0*vT0+t1*vT1)/(t0+t1) ;

      Real sgn_t0 = sign(F,T0)* nF_norm ;
      m_grad[T0] += (vF - vT0)*sgn_t0*nF ;
      m_grad[T1] -= (vF - vT1)*sgn_t0*nF ;
    }
  }

  ENUMERATE_CELL(icell,mesh->allActiveCells())
  {
    m_grad[icell] /= c_meas[icell] ;
    err[icell] = m_grad[icell].abs()*m_h_size[icell] ;

    Real3& grad = m_grad[icell] ;
    m_gradX[icell] = grad.x ;
    m_gradY[icell] = grad.y ;
    m_gradZ[icell] = grad.z ;
  }
}
//================================================================================================
// D�riv�e en temps
//================================================================================================
void TwoPointsEstimatorService::firstOrderTimeDerivative(
    const VariableCellReal& variable,
    const VariableCellReal& variable_tn,
    Real dt,
    VariableCellReal & variable_dt,
    IMesh *mesh)
{

  ENUMERATE_CELL(icell,mesh->allActiveCells())
  {
    variable_dt[icell] = math::abs(variable[icell] - variable_tn[icell]) / dt;
  }

}

//===============================================================================================
//La D�riv�e Mixte en Espace et en Temps
//===============================================================================================
void TwoPointsEstimatorService::mixedDerivative(
    const VariableCellReal& variable,
    const VariableCellReal& variable_tn,
    Real dt,
    VariableCellReal & variable_mixed,
    IMesh *mesh)
{
  firstOrderSpatialDerivative(variable, variable_mixed, mesh); // 1 - On d�rive la variable � l'instant pr�c�dent en espace

  VariableCellReal tmp = VariableBuildInfo(mesh, "tmp"); //2- On derive la variable � l'instant tn en espace
  firstOrderSpatialDerivative(variable_tn, tmp, mesh);

  firstOrderTimeDerivative(variable_mixed, tmp, dt, variable_mixed, mesh); //3- Enfin la d�rive en temps --> la deriv�e seconde

}

//=================================================================================================
//	La NORME
//=================================================================================================

Real TwoPointsEstimatorService::normL2(IMesh *mesh, const VariableCellReal & estimateur)
{

  Real tmp = 0.;

  ENUMERATE_CELL(icell,mesh->allActiveCells())
  {
    tmp += (estimateur[icell] * estimateur[icell]);
  }

  tmp = math::sqrt(tmp);
  return tmp;

}
//==============================================
// L'enregistrement du service
//==============================================
ARCANE_REGISTER_SERVICE_TWOPOINTSESTIMATOR(TwoPointsEstimator,TwoPointsEstimatorService);

