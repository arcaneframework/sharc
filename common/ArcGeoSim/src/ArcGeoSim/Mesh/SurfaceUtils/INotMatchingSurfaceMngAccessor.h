// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_SURFACEUTILS_INOTMATCHINGSURFACEMNGACCESSOR_H
#define ARCGEOSIM_SURFACEUTILS_INOTMATCHINGSURFACEMNGACCESSOR_H

#include "ArcGeoSim/Mesh/SurfaceUtils/NotMatchingSurfaceProperty.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/ContactFamily.h"
#include <boost/shared_ptr.hpp>

//! Pr�-d�claration


BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_SURFACE_NAMESPACE


class INotMatchingSurfaceMng;

class INotMatchingSurfaceMngAccessor
{
public:
  //! Destructeur de la classe
  virtual ~INotMatchingSurfaceMngAccessor() { }
  
public:
  virtual boost::shared_ptr<IContactVariable> & getPropertyVariable(NotMatchingSurfaceProperty::eProperty property) = 0;
  virtual INotMatchingSurfaceMng * mng() = 0;
  virtual FaceFaceContactFamily *& facefaceContactFamily() = 0;
  virtual NodeFaceContactFamily *& nodefaceContactFamily() = 0;
};

END_SURFACE_NAMESPACE
END_ARCGEOSIM_NAMESPACE
#endif /* ARCGEOSIM_SURFACEUTILS_INOTMATCHINGSURFACEMNGACCESSOR_H */
