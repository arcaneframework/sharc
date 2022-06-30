// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef IDIVKGRADDISCRETEOPERATOR_H
#define IDIVKGRADDISCRETEOPERATOR_H

#include "IDiscreteOperator.h"
#include "arcane/VariableTypedef.h"

class IDivKGradDiscreteOperator : public IDiscreteOperator {
public:
  //! Absolute permeability type
  typedef VariableCellReal3x3 AbsolutePermeabilityType;

public:
  virtual ~IDivKGradDiscreteOperator() {}
  virtual const Integer & status() const = 0;
public:

  //! Two-point boundary treatment
  virtual bool twoPointBoundary() = 0;

  // Variables
  //! Form the discrete operator associated with the scalar permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const VariableCellReal& k) = 0;
  //! Form the discrete operator associated with the diagonal permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const VariableCellReal3& k) = 0;
  //! Form the discrete operator associated with the permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const VariableCellReal3x3& k) = 0 ;

  // Shared variables
  //! Form the discrete operator associated with the scalar permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const SharedVariableCellReal& k) = 0;
  //! Form the discrete operator associated with the diagonal permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const SharedVariableCellReal3& k) = 0;
  //! Form the discrete operator associated with the permeability \f$\kappa\f$
  virtual void formDiscreteOperator(const SharedVariableCellReal3x3& k) = 0 ;


  //! Degenerate node flag
  void setNodeDegenerationVariable(VariableNodeInteger & degenerate_nodes)
  {
    m_degenerate_nodes = & degenerate_nodes;
  }

protected:
  VariableNodeInteger * m_degenerate_nodes;
};

#endif
