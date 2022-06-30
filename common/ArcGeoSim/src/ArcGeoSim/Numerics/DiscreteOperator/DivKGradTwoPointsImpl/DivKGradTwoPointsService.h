// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef DIVKGRADTWOPOINTSSERVICE_H
#define DIVKGRADTWOPOINTSSERVICE_H

#include <arcane/SharedVariable.h>
#include <boost/shared_ptr.hpp>

#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#include "ArcGeoSim/Numerics/DiscreteOperator/DiscreteOperatorProperty.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/IDivKGradDiscreteOperator.h"

#include "ArcGeoSim/Numerics/DiscreteOperator/TensorAlgebra.h"

#include "DivKGradTwoPoints_axl.h"

using namespace Arcane;

/*!
  \class Two-points discretization of the \f$\nabla\cdot(\kappa \nabla\cdot)\f$
  operator
  \author Daniele A. Di Pietro
  \date 2007-08-03
*/
class DivKGradTwoPointsService :
  public IDivKGradDiscreteOperator,
  public ArcaneDivKGradTwoPointsObject {
public:
  //! Constructor
  DivKGradTwoPointsService(const ServiceBuildInfo& sbi) :
    ArcaneDivKGradTwoPointsObject(sbi),
    m_status(DiscreteOperatorProperty::S_NONE),
    m_geometry_service(NULL),
    m_options(DiscreteOperatorProperty::O_NONE),
    m_properties(DiscreteOperatorProperty::P_NONE)
  {
    //
  }

  //! Destructor
  virtual ~DivKGradTwoPointsService()
  {
    // Delete m_face group
  }

  //! Return version number
  virtual VersionInfo versionInfo() const
  {
    return VersionInfo(1, 0, 0);
  }

public:
  //! Initialize the service
  void init();

  void prepare(IMesh * mesh,
               CoefficientArrayT<Cell> * cell_coefficients,
               CoefficientArrayT<Face> * face_coefficients);

  //! Preliminary computations (including connectivity)
  ARCANE_DEPRECATED void prepare(const FaceGroup& internal_faces,
                                 const FaceGroup& boundary_faces,
                                 FaceGroup& c_internal_faces,
                                 FaceGroup& cf_internal_faces,
                                 CoefficientArrayT<Cell>* cell_coefficients,
                                 CoefficientArrayT<Face>* face_coefficients);

  //! Finalize
  void finalize();

  //! Required cell geometric properties
  inline Integer getCellGeometricProperties() {
    return IGeometryProperty::PCenter;
  }
  //! Required face geometric properties
  inline Integer getFaceGeometricProperties() {
    return (IGeometryProperty::PCenter | IGeometryProperty::PNormal);
  }

  // Variables
  //! Form the discrete operator associated with the scalar permeability \f$\kappa\f$
  void formDiscreteOperator(const VariableCellReal& k);
  //! Form the discrete operator associated with the diagonal permeability \f$\kappa\f$
  void formDiscreteOperator(const VariableCellReal3& k);
  //! Form the discrete operator associated with the permeability \f$\kappa\f$
  void formDiscreteOperator(const VariableCellReal3x3& k);

  // Shared variables
  //! Form the discrete operator associated with the scalar permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const SharedVariableCellReal& k);
  //! Form the discrete operator associated with the diagonal permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const SharedVariableCellReal3& k);
  //! Form the discrete operator associated with the permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const SharedVariableCellReal3x3& k);

  inline bool twoPointBoundary() 
  {
    return true;
  }

public:
  //! Return faces
  inline const FaceGroup& faces() const
  {
    return m_faces;
  }
  //! Return cells
  inline const CellGroup& cells() const
  {
    return m_cells;
  }
  //! Return boundary faces
  inline const FaceGroup& boundaryFaces() const
  {
    ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_PREPARED),
                   ("Boundary face group has not been computed/defined") );
    return m_boundary_faces;
  }
  //! Return internal faces
  inline const FaceGroup& internalFaces() const
  {
    ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_PREPARED),
                   ("Internal face group has not been computed/defined") );
    return m_internal_faces;
  }
  //! Return internal faces whose stencil contains cell unknowns only
  inline const FaceGroup& cInternalFaces() const
  {
    ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_PREPARED),
                   ("CInternal face group has not been computed/defined") );
    return m_c_internal_faces;
  }
  //! Return internal faces whose stencil contains both cells and face unknowns
  inline const FaceGroup& cfInternalFaces() const
  {
    ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_PREPARED),
                   ("CFInternal face group has not been computed/defined") );
    return m_cf_internal_faces;
  }
  //! Return the current status
  const Integer & status() const
  {
    return m_status;
  }

