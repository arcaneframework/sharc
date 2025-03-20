// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * IGeometry2D.h
 *
 *  Created on: Nov 22, 2010
 *      Author: gratienj
 */

#ifndef IGEOMETRY2DMNG_H_
#define IGEOMETRY2DMNG_H_

class IGeometry2DMng
{
public:
  typedef VariableItemReal2 Real2Variable;
  typedef ItemGroupMapBaseT<Real2> Real2GroupMap;

public:
  typedef enum {
    XY,
    YZ,
    XZ
  } eType ;

  /** Constructeur de la classe */
  IGeometry2DMng()
    {
      ;
    }

  /** Destructeur de la classe */
  virtual ~IGeometry2DMng() {};

public:
  //@{ @name Access to geometric values


  //! Get geometric property values for a Real2 field
  virtual const Real2Variable & getReal2VariableProperty(ItemGroup group, IGeometryProperty::eProperty property) = 0;

  //! Get geometric property values for a Real3 field
  virtual const Real2GroupMap & getReal2GroupMapProperty(ItemGroup group, IGeometryProperty::eProperty property) = 0;

  //@}

};
#endif /* IGEOMETRY2DMNG_H_ */
