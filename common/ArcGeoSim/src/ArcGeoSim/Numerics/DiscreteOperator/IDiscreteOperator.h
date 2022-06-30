// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef IDISCRETEOPERATOR_H
#define IDISCRETEOPERATOR_H

#include <vector>
#include <map>

#include <arcane/IMesh.h>
#include <arcane/VariableTypes.h>
#include <arcane/VariableTypedef.h>

#include "CoefficientArray.h"
#include "ArcGeoSim/Utils/ItemComparator.h"
#include "ArcGeoSim/Utils/ItemGroupBuilder.h"

#include "ArcGeoSim/Numerics/DiscreteOperator/DiscreteOperatorProperty.h"

using namespace Arcane;

class IDiscreteOperator {
 public:
  //! Stencil type
  typedef ItemVectorView StencilType;

  //! Flux coefficient type
  typedef Real FluxCoeffType;
  //! Stencil flux coefficient type
  typedef ArrayView<Real> StencilFluxCoeffType;

 public:
  virtual ~IDiscreteOperator() {};

 public:
  //! Initialize service
  virtual void init() = 0;
  /*! Preliminary computations (including connectivity).
    \param mesh a pointer to IMesh

    \param cell_coefficients data structure containing cell stencils and
    coefficients

    \param face_coefficients data structure containing face stencils and
    coefficients
  */
  virtual void prepare(IMesh * mesh,
                       CoefficientArrayT<Cell> * cell_coefficients,
                       CoefficientArrayT<Face> * face_coefficients) = 0;

  /*! Preliminary computations (including connectivity).

  \param internal_faces internal faces

  \param boundary_faces boundary faces, whose flux stencil contains both
  cell and (boundary) face unknowns and which require a boundary condition
  value to be added to the right-hand side

  \param c_internal_faces the group of internal faces whose flux stencil
  contains cell unknowns only

  \param cf_internal_faces the group of internal faces whose flux stencil
  contains both cell and (boundary) face unknowns

  \param cell_coefficients data structure containing cell stencils and
  coefficients

  \param face_coefficients data structure containing face stencils and
  coefficients
  */
 ARCANE_DEPRECATED virtual void prepare(const FaceGroup& internal_faces,
                                        const FaceGroup& boundary_faces,
                                        FaceGroup& c_internal_faces,
                                        FaceGroup& cf_internal_faces,
                                        CoefficientArrayT<Cell> * cell_coefficients,
                                        CoefficientArrayT<Face> * face_coefficients) = 0;

  //! Reset operator
  virtual void finalize() = 0;

  //! Required cell geometric properties
  virtual Integer getCellGeometricProperties() = 0;
  //! Required face geometric properties
  virtual Integer getFaceGeometricProperties() = 0;

 public:
  //! Return faces
  virtual const FaceGroup& faces() const = 0;
  //! Return cells
  virtual const CellGroup& cells() const = 0;
  //! Return boundary faces
  virtual const FaceGroup& boundaryFaces() const = 0;
  //! Return internal faces
  virtual const FaceGroup& internalFaces() const = 0;
};

#endif
