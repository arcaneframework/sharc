// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_ESTIMATOR_ESTIMATORSERVICE_H
#define ARCGEOSIM_MESH_ESTIMATOR_ESTIMATORSERVICE_H

#include <arcane/ArcaneTypes.h>
#include <arcane/VariableTypedef.h>
#include <arcane/IMesh.h>
#include "arcane/ItemTypes.h"
#include <arcane/VariableBuildInfo.h>

namespace Arcane
{
}

using namespace Arcane;

#include "IEstimator.h"

#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Mesh/MeshAdapter/IMeshAdapter.h"

#include "TypesEstimator.h"
#include "Estimator_axl.h"

class EstimatorService
: public ArcaneEstimatorObject
, public IAMRDataModel
{
public:
  /** Constructeur de la classe */
  EstimatorService(const ServiceBuildInfo & sbi)
  : ArcaneEstimatorObject(sbi)
  , IAMRDataModel()
  , m_counter(VariableBuildInfo(mesh(), "counter"))
  , m_geometry_service(NULL)
  , m_first_update(true)
  {
    ;
  }

  virtual ~EstimatorService(); // Destructeur


  void init() {}

  void init(IMesh* mesh) ;

  void registerData(IMeshAdapter* adapter) ;

  void update(IMesh* mesh) ;
public:
  //====================================================================================================
  // Methode globale de calcul des derivees
  //====================================================================================================
  void computeDerivative(
      const VariableCellReal& variable,
      const VariableCellReal& variable_tn,
      Real dt,
      VariableCellReal & variable_result,
      IMesh *mesh);
      
private:
  //====================================================================================================
  // Trois m�thodes de calcul des d�riv�es
  //====================================================================================================
  void firstOrderSpatialDerivative(const VariableCellReal& variable, VariableCellReal & variable_in_space, IMesh *mesh);
  //!
  void firstOrderTimeDerivative(
      const VariableCellReal& variable,
      const VariableCellReal& variable_tn,
      Real dt,
      VariableCellReal & variable_dt,
      IMesh *mesh);
  //!
  void mixedDerivative(
      const VariableCellReal& variable,
      const VariableCellReal& variable_tn,
      Real dt,
      VariableCellReal & variable_mixed,
      IMesh *mesh);
  //====================================================================================================
  // Calcul de la norme L2
  //====================================================================================================
  Real normL2(IMesh *mesh, const VariableCellReal & estimateur);

  //====================================================================================================
  // Calcul du diametre d'une maille en 3D
  //====================================================================================================
  Real _computeCell3DDiameter(const Cell& cell,const IGeometryMng::RealVariable& c_measures,const IGeometryMng::RealVariable& f_measures);

private:
  //=====================================================================================================
  // Les variables
  //=====================================================================================================
  VariableCellInteger    m_counter;
  IGeometryMng *         m_geometry_service;
  IMeshAdapter*          m_mesh_adapter ;
  IMeshAdapter::Observer m_mesh_observer ;
  bool                   m_first_update ;
};

#endif /* ARCGEOSIM_MESH_ESTIMATOR_ESTIMATORSERVICE */
