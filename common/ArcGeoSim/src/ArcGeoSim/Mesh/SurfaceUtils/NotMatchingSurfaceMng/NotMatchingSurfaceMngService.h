// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_SURFACEUTILS_NOTMATCHINGSURFACEMNGSERVICE_H
#define ARCGEOSIM_SURFACEUTILS_NOTMATCHINGSURFACEMNGSERVICE_H

#include "ArcGeoSim/Mesh/SurfaceUtils/INotMatchingSurfaceBuilder.h"
#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/INotMatchingSurfaceMng.h"

namespace Arcane { }
using namespace Arcane;

// Prédéclaration
class FaceFaceContactFamily;
class NodeFaceContactFamily;

#include "NotMatchingSurfaceMng_axl.h"

BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_SURFACE_NAMESPACE

class NotMatchingSurfaceMngAccessor;

class NotMatchingSurfaceMngService :
  public ArcaneNotMatchingSurfaceMngObject
{
public:
  //! Constructeur de la classe
  NotMatchingSurfaceMngService(const Arcane::ServiceBuildInfo & sbi);
  
  //! Destructeur de la classe
  virtual ~NotMatchingSurfaceMngService();
  
public:
  //! Initialisation
  void init() override;

  //! Add a property set
  void addProperties(Integer property) override;

  //! Get current property set
  Integer getProperties() const override;

  //! Check if existing property
  bool hasProperties(Integer property) const override;

  //! Update properties
  void update();

  //! Change NotMatchingSurfaceMngAccessor build
  INotMatchingSurfaceBuilder * switchBuilder(INotMatchingSurfaceBuilder* builder) override;

  //! Accesseur de propri�t� par type
  const IContactVariable * getPropertyVariable(NotMatchingSurfaceProperty::eProperty property) const override;

  //! Famille des contacts face-face
  FaceFaceContactFamily * facefaceContactFamily() override;

  //! Famille des contacts node-face
  NodeFaceContactFamily * nodefaceContactFamily() override;
  
  //! return allFaceFaceContacts
  FaceFaceContactSupportType allFaceFaceContacts() const override;
  
  //! return allFaces of FaceFaceContactSurface
  FaceSupportType allFaceFaceContactFaces() const override;

  //! Inform is contact distribution is computed
  virtual bool isContactDistributionComputed() const override;
  

private:
  typedef NotMatchingSurfaceMngAccessor Internal;
  friend class NotMatchingSurfaceMngAccessor;

  Internal * m_internal;
  INotMatchingSurfaceBuilder * m_updater;

  Integer m_properties;
  FaceFaceContactFamily * m_faceface_family;
  NodeFaceContactFamily * m_nodeface_family;
  bool m_is_contact_distribution_computed;
};

END_SURFACE_NAMESPACE
END_ARCGEOSIM_NAMESPACE
  
#endif /* ARCGEOSIM_SURFACEUTILS_NOTMATCHINGSURFACEMNGSERVICE_H */
