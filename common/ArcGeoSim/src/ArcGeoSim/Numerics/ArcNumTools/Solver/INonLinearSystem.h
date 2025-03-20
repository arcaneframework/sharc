// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#pragma once

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#include "ArcGeoSim/Numerics/ArcNumTools/ArcNumTools.h"

#include "ArcGeoSim/Numerics/ArcNumTools/LinearAlgebra/Vector.h"
#include "ArcGeoSim/Numerics/ArcNumTools/LinearAlgebra/Matrix.h"

namespace ArcNum {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Vector;
class Matrix;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<ArcNum::ContainerKind::eType CK, ArcNum::ItemKind::eType IK>
struct INonLinearOperator
{
  using Domain = ArcNum::Domain<CK,IK> ;

  virtual ~INonLinearOperator() {}

  //virtual const Law::PropertyVector& equationSystem() const = 0;
  //virtual const Law::VariableFolder<CK,IK>& folder() const = 0;

  virtual void build(Vector& residual, Matrix& matrix) = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class INonLinearSystem
  : public INonLinearOperator<ArcNum::ContainerKind::Variable, ArcNum::ItemKind::Cell>
{
public:

  struct IScalarEquation
    : public INonLinearOperator<ArcNum::ContainerKind::Scalar, ArcNum::ItemKind::None>
  {
    virtual ~IScalarEquation() {}

    virtual Arcane::Integer owner() const = 0;

    virtual Arcane::CellGroup interface() const = 0;

    virtual void newtonUpdateCallBack(Arcane::Real delta) = 0;

    virtual void build(Vector& residual, Matrix& matrix) = 0;

    /*virtual void build(ArcNum::Vector& residual,
                       ArcNum::Matrix& jacobian,
                       const Geoxim::Corrector& corrector) = 0;*/

  };

public:

  virtual ~INonLinearSystem() {}

  virtual Domain const& systemDomain() const = 0 ;
  virtual Integer nbEquations() const = 0 ;

  // Problemes couples
  virtual Arcane::SharedArray<IScalarEquation*> coupledProblems() const
  {
    return Arcane::SharedArray<IScalarEquation*>();
  }

  virtual void initLinearSystemProfile(Arcane::CellGroup cells,
                                       Alien::MatrixProfiler& blockProfiler,
                                       Arcane::ConstArray2View<Arcane::Integer> indexes,
                                       Arcane::Integer block_size ) = 0 ;

  virtual void setSolutionVariables(Arcane::SharedArray<Arcane::VariableCellReal*>& solutions) = 0 ;

  // Applique les contraintes sur la solution 
  // Par defaut, aucune
  virtual void applyConstraintOnSolution(bool &NonPhysicalSolution) {}

  // Mise a jour des termes explicites
  // Par defaut, ne fait rien (pour portage)
  virtual void updateExplicitTerms() {}

  // Formulation
  // Si la methode n'est pas surchargee, formulation a une equation
  /*virtual const IFormulation& formulation() const
  {
    m_default_formulation = OneEquationSystem(equationSystem());

    return m_default_formulation;
  }*/

private:

  //mutable OneEquationSystem m_default_formulation;
};

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
