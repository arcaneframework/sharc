// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
//
// Created by encheryg on 30/09/2026.
//

#pragma once

#include "ArcGeoSim/Utils/ArcGeoSim.h"

// -- ADD: structure describing a planar sub-face of a non-planar Arcane face.
//    It explicitly provides area, unit normal and center of the sub-face.
struct PlanarSubFaceInfo
{
  //! Area of the planar sub-face
  Arcane::Real measure;
  //! Unit normal of the sub-face (orientation defined by the implementation)
  Arcane::Real3 normal;
  //! Center (barycenter) of the planar sub-face
  Arcane::Real3 center;
};
