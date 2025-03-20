// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_UTILS_VARIABLEITEMARRAY2_H
#define ARCGEOSIM_UTILS_VARIABLEITEMARRAY2_H

// A completer �ventuellement avec autres item...

#include "ArcGeoSim/Utils/MeshVariableArray2RefT.h"

typedef MeshVariableArray2RefT<Node,Real> VariableNodeArray2Real;
typedef MeshVariableArray2RefT<Node,Real2> VariableNodeArray2Real2;
typedef MeshVariableArray2RefT<Node,Real3> VariableNodeArray2Real3;
typedef MeshVariableArray2RefT<Node,Real2x2> VariableNodeArray2Real2x2;
typedef MeshVariableArray2RefT<Node,Real3x3> VariableNodeArray2Real3x3;
typedef MeshVariableArray2RefT<Node,Integer> VariableNodeArray2Integer;
typedef MeshVariableArray2RefT<Node,Int32> VariableNodeArray2Int32;
typedef MeshVariableArray2RefT<Node,Int64> VariableNodeArray2Int64;
typedef MeshVariableArray2RefT<Node,Byte> VariableNodeArray2Byte;
typedef MeshVariableArray2RefT<Node,Byte> VariableNodeArray2Bool;

typedef MeshVariableArray2RefT<DualNode,Real> VariableDualNodeArray2Real;
typedef MeshVariableArray2RefT<DualNode,Real2> VariableDualNodeArray2Real2;
typedef MeshVariableArray2RefT<DualNode,Real3> VariableDualNodeArray2Real3;
typedef MeshVariableArray2RefT<DualNode,Real2x2> VariableDualNodeArray2Real2x2;
typedef MeshVariableArray2RefT<DualNode,Real3x3> VariableDualNodeArray2Real3x3;
typedef MeshVariableArray2RefT<DualNode,Integer> VariableDualNodeArray2Integer;
typedef MeshVariableArray2RefT<DualNode,Int32> VariableDualNodeArray2Int32;
typedef MeshVariableArray2RefT<DualNode,Int64> VariableDualNodeArray2Int64;
typedef MeshVariableArray2RefT<DualNode,Byte> VariableDualNodeArray2Byte;
typedef MeshVariableArray2RefT<DualNode,Byte> VariableDualNodeArray2Bool;

#include "ArcGeoSim/Utils/MeshVariableArray2NoUniformRefT.h"

typedef MeshVariableArray2NoUniformRefT<Node,Real> VariableNodeArray2NoUniformReal;
typedef MeshVariableArray2NoUniformRefT<Node,Real2> VariableNodeArray2NoUniformReal2;
typedef MeshVariableArray2NoUniformRefT<Node,Real3> VariableNodeArray2NoUniformReal3;
typedef MeshVariableArray2NoUniformRefT<Node,Real2x2> VariableNodeArray2NoUniformReal2x2;
typedef MeshVariableArray2NoUniformRefT<Node,Real3x3> VariableNodeArray2NoUniformReal3x3;
typedef MeshVariableArray2NoUniformRefT<Node,Integer> VariableNodeArray2NoUniformInteger;
typedef MeshVariableArray2NoUniformRefT<Node,Int32> VariableNodeArray2NoUniformInt32;
typedef MeshVariableArray2NoUniformRefT<Node,Int64> VariableNodeArray2NoUniformInt64;
typedef MeshVariableArray2NoUniformRefT<Node,Byte> VariableNodeArray2NoUniformByte;
typedef MeshVariableArray2NoUniformRefT<Node,Byte> VariableNodeArray2NoUniformBool;

typedef MeshVariableArray2NoUniformRefT<DualNode,Real> VariableDualNodeArray2NoUniformReal;
typedef MeshVariableArray2NoUniformRefT<DualNode,Real2> VariableDualNodeArray2NoUniformReal2;
typedef MeshVariableArray2NoUniformRefT<DualNode,Real3> VariableDualNodeArray2NoUniformReal3;
typedef MeshVariableArray2NoUniformRefT<DualNode,Real2x2> VariableDualNodeArray2NoUniformReal2x2;
typedef MeshVariableArray2NoUniformRefT<DualNode,Real3x3> VariableDualNodeArray2NoUniformReal3x3;
typedef MeshVariableArray2NoUniformRefT<DualNode,Integer> VariableDualNodeArray2NoUniformInteger;
typedef MeshVariableArray2NoUniformRefT<DualNode,Int32> VariableDualNodeArray2NoUniformInt32;
typedef MeshVariableArray2NoUniformRefT<DualNode,Int64> VariableDualNodeArray2NoUniformInt64;
typedef MeshVariableArray2NoUniformRefT<DualNode,Byte> VariableDualNodeArray2NoUniformByte;
typedef MeshVariableArray2NoUniformRefT<DualNode,Byte> VariableDualNodeArray2NoUniformBool;

#endif /* ARCGEOSIM_UTILS_VARIABLEITEMARRAY2_H */
