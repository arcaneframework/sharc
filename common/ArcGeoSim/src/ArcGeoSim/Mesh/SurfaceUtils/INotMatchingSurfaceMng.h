// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCANEDEMO_NOTMATCHINGSURFACETEST_INOTMATCHINGSURFACEMNG_H
#define ARCANEDEMO_NOTMATCHINGSURFACETEST_INOTMATCHINGSURFACEMNG_H

#include <arcane/ArcaneException.h>
#include <arcane/utils/FatalErrorException.h>
using namespace Arcane;

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/Contact.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/ContactGroup.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/ContactVariable.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/NotMatchingSurfaceProperty.h"

//! Pr�-d�claration des familles
class FaceFaceContactFamily;
class NodeFaceContactFamily;

BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_SURFACE_NAMESPACE

//! Future politique pour la gestion externe de update()
class NotMatchingSurfacePolicy { };

class INotMatchingSurfaceBuilder ;

//! Gestionnaire de non-coincidence de surfaces
class INotMatchingSurfaceMng
{
public:
  typedef FaceFaceContact FaceFaceContactType ;
  typedef FaceFaceContactGroup FaceFaceContactSupportType ;
  typedef FaceFaceContactGroup FaceFaceContactSupportViewType ;
  typedef FaceGroup FaceSupportType ;
  typedef FaceGroup FaceSupportViewType ;
  typedef ContactVariableT<FaceFaceContactType,Real3> Real3FaceFaceContactVarType ;
  typedef ContactVariableT<FaceFaceContactType,Real> RealFaceFaceContactVarType ;
  typedef ContactPartialVariableT<FaceFaceContactType,Real3> Real3FaceFaceContactPartialVarType ;
  typedef ContactPartialVariableT<FaceFaceContactType,Real> RealFaceFaceContactPartialVarType ;
  typedef ContactVariableT<FaceFaceContact,std::pair<Real,Real> > RealPairFaceFaceContactVarType;
  typedef NotMatchingSurfaceProperty PropertyElementType ;

  /** Constructeur de la classe */
  INotMatchingSurfaceMng() { }
  
  /** Destructeur de la classe */
  virtual ~INotMatchingSurfaceMng() { }
  
public:
  //! Initialisation
  virtual void init() = 0;

  //! Add a property set
  virtual void addProperties(Integer property) = 0;

  //! Get current property set
  virtual Integer getProperties() const = 0;

  //! Check if existing property
  virtual bool hasProperties(Integer property) const = 0;

  //! Accesseur de propri�t� par template
  template<enum NotMatchingSurfaceProperty::eProperty property>
  const typename NotMatchingSurfaceProperty::Variable<property>::type & getPropertyVariable() const;

  //! Accesseur de propri�t� par type
  virtual const IContactVariable * getPropertyVariable(enum NotMatchingSurfaceProperty::eProperty property) const = 0;

  //! Famille des contacts face-face
  virtual FaceFaceContactFamily * facefaceContactFamily() = 0;
  virtual FaceFaceContactSupportType allFaceFaceContacts() const = 0 ;


  //! Famille des contacts node-face
  virtual NodeFaceContactFamily * nodefaceContactFamily() = 0;

  //! groupe de toutes les faces intervenants dans des FaceFaceContacts (util pour COORES)
  virtual FaceSupportType allFaceFaceContactFaces() const = 0 ;

  //! Update properties
  /*! Use specified builder for filling structures */
  virtual void update() = 0;
  
  //! Switch builder used for filling NotMatchingSurfaceAccessor
  /*! \return previous builder */
  virtual INotMatchingSurfaceBuilder * switchBuilder(INotMatchingSurfaceBuilder* builder) = 0;

  //! Inform is contact distribution is computed
  virtual bool isContactDistributionComputed() const = 0;
};

template<enum NotMatchingSurfaceProperty::eProperty property>
const typename NotMatchingSurfaceProperty::Variable<property>::type & 
INotMatchingSurfaceMng::
getPropertyVariable() const
{
  typedef typename NotMatchingSurfaceProperty::Variable<property>::type VariableType;
  const IContactVariable * variable = getPropertyVariable(property);
  if (variable == NULL) 
    throw FatalErrorException(A_FUNCINFO,String::format("NotMatchingSurfaceProperty {0} undefined",NotMatchingSurfaceProperty::name(property)));
  const VariableType * typed_variable = dynamic_cast<const VariableType*>(variable);
  if (typed_variable == NULL) 
    throw InternalErrorException(A_FUNCINFO,String::format("Cannot cast to real type NotMatchingSurfaceProperty {0}",NotMatchingSurfaceProperty::name(property)));
  return *typed_variable;
}

END_SURFACE_NAMESPACE
END_ARCGEOSIM_NAMESPACE

#endif /* ARCANEDEMO_NOTMATCHINGSURFACETEST_INOTMATCHINGSURFACEMNG_H */
