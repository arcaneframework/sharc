// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include <vector>

#include "DivKGradTwoPointsService.h"

#include "ArcGeoSim/Appli/IAppServiceMng.h"

#include "ArcGeoSim/Utils/ItemGroupBuilder.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::init() 
{
  if( m_status & DiscreteOperatorProperty::S_INITIALIZED) 
    {
      return;
    }

  // Retrieve and initialize application service manager
  IAppServiceMng* app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());
  
  // Retrieve shared geometry service  
  m_geometry_service = app_service_mng->find<IGeometryMng>(true);

  m_cells_group_name = IMPLICIT_UNIQ_NAME;
  m_faces_group_name = IMPLICIT_UNIQ_NAME;

  m_status |= DiscreteOperatorProperty::S_INITIALIZED;
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::prepare(IMesh * mesh,
                                       CoefficientArrayT<Cell> * cell_coefficients,
                                       CoefficientArrayT<Face> * face_coefficients) 
{
  ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_INITIALIZED), 
                 ("Operator not initialized when calling prepare") );

  m_cells          = mesh->allCells();
  m_faces          = mesh->allFaces();
  m_internal_faces = mesh->allCells().innerFaceGroup();
  m_boundary_faces = mesh->allCells().outerFaceGroup();
 
  // Initialize containers
  m_cell_coefficients = cell_coefficients;
  m_face_coefficients = face_coefficients;
  
  // Prepare stencils
  SharedArray<std::pair<ItemGroup, Integer> > c_stencil_sizes(2);
  c_stencil_sizes[0].first  = m_internal_faces;
  c_stencil_sizes[0].second = 2;
  c_stencil_sizes[1].first  = m_boundary_faces;
  c_stencil_sizes[1].second = 1;

  SharedArray<std::pair<ItemGroup, Integer> > f_stencil_sizes(2);
  f_stencil_sizes[0].first  = m_internal_faces;
  f_stencil_sizes[0].second = 0;
  f_stencil_sizes[1].first  = m_boundary_faces;
  f_stencil_sizes[1].second = 1;

  m_cell_coefficients->init(c_stencil_sizes);
  m_face_coefficients->init(f_stencil_sizes);

  // Compute stencils
  if( !(m_options & DiscreteOperatorProperty::O_DISABLE_STENCIL_COMPUTATION) )
    {
      ENUMERATE_FACE(iF, m_internal_faces) {
        const Face& F = *iF;

        ArrayView<Integer> stencil_F = m_cell_coefficients->stencilLocalId(F);
        stencil_F[0] = F.backCell().localId();
        stencil_F[1] = F.frontCell().localId();
      }
  
      ENUMERATE_FACE(iF, m_boundary_faces) {
        const Face& F = *iF;

        ArrayView<Integer> c_stencil_F = m_cell_coefficients->stencilLocalId(F);
        ArrayView<Integer> f_stencil_F = m_face_coefficients->stencilLocalId(F);

        c_stencil_F[0] = F.boundaryCell().localId();
        f_stencil_F[0] = F.localId();
      }
    }

  m_status |= DiscreteOperatorProperty::S_PREPARED;
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::prepare(const FaceGroup & internal_faces,
                                       const FaceGroup & boundary_faces,
                                       FaceGroup & c_internal_faces,
                                       FaceGroup & cf_internal_faces,
                                       CoefficientArrayT<Cell>* cell_coefficients,
                                       CoefficientArrayT<Face>* face_coefficients) 
{
  ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_INITIALIZED), 
                 ("Operator not initialized when calling prepare") );

  m_internal_faces = internal_faces;
  m_boundary_faces = boundary_faces;

  IMesh * mesh = m_internal_faces.mesh();
  m_cells = mesh->allCells(); 
  m_faces = mesh->allFaces();
  
  // Initialize containers
  m_cell_coefficients = cell_coefficients;
  m_face_coefficients = face_coefficients;

  // c internal faces
  ItemGroupBuilder<Face> c_internal_faces_builder(m_internal_faces.mesh(), 
                                                  c_internal_faces.name());
  c_internal_faces_builder.add(m_internal_faces.enumerator());
  m_c_internal_faces = c_internal_faces_builder.buildGroup();  

  // cf internal faces
  ItemGroupBuilder<Face> cf_internal_faces_builder(m_internal_faces.mesh(),
                                                   cf_internal_faces.name());
  m_cf_internal_faces = cf_internal_faces_builder.buildGroup(); // Empty group

  ItemGroupBuilder<Cell> cells_builder(m_internal_faces.mesh(), m_cells_group_name);

//   c_internal_faces = m_internal_faces;

  // Prepare stencils
  SharedArray<std::pair<ItemGroup, Integer> > c_stencil_sizes(2);
  c_stencil_sizes[0].first  = m_internal_faces;
  c_stencil_sizes[0].second = 2;
  c_stencil_sizes[1].first  = m_boundary_faces;
  c_stencil_sizes[1].second = 1;

  SharedArray<std::pair<ItemGroup, Integer> > f_stencil_sizes(2);
  f_stencil_sizes[0].first  = m_internal_faces;
  f_stencil_sizes[0].second = 0;
  f_stencil_sizes[1].first  = m_boundary_faces;
  f_stencil_sizes[1].second = 1;

  m_cell_coefficients->init(c_stencil_sizes);
  m_face_coefficients->init(f_stencil_sizes);

  // Compute stencils
  if( !(m_options & DiscreteOperatorProperty::O_DISABLE_STENCIL_COMPUTATION) )
    {
      ENUMERATE_FACE(iF, m_internal_faces) {
        const Face& F = *iF;

        ArrayView<Integer> stencil_F = m_cell_coefficients->stencilLocalId(F);
        stencil_F[0] = F.backCell().localId();
        stencil_F[1] = F.frontCell().localId();
      }
  
      ENUMERATE_FACE(iF, m_boundary_faces) {
        const Face& F = *iF;

        ArrayView<Integer> c_stencil_F = m_cell_coefficients->stencilLocalId(F);
        ArrayView<Integer> f_stencil_F = m_face_coefficients->stencilLocalId(F);

        c_stencil_F[0] = F.boundaryCell().localId();
        f_stencil_F[0] = F.localId();
      }
    }

  m_status |= DiscreteOperatorProperty::S_PREPARED;
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::finalize() 
{
  ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_PREPARED),
                 ("Operator not prepared when calling finalize") );
  m_properties = DiscreteOperatorProperty::P_NONE;
  m_options    = DiscreteOperatorProperty::O_NONE;
  m_status     = DiscreteOperatorProperty::S_INITIALIZED;
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::formDiscreteOperator(const VariableCellReal& k) 
{
  _form_discrete_operator(k);
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::formDiscreteOperator(const VariableCellReal3& k) 
{
  _form_discrete_operator(k);
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::formDiscreteOperator(const VariableCellReal3x3& k) 
{
  _form_discrete_operator(k);
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::formDiscreteOperator(const SharedVariableCellReal& k) 
{
  _form_discrete_operator(k);
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::formDiscreteOperator(const SharedVariableCellReal3& k) 
{
  _form_discrete_operator(k);
}

/*---------------------------------------------------------------------------*/

void DivKGradTwoPointsService::formDiscreteOperator(const SharedVariableCellReal3x3& k) 
{
  _form_discrete_operator(k);
}

ARCANE_REGISTER_SERVICE_DIVKGRADTWOPOINTS(DivKGradTwoPoints,
                                          DivKGradTwoPointsService);
