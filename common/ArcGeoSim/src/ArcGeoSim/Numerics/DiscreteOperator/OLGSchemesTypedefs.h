// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef OLGSCHEMES_TYPEDEFS_H
#define OLGSCHEMES_TYPEDEFS_H

#include "ArcGeoSim/Numerics/DiscreteOperator/CoefficientArrayBuilder.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/StencilBuilder.h"

#include "ArcGeoSim/Numerics/DiscreteOperator/DivKGradOLGSchemesImpl/DivKGradOScheme.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/DivKGradOLGSchemesImpl/DivKGradLScheme.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/DivKGradOLGSchemesImpl/DivKGradGScheme.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

// O scheme
typedef DivKGradOScheme<OrderedStencilBuilderT<Cell>, 
                        OrderedStencilBuilderT<Face>,
                        CoefficientArrayBuilderWithInserterT<Cell>, 
                        CoefficientArrayBuilderWithInserterT<Face> > DivKGradOSchemeWithOrderedStencil;

SET_OPERATOR_IMPLEMENTATION(DivKGradOSchemeWithOrderedStencil, ImplementationWithInserter);

typedef DivKGradOScheme<OrderedStencilBuilderT<Cell>, 
                        OrderedStencilBuilderT<Face>,
                        DirectCoefficientArrayBuilderT<Cell>, 
                        DirectCoefficientArrayBuilderT<Face> > DivKGradOSchemeWithOrderedStencilAndDirectInserter;

SET_OPERATOR_IMPLEMENTATION(DivKGradOSchemeWithOrderedStencilAndDirectInserter, ImplementationWithoutInserter);

typedef DivKGradOScheme<CooresCellStencilBuilder,
                        OrderedStencilBuilderT<Face>,
                        CoefficientArrayBuilderWithInserterT<Cell, CooresCellStencilBuilder>,
                        CoefficientArrayBuilderWithInserterT<Face> > DivKGradOSchemeWithCooresStencil;

SET_OPERATOR_IMPLEMENTATION(DivKGradOSchemeWithCooresStencil, ImplementationWithInserter);

// L scheme
typedef DivKGradLScheme<OrderedStencilBuilderT<Cell>, 
                        OrderedStencilBuilderT<Face>,
                        CoefficientArrayBuilderWithInserterT<Cell>, 
                        CoefficientArrayBuilderWithInserterT<Face> > DivKGradLSchemeWithOrderedStencil;

SET_OPERATOR_IMPLEMENTATION(DivKGradLSchemeWithOrderedStencil, ImplementationWithInserter);

typedef DivKGradLScheme<OrderedStencilBuilderT<Cell>,
                        OrderedStencilBuilderT<Face>,
                        DirectCoefficientArrayBuilderT<Cell>,
                        DirectCoefficientArrayBuilderT<Face> > DivKGradLSchemeWithOrderedStencilAndDirectInserter;

SET_OPERATOR_IMPLEMENTATION(DivKGradLSchemeWithOrderedStencilAndDirectInserter, ImplementationWithoutInserter);

typedef DivKGradLScheme<CooresCellStencilBuilder,
                        OrderedStencilBuilderT<Face>,
                        CoefficientArrayBuilderWithInserterT<Cell, CooresCellStencilBuilder>,
                        CoefficientArrayBuilderWithInserterT<Face> > DivKGradLSchemeWithCooresStencil;

SET_OPERATOR_IMPLEMENTATION(DivKGradLSchemeWithCooresStencil, ImplementationWithInserter);

// G scheme
typedef DivKGradGScheme<OrderedStencilBuilderT<Cell>, 
                        OrderedStencilBuilderT<Face>,
                        CoefficientArrayBuilderWithInserterT<Cell>, 
                        CoefficientArrayBuilderWithInserterT<Face> > DivKGradGSchemeWithOrderedStencil;

SET_OPERATOR_IMPLEMENTATION(DivKGradGSchemeWithOrderedStencil, ImplementationWithInserter);

typedef DivKGradGScheme<OrderedStencilBuilderT<Cell>, 
                        OrderedStencilBuilderT<Face>,
                        DirectCoefficientArrayBuilderT<Cell>, 
                        DirectCoefficientArrayBuilderT<Face> > DivKGradGSchemeWithOrderedStencilAndDirectInserter;

SET_OPERATOR_IMPLEMENTATION(DivKGradGSchemeWithOrderedStencilAndDirectInserter, ImplementationWithoutInserter);

typedef DivKGradGScheme<CooresCellStencilBuilder,
                        OrderedStencilBuilderT<Face>,
                        CoefficientArrayBuilderWithInserterT<Cell, CooresCellStencilBuilder>,
                        CoefficientArrayBuilderWithInserterT<Face> > DivKGradGSchemeWithCooresStencil;

SET_OPERATOR_IMPLEMENTATION(DivKGradGSchemeWithCooresStencil, ImplementationWithInserter);

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
