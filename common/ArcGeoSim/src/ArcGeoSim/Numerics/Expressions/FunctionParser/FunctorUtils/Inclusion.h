// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*!
 * \file Inclusion.h
 * \brief Include de tous les fichier de FunctorUtils
 * \date 17/04/2008
 * \author Xavier TUNC
 */

#define USE_TRIGO

#include "Utils/Expr.h"
#include "Type/Array.h"
#include "Type/Fad.h"
#include "Type/Real3.h"
#include "Utils/Cst.h"
#include "Utils/Promote.h"
#include "Utils/BinaryOp.h"
#ifdef USE_TRIGO
#include "Functions/MathFunc.h"
#include "Functions/LogFunc.h"
#endif
