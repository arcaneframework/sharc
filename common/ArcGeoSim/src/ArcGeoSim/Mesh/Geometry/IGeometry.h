// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_GEOMETRY_IGEOMETRY_H
#define ARCGEOSIM_GEOMETRY_IGEOMETRY_H

#include <arcane/utils/Real3.h>
#include <arcane/Item.h>
#include <arcane/MathUtils.h>

using namespace Arcane;

class IGeometry
{
public:
  /** Constructeur de la classe */
  IGeometry() 
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~IGeometry() { }
  
public:
  //! Calcul du centre de masse
  virtual Real3 computeCenter(const ItemWithNodes & item) = 0;
  virtual Real3 computeCenter(const ItemWithNodes & item, Array<Real> & coords) = 0;

  //! Calcul de la mesure orient�e
  /*! Dans le cas d'un �l�ment plan, ceci correspond � 
   *  la normale moyenne unitaire * mesure de l'�l�ment
   *  et dans le cas d'un simple �l�ment volumique nous obtenons
   *  volume * z (ou z=(0,0,1))
   */
  virtual Real3 computeOrientedMeasure(const ItemWithNodes & item) = 0;

  //! Calcul de la mesure (sans orientation)
  virtual Real  computeMeasure(const ItemWithNodes & item) = 0;

  //! Calcul de la longueur
  /*! Uniquement pour les Items lin��ques */
  virtual Real  computeLength(const ItemWithNodes & item) = 0;

  //! Calcul de l'aire 
  /*! Uniquement pour les Items surfaciques */
  virtual Real  computeArea(const ItemWithNodes & item) = 0;

  //! Calcul du volume
  /*! Uniquement pour les Items volumiques */
  virtual Real  computeVolume(const ItemWithNodes & item) = 0;

//   //! Calcul du centre
//   /*! Uniquement pour les Items surfaciques */  
//   virtual Real3 computeSurfaceCenter(Integer n, const Real3 * coords) = 0;

  //! Calcul de l'aire orient�e (ie normale)
  /*! Uniquement pour les Items surfaciques */  
  virtual Real3 computeOrientedArea(Integer n, const Real3 * coords) = 0;

  //! Calcul de longueur d'un segment d�fini par deux points
  virtual Real computeLength(const Real3& m, const Real3& n) = 0;
};

#endif /* ARCGEOSIM_GEOMETRY_IGEOMETRY_H */
