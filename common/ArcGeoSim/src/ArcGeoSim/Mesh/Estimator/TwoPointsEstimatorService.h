// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_ESTIMATOR_TWOPOINTSESTIMATORSERVICE_H
#define ARCGEOSIM_MESH_ESTIMATOR_TWOPOINTSESTIMATORSERVICE_H

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

#include "TwoPointsEstimator_axl.h"

class TwoPointsEstimatorService
: public ArcaneTwoPointsEstimatorObject
, public IAMRDataModel
{
public:
  /** Constructeur de la classe */
  TwoPointsEstimatorService(const ServiceBuildInfo & sbi)
  : ArcaneTwoPointsEstimatorObject(sbi)
  , m_counter(VariableBuildInfo(mesh(), "counter"))
  , m_mesh(NULL)
  , m_geometry_service(NULL)
  , m_mesh_adapter(NULL)
  , m_first_update(true)
  {
    ;
  }

  virtual ~TwoPointsEstimatorService(); // Destructeur

public:
  void init() {}

  void init(IMesh* mesh) ;

  void registerData(IMeshAdapter* adapter) ;

  void update(IMesh* mesh) ;

  //void compute(VariableCellReal & error,IMesh *mesh) {}
  //====================================================================================================
  // Methode globale de calcul des derivees
  //====================================================================================================
  void computeDerivative(
      const VariableCellReal& variable,
      const VariableCellReal& variable_tn,
      Real dt,
      VariableCellReal & variable_result,
      IMesh *mesh);
  //====================================================================================================
  // Trois m�thodes de calcul des d�riv�es
  //====================================================================================================
  void firstOrderSpatialDerivative(const VariableCellReal& variable, VariableCellReal & variable_in_space, IMesh *mesh);
  void firstOrderTimeDerivative(
      const VariableCellReal& variable,
      const VariableCellReal& variable_tn,
      Real dt,
      VariableCellReal & variable_dt,
      IMesh *mesh);
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

private:
  Integer sign(Face const& face, Cell const& cell) {
    if(face.backCell()==cell)
      return 1 ;
    else
      return -1 ;
  }
  //=====================================================================================================
  // Les variables
  //=====================================================================================================
  VariableCellInteger    m_counter;
  IMesh*                 m_mesh ;
  IGeometryMng *         m_geometry_service;
  IMeshAdapter*          m_mesh_adapter ;
  IMeshAdapter::Observer m_mesh_observer ;
  bool                   m_first_update ;
  CellGroup              m_boundary_cell_group ;

};

#endif /* ARCGEOSIM_MESH_ESTIMATOR_ESTIMATORSERVICE */
