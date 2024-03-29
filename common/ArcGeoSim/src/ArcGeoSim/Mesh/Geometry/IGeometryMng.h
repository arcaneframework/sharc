// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_GEOMETRY_IGEOMETRYMNG_H
#define ARCGEOSIM_GEOMETRY_IGEOMETRYMNG_H
/* Author : havep at Wed Nov 14 13:41:31 2007
 * Generated by createNew
 */

#include <arcane/ItemGroup.h>
#include <arcane/ArcaneTypes.h>
#include <arcane/MeshVariable.h>
#include <arcane/VariableTypedef.h>
#include <arcane/utils/String.h>

#include "ArcGeoSim/Utils/ItemGroupMap.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryPolicy.h"
#include "ArcGeoSim/Mesh/Geometry/NoOptimizationGeometryPolicy.h"

using namespace Arcane;

//! Interface for local geometry computations
class IGeometry;

//! Geometric properties
/*! Some of them are contextual. 
 *  For example, the PMeasure of a 3D cell is its PVolume.
 * 
 *  Technical notes: A external container given by the user cannot
 *  be used for anything except the given property.
 *  An internal ItemGroupMap cannot be allocated for a group different 
 *  of the group wished by the user.
 */
struct IGeometryProperty 
{
  enum eProperty
    {
      PNone               = 0,
      PMeasure            = (1 << 0),
      PLength             = (1 << 1),
      PArea               = (1 << 2),
      PVolume             = (1 << 3),
      PCenter             = (1 << 4),
      PMassCenter         = (1 << 5),
      PNormal             = (1 << 6),
      PVolumeSurfaceRatio = (1 << 7),
      PEnd                = (1 << 8) //! Marqueur de dernier type
    };

  enum eStorage
    {
      PNoStorage    = 0,
      PVariable     = (1 << 0),
      PItemGroupMap = (1 << 1)
    };

  static bool isScalar(const eProperty p) 
  {
    return (p & (PMeasure|PLength|PArea|PVolume|PVolumeSurfaceRatio)) != 0;
  }

  static bool isVectorial(const eProperty p) 
  {
    return (p & (PCenter|PMassCenter|PNormal)) != 0;
  }

  static bool isKindIndependent(const eProperty p)
  {
    return (p & (PMeasure|PCenter|PMassCenter|PNormal)) != 0;
  }

  static bool isSurfaceOnlyProperty(const eProperty p)
  {
    return (p & (PMassCenter)) != 0;
  }

  class Enumerator
  {
  public:
    Enumerator() : m_state(1) { }
    eProperty operator*() const { return (eProperty)m_state; }
    void operator++() { m_state <<= 1; }
    bool end() const { return m_state == PEnd; }
    private:
      Integer m_state;
  };

  static String name(const eProperty p) {
    switch (p) {
    case PNone:
      return "None";
    case PMeasure:
      return "Measure";
    case PLength:
      return "Length";
    case PArea:
      return "Area";
    case PVolume:
      return "Volume";        
    case PCenter:
      return "Center";
    case PMassCenter:
      return "Center";
    case PNormal:
      return "Normal";
    case PVolumeSurfaceRatio:
      return "VolumeSurfaceRatio";
    case PEnd:
    default:
      throw FatalErrorException(A_FUNCINFO,"Undefined property");
      return String();
    }
  }

  static String name(const eStorage p) {
    switch (p) {
    case PNoStorage:
      return "NoStorage";
    case PVariable:
      return "Variable";
    case PItemGroupMap:
      return "ItemGroupMap";
    default:
      throw FatalErrorException(A_FUNCINFO,"Undefined storage");
      return String();
    }
  }
};

class IGeometryMng
{
public:
  typedef VariableItemReal RealVariable;
  typedef VariableItemReal3 Real3Variable;
  typedef ItemGroupMapBaseT<Real> RealGroupMap;
  typedef ItemGroupMapBaseT<Real3> Real3GroupMap;