private:
  Integer m_status;

  // Geometry service
  IGeometryMng* m_geometry_service;

  // Status flags
  Integer m_options;
  Integer m_properties;

  // Prepared flag
  bool m_prepared;

  // Faces group
  FaceGroup m_faces;
  // Cells group
  CellGroup m_cells;
  // Faces whose flux stencil contains cell unknowns only
  FaceGroup m_internal_faces;
  // Boundary faces group
  FaceGroup m_boundary_faces;
  // Internal faces whose stencil contains cell unknowns only
  FaceGroup m_c_internal_faces;
  // Internal faces whose stencil contains both cells and face unknowns
  FaceGroup m_cf_internal_faces;

  // Cell group name
  String m_cells_group_name;
  // Face group name
  String m_faces_group_name;

  // Cell coefficients
  CoefficientArrayT<Cell>* m_cell_coefficients;
  // Face coefficients
  CoefficientArrayT<Face>* m_face_coefficients;

  // Object ownership, for backward compatibility only
  boost::shared_ptr<CoefficientArrayT<Cell> > m_cell_ownership;
  boost::shared_ptr<CoefficientArrayT<Face> > m_face_ownership;

  template<typename VariableTypeT>
  void _form_discrete_operator(const VariableTypeT & k);
};

/*---------------------------------------------------------------------------*/

template<typename VariableTypeT>
void DivKGradTwoPointsService::_form_discrete_operator(const VariableTypeT & k)
{
  ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_PREPARED),
                 ("Operator not prepared") );

  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;
  
  // Retrieve geometrical properties
  const IGeometryMng::Real3Variable& f_centers = m_geometry_service->getReal3VariableProperty(m_faces, IGeometryProperty::PCenter);
  const IGeometryMng::Real3Variable& f_normals = m_geometry_service->getReal3VariableProperty(m_faces, IGeometryProperty::PNormal);

  const IGeometryMng::Real3Variable& c_centers  = m_geometry_service->getReal3VariableProperty(m_cells, IGeometryProperty::PCenter);

  ENUMERATE_FACE(iF, m_internal_faces) {
    const Face& F = *iF;

    // Retrieve information
    const Cell& T0 = F.backCell();
    const Cell& T1 = F.frontCell();

    const Real3& nF = f_normals[F];

    const Real3& C0 = c_centers[T0];
    const Real3& C1 = c_centers[T1];
    const Real3& CF = f_centers[F];

    Real3 s0F = CF - C0;
    Real3 s1F = CF - C1;
    Real3 s01 = C1 - C0;

    Real d0F = math::abs(math::scaMul(s0F, nF));
    Real d1F = math::abs(math::scaMul(s1F, nF));
    Real d01 = math::scaMul(s01, s01);

    const DiffusionType & k0 = k[T0];
    const DiffusionType & k1 = k[T1];

    Real t0 = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(k0, nF)) / d0F;
    Real t1 = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(k1, nF)) / d1F;

    // Compute transmissivities
    Real nFnF = math::scaMul(nF, nF);
    Real cos2 = (d0F+d1F)*(d0F+d1F)/ (d01 * nFnF);
    Real tau  = math::abs(t0 + t1) ? (t0 * t1 / (t0 + t1)) * cos2 : 0;

    StencilFluxCoeffType tau_F = m_cell_coefficients->coefficients(F);
    tau_F[0] = tau;
    tau_F[1] = -tau;
  }

  // Boundary faces
  ENUMERATE_FACE(iF, m_boundary_faces) {

    const Face& F = *iF;

    // Retrieve information
    const Cell& T0  = F.boundaryCell();

    const Real3& nF = f_normals[F];

    const Real3& C0 = c_centers[T0];
    const Real3& CF = f_centers[F];
    const Real3 s0F = CF - C0;

    Real d0F1 = math::scaMul(s0F, s0F);
    Real d0F2 = math::abs(math::scaMul(s0F, nF));

    const DiffusionType & k0 = k[T0];

    // Compute transmissivities
    Real nFnF =  math::scaMul(nF, nF);
    Real cos2 =  (d0F2 * d0F2) / (d0F1 * nFnF)  ;
    Real tau  =  math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(k0, nF)) / d0F2;
    tau  = tau * cos2;

    StencilFluxCoeffType c_tau_F = m_cell_coefficients->coefficients(F);
    StencilFluxCoeffType f_tau_F = m_face_coefficients->coefficients(F);

    c_tau_F[0] = tau;
    f_tau_F[0] = -tau;
  }
}

#endif
