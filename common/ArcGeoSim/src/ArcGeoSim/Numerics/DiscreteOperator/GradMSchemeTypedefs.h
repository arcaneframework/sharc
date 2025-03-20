// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef GRADMSCHEME_TYPEDEFS_H
#define GRADMSCHEME_TYPEDEFS_H

#include "CoefficientArrayBuilder.h"
#include "StencilBuilder.h"
#include "DivKGradGradMImpl/DivKGradGradMScheme.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef DivKGradGradMScheme<OrderedStencilBuilderT<Cell>, 
                            OrderedStencilBuilderT<Face>,
                            CoefficientArrayBuilderWithInserterT<Cell>, 
                            CoefficientArrayBuilderWithInserterT<Face> > DivKGradGradMSchemeWithOrderedStencil;

typedef DivKGradGradMScheme<CooresCellStencilBuilder,
                            OrderedStencilBuilderT<Face>,
                            CoefficientArrayBuilderWithInserterT<Cell, CooresCellStencilBuilder>,
                            CoefficientArrayBuilderWithInserterT<Face> > DivKGradGradMSchemeWithCooresStencil;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
