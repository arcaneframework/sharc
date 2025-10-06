// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ZONEMODELMNG_IZONEMODELMNG_H
#define ARCGEOSIM_ZONEMODELMNG_IZONEMODELMNG_H

#include "IZoneModel.h"

#include <arcane/ItemGroup.h>
#include <arcane/utils/Enumerator.h>
#include <arcane/utils/Collection.h>

using namespace Arcane;

class IZoneModelMng
{
public:
  /** Constructeur de la classe */
  IZoneModelMng() 
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~IZoneModelMng() { }
  
public:

  class IModelEnumeratorImpl 
    : public ObjectImpl {
  public:
    virtual bool hasNext() = 0;
    virtual void moveNext() = 0;
    virtual Integer count() = 0;
    virtual IZoneModel * model() = 0;
    virtual ItemGroupCollection itemGroups() = 0;
  };

  class ModelEnumerator {
  public:
    ModelEnumerator(const ModelEnumerator & e) : m_impl(e.m_impl) { }
    ModelEnumerator(IModelEnumeratorImpl * impl) : m_impl(impl) { }
    bool operator()() { return m_impl->hasNext(); }
    void operator++() { return m_impl->moveNext(); }
    Integer count() { return m_impl->count(); }
    IZoneModel * model() { return m_impl->model(); }
    ItemGroupCollection itemGroups() { return m_impl->itemGroups(); }
    ModelEnumerator * operator->() { return this; }
  protected:
    AutoRefT<IModelEnumeratorImpl> m_impl;
  };

  template<typename ZoneModel>
  class ModelEnumeratorT : public ModelEnumerator {
  private:
    friend class IZoneModelMng;
    ModelEnumeratorT(const ModelEnumerator & e)
      : ModelEnumerator(e) { }
  public:
    ModelEnumeratorT(const ModelEnumeratorT & e) 
      : ModelEnumerator(e) { }
    ZoneModel * model() 
    { 
      IZoneModel * model = this->m_impl->model();
      ARCANE_ASSERT((dynamic_cast<ZoneModel*>(model)!=NULL),("Invalid Internal cast"));
      return reinterpret_cast<ZoneModel*>(model); 
    }
  };

  class IModelTypeEnumeratorImpl 
    : public ObjectImpl {
  public:
    virtual bool hasNext() = 0;
    virtual void moveNext() = 0;
    virtual Integer count() = 0;
    virtual IZoneModel::ModelType modelType() = 0;
    virtual ModelEnumerator models() = 0;
  };

  class ModelTypeEnumerator {
  public:
    ModelTypeEnumerator(const ModelTypeEnumerator & e) : m_impl(e.m_impl) { }
    ModelTypeEnumerator(IModelTypeEnumeratorImpl * impl) : m_impl(impl) { }
    bool operator()() { return m_impl->hasNext(); }
    void operator++() { return m_impl->moveNext(); }
    Integer count() { return m_impl->count(); }
    IZoneModel::ModelType modelType() { return m_impl->modelType(); }
    ModelEnumerator models() { return m_impl->models(); }
    ModelTypeEnumerator * operator->() { return this; }
  protected:
    AutoRefT<IModelTypeEnumeratorImpl> m_impl;
  };

public:

  //! Initialisation
  virtual void init() = 0;

  //! Ajoute un mod�le sur une zone de maillage
  virtual void addModelZone(ItemGroup group, IZoneModel * model) = 0;

  //! Retire un mod�le sur une zone de maillage
  /*! L'association (itemGroup,model) doit etre exacte pour que la 
   *  suppression se passe avec succ�s (return=true) */
  virtual bool removeModelZone(ItemGroup group, IZoneModel * model) = 0;

  //! Liste des groupes concern� par le mod�le
  virtual ItemGroupCollection getModelGroups(IZoneModel * model) = 0;

  //! Liste des types de mod�les
  virtual ModelTypeEnumerator getModelTypes() = 0;

  //! Liste des mod�les concern� par le type de mod�le
  virtual ModelEnumerator getModels(IZoneModel::ModelType model_type) = 0;

  //! Liste des groupes concern� par le type de mod�le (version template)
  template<typename ZoneModel>
  ModelEnumeratorT<ZoneModel> getModelsT()
  {
    return getModels(ZoneModelType<ZoneModel>::modelType());
  }
  
  //! Retourne l'unique mod�le concern� par le type de mod�le
  virtual IZoneModel * getSingletonModel(IZoneModel::ModelType model_type) = 0;

  //! Retourne l'unique mod�le concern� par le type de mod�le (version template)
  template<typename ZoneModel>
  ZoneModel * getSingletonModelT()
  {
      return dynamic_cast<ZoneModel*>(getSingletonModel(ZoneModelType<ZoneModel>::modelType()));
  }
  



  //! Invalide le mod�le
  /*! Indique que le mod�le a chang�; permet le d�clenchement d'action en r�ponse. */
  virtual void invalidateModel(IZoneModel * model) = 0;

  //! Invalide le groupe
  /*! Indique que le groupe a chang�; permet le d�clenchement d'action en r�ponse. */
  virtual void invalidateGroup(ItemGroup group) = 0;
};

#endif /* ARCGEOSIM_ZONEMODELMNG_IZONEMODELMNG_H */
