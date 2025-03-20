// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * IMeshAdapter.h
 *
 */

#ifndef ARCGEOSIM_MESH_MESHADAPTER_IMESHADAPTER_H_
#define ARCGEOSIM_MESH_MESHADAPTER_IMESHADAPTER_H_

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/TraceInfo.h>
#include <arcane/utils/FatalErrorException.h>
#include "ArcGeoSim/Mesh/Estimator/IEstimator.h"

using namespace Arcane;


class IMeshAdapter
{

public:
  class IObserver
  {
  public :
    IObserver() {}
    virtual ~IObserver() {}

    virtual void notify() = 0 ;

    virtual bool hasChanged() const = 0 ;

    virtual void reset() = 0 ;
  };

  class Observer : public IObserver
  {
  public :
    Observer()
    : IObserver()
    , m_changed(false)
    {}

    virtual ~Observer(){}

    void notify()
    {
      m_changed = true ;
    }

    bool hasChanged() const {
      return m_changed ;
    }

    void reset() {
      m_changed = false ;
    }
  protected :
    bool m_changed ;
  };

  //! constructeur de la classe
  IMeshAdapter() {};
  //! destructeur de la classe
  virtual ~IMeshAdapter() {};

public:
  //! initialisation
  virtual void init() = 0;
  virtual void init(IMesh * mesh) = 0;
  virtual Integer maxLevel() = 0 ;
  virtual bool refineOnInit() = 0 ;
  //! methode de calcul
  virtual bool compute(VariableCellReal& sol,
                       VariableCellReal& sol_tn,
                       VariableCellReal& error,
                       Real dt) = 0;

  virtual bool compute(VariableCellReal& error) = 0;

  virtual void update() = 0 ;
  virtual void adapt() = 0 ;
  virtual void refineAllMesh() = 0 ;
  virtual bool flagRefinedCellGroup(CellGroup const& group) = 0 ;

  virtual bool needMeshAdaptation() = 0 ;
  virtual void reset() = 0 ;

  virtual IEstimator* getEstimator() = 0 ;

  virtual void addObserver(IObserver* observer) = 0 ;

  virtual void notifyAll() = 0 ;

  //! ajout d'une propriete scalaire sur les faces a transferer
  virtual void addDataToRegister(String name, VariableFaceReal & data) = 0;
  virtual void addDataToRegister(String name, VariableFaceInteger & data) = 0;
  virtual void addDataToRegister(String name, VariableFaceInt64 & data) = 0;
  //! ajout d'une propriete scalaire sur les mailles a transferer
  virtual void addDataToRegister(String name, VariableCellReal & data) = 0;
  virtual void addDataToRegister(String name, VariableCellReal & data, VariableParticleReal & particle_data) = 0;
  //! ajout d'une propriete vectorielle sur les mailles a transferer
  virtual void addDataToRegister(String name, VariableCellReal3 & data) = 0;
  virtual void addDataToRegister(String name, VariableCellArrayReal3 & data) = 0;
  //! ajout d'une propriete tensorielle sur les mailles a transferer
  virtual void addDataToRegister(String name, VariableCellReal3x3 & data) = 0;
  //! ajout d'une propriete scalaire sur les mailles a transferer
  virtual void addDataToRegister(String name, VariableCellInteger & data) = 0;
  //! ajout d'une propriete tableau de reels sur les mailles a transferer
  virtual void addDataToRegister(String name, VariableCellArrayReal & data) = 0;
  virtual void addDataToRegister(String name, VariableFaceArrayReal & data) = 0;
  virtual void addExtensiveDataToRegister(String name, VariableCellArrayReal & data) = 0;
  //! enregistrer la fonction de transport donnee par l'utilisateur
  virtual void registerTransportFunctor() = 0;

  virtual void registerUnrefinableCellGroup(CellGroup const& group) = 0 ;
  virtual void registerUnrefinableFaceGroup(FaceGroup const& group) = 0 ;


  virtual CellGroup const& getJustAddedCells() const = 0 ;
  virtual CellGroup const& getJustCoarsenCells() const = 0 ;
  virtual CellGroup const& getJustRefinedCells() const = 0 ;
  virtual CellGroup const& getNewActiveCells() const = 0 ;

  virtual bool contains(Arcane::String name) const = 0;
};


class AbstractMeshAdapter : public IMeshAdapter
{
public :
  AbstractMeshAdapter()
  : IMeshAdapter()
  {
    reset() ;
  }

  virtual void init()
  {
    reset() ;
  }

  virtual bool needMeshAdaptation() {
    return m_mesh_need_adaptation ;
  }

  void reset()
  {
    m_mesh_need_adaptation = false ;
  }

  void addObserver(IObserver* observer)
  {
    m_observers.add(observer) ;
  }

  void notifyAll()
  {
    for(Integer i=0;i<m_observers.size();++i)
      m_observers[i]->notify() ;
  }
protected :
  bool m_mesh_need_adaptation ;

  SharedArray<IObserver*> m_observers ;
};

class IAMRDataModel
{
public :
  IAMRDataModel() {}
  virtual ~IAMRDataModel() {}

  virtual void registerData(IMeshAdapter* adapter) {
    throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");
  }
};


#endif /* ARCGEOSIM_MESH_MESHADAPTER_IMESHADAPTER_H_ */
