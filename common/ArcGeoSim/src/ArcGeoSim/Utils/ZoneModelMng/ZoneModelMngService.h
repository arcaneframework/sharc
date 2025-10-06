// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ZONEMODELMNG_ZONEMODELMNGSERVICE_H
#define ARCGEOSIM_ZONEMODELMNG_ZONEMODELMNGSERVICE_H

#include "IZoneModelMng.h"

namespace Arcane { }
using namespace Arcane;

#include "ZoneModelMng_axl.h"
#include <map>
#include <algorithm>
#include <arcane/utils/List.h>

class ZoneModelMngService :
  public ArcaneZoneModelMngObject
{
public:
  /** Constructeur de la classe */
  ZoneModelMngService(const Arcane::ServiceBuildInfo & sbi) : 
    ArcaneZoneModelMngObject(sbi),
    m_initialized(false)
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~ZoneModelMngService() {}
  
public:

  //! Initialisation
  void init();

  //! Ajoute un mod�le sur une zone de maillage
  void addModelZone(ItemGroup group, IZoneModel * model);

  //! Retire un mod�le sur une zone de maillage
  /*! L'association (itemGroup,model) doit etre exacte pour que la 
   *  suppression se passe avec succ�s (return=true) */
  bool removeModelZone(ItemGroup group, IZoneModel * model);

  //! Liste des groupes concern� par le mod�le
  ItemGroupCollection getModelGroups(IZoneModel * model);

  //! Liste des types de mod�les
  ModelTypeEnumerator getModelTypes();

  //! Liste des mod�les concern� par le type de mod�le
  ModelEnumerator getModels(IZoneModel::ModelType model_type);

  //! Retourne l'unique mod�le concern� par le type de mod�le
  IZoneModel * getSingletonModel(IZoneModel::ModelType model_type);

  //! Invalide le mod�le
  /*! Indique que le mod�le a chang�; permet le d�clenchement d'action en r�ponse. */
  void invalidateModel(IZoneModel * model);

  //! Invalide le groupe
  /*! Indique que le groupe a chang�; permet le d�clenchement d'action en r�ponse. */
  void invalidateGroup(ItemGroup group);

private:
  friend class ModelTypeEnumeratorImpl;
  friend class ModelEnumeratorImpl;
  typedef std::map<IZoneModel*,ItemGroupList> ModelZoneMap;
  typedef std::map<IZoneModel::ModelType, ModelZoneMap, IZoneModel::ModelComparator> ModelMap;
  ModelMap m_models;
  bool m_initialized;
};

#endif /* ARCGEOSIM_ZONEMODELMNG_ZONEMODELMNGSERVICE_H */