  enum class Dimension
    {
      eUndefined = 0,
      e3Dxyz,
	  e2Dxy,
      e2Dxz,
      e2Dyz,
      e2Dxyz
    };

public:
  /** Constructeur de la classe */
  IGeometryMng() 
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~IGeometryMng() {};
  
public:
  //! Initialisation
  virtual void init() = 0;

  //@{ @name Geometric property management for ItemGroup 

  //! Set geometric properties on a ItemGroup
  virtual void addItemGroupProperty(ItemGroup group, Integer property, Integer storage) = 0;

  //! Set geometric properties on a ItemGroup with an external container (real field)
  virtual void addItemGroupProperty(ItemGroup group, IGeometryProperty::eProperty property, RealVariable var) = 0;

  //! Set geometric properties on a ItemGroup with an external container (real3 field)
  virtual void addItemGroupProperty(ItemGroup group, IGeometryProperty::eProperty property, Real3Variable var) = 0;

  //! Get set of properties of an ItemGroup
  virtual Integer getItemGroupProperty(ItemGroup group) = 0;

  //! Check if a property is activated on an ItemGroup
  virtual bool hasItemGroupProperty(ItemGroup group, Integer property) = 0;

  //! Check if a storage is available on an ItemGroup
  virtual Integer getItemGroupPropertyStorage(ItemGroup group, IGeometryProperty::eProperty property) = 0;

  //@}

  //@{ @name Access to geometric values

  //! Get geometric property values for a Real field
  virtual const RealVariable & getRealVariableProperty(ItemGroup group, IGeometryProperty::eProperty property) = 0;

  //! Get geometric property values for a Real3 field
  virtual const Real3Variable & getReal3VariableProperty(ItemGroup group, IGeometryProperty::eProperty property) = 0;

  //! Get geometric property values for a Real field
  virtual const RealGroupMap & getRealGroupMapProperty(ItemGroup group, IGeometryProperty::eProperty property) = 0;

  //! Get geometric property values for a Real3 field
  virtual const Real3GroupMap & getReal3GroupMapProperty(ItemGroup group, IGeometryProperty::eProperty property) = 0;

  //@}

  //@{ @name global property management 

  //! Update property values for all ItemGroups on all meshes
  virtual void update(IGeometryPolicy * policy) = 0;

  //! Update property values for all ItemGroups on given mesh
  virtual void update(IMesh * mesh, IGeometryPolicy * policy) = 0;

  //! Reset property for all ItemGroups
  virtual void reset() = 0;
  
  //@}

  //! Get underlying geometry
  /*! Null value argument is given for retro compatibility 
   *  (return default geometry for default mesh) */
  virtual IGeometry * geometry(IMesh * mesh = NULL) = 0;

  //! Get dimension associated to this geometry mng
  virtual Dimension dimension() const = 0;

  //! Remove geometry reference to a given mesh
  /*! Used for deleted sub-meshes */
  virtual void unregisterMesh(IMesh * mesh) = 0;

  //! Temporary interface to set tolerance on geometry policy
  virtual void setPolicyTolerance(bool tolerance) = 0;
};

#include <iostream>
inline std::ostream & operator<<(std::ostream & o, const IGeometryMng::Dimension & dimension) {
	switch (dimension) {
	case IGeometryMng::Dimension::eUndefined:
		return o << "Undefined";
	case IGeometryMng::Dimension::e3Dxyz:
		return o << "3Dxyz";
	case IGeometryMng::Dimension::e2Dxy:
		return o << "2Dxy";
	case IGeometryMng::Dimension::e2Dxz:
		return o << "2Dxy";
	case IGeometryMng::Dimension::e2Dyz:
		return o << "2Dyz";
	case IGeometryMng::Dimension::e2Dxyz:
		return o << "2Dxyz";
	}
	throw Arcane::FatalErrorException("Undefined IGeometryMng Dimension");
}

#endif /* ARCGEOSIM_GEOMETRY_IGEOMETRYMNG_H */
