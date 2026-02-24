// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_UTILS_ARCGEOSIM_H
#define ARCGEOSIM_ARCGEOSIM_UTILS_ARCGEOSIM_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *   ArcGeoSim utils header
 *   
 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include <arcane/ArcaneVersion.h>
#include <arcane/utils/ArcanePrecomp.h>
#include <arcane/utils/ArcaneGlobal.h>
#include <arcane/utils/UtilsTypes.h>
#include <arcane/Item.h>
#include <arcane/VariableTypes.h>

#include <arcane/utils/PlatformUtils.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Utils/ContainerProxy/MeshVariableArray2Proxy.h"
#include "ArcGeoSim/Utils/ContainerProxy/MeshPartialVariableArray2Proxy.h"
#include "ArcGeoSim/Utils/ContainerProxy/VariableRefArray3Proxy.h"
#include "ArcGeoSim/Utils/ContainerProxy/Array3Proxy.h"
#include "ArcGeoSim/Utils/ArcGeoSimVersion.h"

// Temporary to know if ArcGeoSimVersion.h exists and can be included.
// Can be removed when ArcGeoSim 2026.1.1 is installed
#define ARCGEOSIM_HAS_VERSION

namespace ArcGeoSim
{
inline Arcane::String getOuputRootPath()
  {
    const Arcane::String env_full_output_path = Arcane::platform::getEnvironmentVariable("ARCANE_OUTPUT_ROOT_PATH");
    if(env_full_output_path.null())
      return Arcane::platform::getCurrentDirectory();
    else
      return env_full_output_path;
  }
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Namespace ArcGeoSim
namespace ArcGeoSim {
  namespace Mesh {}
  namespace Surface {}
  namespace Property {}
  namespace Numerics {}
}

#define BEGIN_ARCGEOSIM_NAMESPACE namespace ArcGeoSim {
#define END_ARCGEOSIM_NAMESPACE }
#define BEGIN_MESH_NAMESPACE namespace Mesh {
#define END_MESH_NAMESPACE }
#define BEGIN_SURFACE_NAMESPACE namespace Surface {
#define END_SURFACE_NAMESPACE }
#define BEGIN_PROPERTY_NAMESPACE namespace Property {
#define END_PROPERTY_NAMESPACE }
#define BEGIN_EXPRESSIONS_NAMESPACE namespace Expressions {
#define END_EXPRESSIONS_NAMESPACE }
#define BEGIN_NUMERICS_NAMESPACE namespace Numerics {
#define END_NUMERICS_NAMESPACE }
#define BEGIN_LAW_NAMESPACE namespace Law {
#define END_LAW_NAMESPACE }
#define BEGIN_AUDI_NAMESPACE namespace audi {
#define END_AUDI_NAMESPACE }


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Types primitifs de donn�es
using Arcane::Real;
using Arcane::Real2;
using Arcane::Real2x2;
using Arcane::Real3;
using Arcane::Real3x3;
using Arcane::Integer;
using Arcane::Int32;
using Arcane::Int64;
using Arcane::String;
using Arcane::Byte;

// Types tableaux
using Arcane::Array;
using Arcane::Array2;
using Arcane::Array3;
using Arcane::ArrayView;
using Arcane::Array2View;
using Arcane::ConstArrayView;
using Arcane::ConstArray2View;
using Arcane::SharedArray;
using Arcane::UniqueArray;

#define ARRAY_MACRO(array_type)                 \
  using Arcane::Real##array_type;               \
  using Arcane::Real2##array_type;              \
  using Arcane::Real2x2##array_type;            \
  using Arcane::Real3##array_type;              \
  using Arcane::Real3x3##array_type;            \
  using Arcane::Integer##array_type;            \
  using Arcane::Int32##array_type;              \
  using Arcane::Int64##array_type;              \
  using Arcane::Bool##array_type;

ARRAY_MACRO(Array);
ARRAY_MACRO(Array2);
ARRAY_MACRO(Array3);
ARRAY_MACRO(ArrayView);
ARRAY_MACRO(Array2View);
ARRAY_MACRO(ConstArrayView);
ARRAY_MACRO(ConstArray2View);
 
#undef ARRAY_MACRO

using Arcane::Item;
using Arcane::Cell;
using Arcane::Edge;
using Arcane::Face;
using Arcane::Node;
#ifndef USE_ARCANE_V3
using Arcane::DualNode;
using Arcane::Link;
#endif
using Arcane::Particle;
using Arcane::DoF;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_UTILS_ARCGEOSIM_H */
