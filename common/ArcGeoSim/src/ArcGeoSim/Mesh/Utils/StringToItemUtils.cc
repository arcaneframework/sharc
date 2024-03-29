// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "StringToItemUtils.h"
/* Author : dechaiss at Thu Oct 14 15:41:55 2010
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/

#include <arcane/utils/NotImplementedException.h>
#include <arcane/utils/TraceInfo.h>

using namespace Arcane;

Arcane::Integer
ArcGeoSim::StringToItemUtils::
stringToItemType(const Arcane::String & string)
{
  // 1D primitive
  if (string == "Line2") return IT_Line2;
  // 2D primitives
  if (string == "Triangle3") return IT_Triangle3;
  if (string == "Quad4") return IT_Quad4;
  if (string == "Pentagon5") return IT_Pentagon5;
  if (string == "Hexagon6") return IT_Hexagon6;
#if ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0)
  if (string == "Heptagon7") return IT_Heptagon7;
  if (string == "Octogon8") return IT_Octogon8;
#endif
  // 3D primitives
  if (string == "Tetra4") return IT_Tetraedron4;
  if (string == "Pyramid5") return IT_Pyramid5;
  if (string == "Pentaedron6") return IT_Pentaedron6;
  if (string == "Hexaedron8") return IT_Hexaedron8;
  if (string == "Heptaedron10") return IT_Heptaedron10;
  if (string == "Octaedron12") return IT_Octaedron12;
  if (string == "HemiHexa7") return IT_HemiHexa7;
  if (string == "HemiHexa6") return IT_HemiHexa6;
  if (string == "HemiHexa5") return IT_HemiHexa5;
  if (string == "AntiWedgeLeft6") return IT_AntiWedgeLeft6;
  if (string == "AntiWedgeRight6") return IT_AntiWedgeRight6;
  if (string == "DiTetra5") return IT_DiTetra5;
#if ARCANE_VERSION > 12201 || (ARCANE_VERSION==12201 && ARCANE_VERSION_BETA > 0)
  if (string == "Enneedron14") return IT_Enneedron14;
  if (string == "Decaedron16") return IT_Decaedron16;
#endif
  throw NotImplementedException(A_FUNCINFO, "Unexpected item kind");
}

/*---------------------------------------------------------------------------*/

Arcane::eItemKind
ArcGeoSim::StringToItemUtils::
stringToItemKind(const Arcane::String & string)
{
  if (string == "cell") return IK_Cell;
  if (string == "face") return IK_Face;
  if (string == "edge") return IK_Edge;
  if (string == "node") return IK_Node;
  if (string == "particle") return IK_Particle;
#ifndef USE_ARCANE_V3
  if (string == "dual_node") return IK_DualNode;
  if (string == "link") return IK_Link;
#endif
  if (string == "global") return IK_Unknown;
  throw NotImplementedException(A_FUNCINFO, "Unexpected item kind");
}

/*---------------------------------------------------------------------------*/

Arcane::String
ArcGeoSim::StringToItemUtils::
itemKindToString(const Arcane::eItemKind & item_kind)
{
  switch (item_kind)
  {
  case IK_Cell:
    return "cell";
  case IK_Face:
    return "face";
  case IK_Edge:
    return "edge";
  case IK_Node:
    return "node";
  case IK_Particle:
    return "particle";
#ifndef USE_ARCANE_V3    
  case IK_DualNode:
    return "dual_node";
  case IK_Link:
    return "link";
#endif
  case IK_Unknown:
    return "global"; // Choice for IXM mesh format.
  default:
    return "unknown";
  }
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
