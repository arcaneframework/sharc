// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_GEOMETRY_IGEOMETRYPOLICY_H
#define ARCGEOSIM_MESH_GEOMETRY_IGEOMETRYPOLICY_H

#include <arcane/ArcaneTypes.h>

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Mesh/ArcGeoSim_meshExport.h"
#else
#define ARCGEOSIM_MESH_EXPORT
#endif 

class NoOptimizationGeometryPolicy;

//! Future interface de la gestion d'�criture de la g�om�trie;
class IGeometryPolicy
{
public:
  /** Destructeur de la classe */
  virtual ~IGeometryPolicy() {}
  
public:
  //! Retourne le contexte courant
  /*! Ne devrait pouvoir �tre appel� qu'� l'int�rieur d'un bloc beginContext / endContext.
   *  La valeur -1 signifie non d�finie.
   */
  virtual Arcane::Int64 context() = 0;
  //! D�marre un contexte dans le IGeometryMng
  virtual void beginContext() = 0;
  //! Arr�te un contexte dans le IGeometryMng
  virtual void endContext() = 0;

public:
  //! Politique par d�faut  
  static NoOptimizationGeometryPolicy * nullPolicy() { return &s_shared_null_policy; }

private:
  //! Politique par d�faut
  static ARCGEOSIM_MESH_EXPORT NoOptimizationGeometryPolicy s_shared_null_policy;
};

#endif /* ARCGEOSIM_MESH_GEOMETRY_IGEOMETRYPOLICY_H */
