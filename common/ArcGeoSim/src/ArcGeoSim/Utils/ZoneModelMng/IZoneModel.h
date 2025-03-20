// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ZONEMODELMNG_IZONEMODEL_H
#define ARCGEOSIM_ZONEMODELMNG_IZONEMODEL_H

#include <arcane/utils/String.h>

class IZoneModel
{
public:
  /** Constructeur de la classe */
  IZoneModel() 
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~IZoneModel() { }
  
public:
  //! Type repr�sentant le type d'un mod�le
  typedef const char * ModelType;

  struct ModelComparator {
    bool operator()(const ModelType & a, const ModelType & b) const
    {
      if (b == NULL) return false;
      if (a == NULL) return true;
      return (strcmp(a,b)<0);
    }
  };

  // Alternative ModelType type
//   typedef Arcane::String ModelType;

//   struct ModelComparator {
//     bool operator()(const ModelType & a, const ModelType & b) const
//     {
//       if (b.null()) return false;
//       if (a.null()) return true;
//       return (a<b);
//     }
//   };

public:
  //! Initialisation
  virtual void init() = 0;

  //! Type de ce mod�le
  /*! Les mod�les sont regroup�s suivant leur type */
  virtual ModelType modelType() = 0;
};

template<typename T>
struct ZoneModelType {
  //! This method forces static interface on models
  // static IZoneModel::ModelType modelType() { return "notype"; }
};


#endif /* ARCGEOSIM_ZONEMODELMNG_IZONEMODEL_H */
