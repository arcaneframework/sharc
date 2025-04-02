// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_DELEGATE_H
#define ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_DELEGATE_H
#include "ArcGeoSim/Appli/Architecture/ModuleMaster.h"

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Appli/ArcGeoSim_appliExport.h"
#else
#define ARCGEOSIM_APPLI_EXPORT
#endif 

// Outil pour obtenir une delegation aupres du ArcGeoSim::ModuleMaster
// Les fonctionnalites deleguees sont :
// - Gestion de l'affichage du pas de temps (IIterationInformation)
// - Gestion de l'avancement du temps (INextTimeComputer)
// - Gestion de points d'entrees extra (IExtraEntryPoints)
//
// Par exemple : 
//
// class MyTimeInformation 
//   : public ArcGeoSim::Delegate<ArcGeoSim::IIterationInformation>
// {
// public:
//   void timeStepInformation(Arcane::Integer nb_loop) { ... }
// };
//
// MyTimeInformation sera le delegue pour l'affichage des informations dans
// la time-loop
//
// Attention, on peut avoir plusieurs delegues mais seul le premier enregistre
// (i.e. instance cree) sera le delegue effectif. Pour changer de delegation,
// on appelera la methode claimDelegation().

    
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T>
class Delegate
  : public T
{
protected:

  Delegate()
  {
    if(m_unique_delegate == nullptr) {
      m_unique_delegate = this;
    }
  }

public:
  
  virtual ~Delegate() {}

public:

  //! Pour savoir si une delegation a ete donnee
  static bool isEnabled()
  {
    return m_unique_delegate != nullptr;
  }

  //! Pour recuperer le delegue. La cle est une intance du
  // Arcane::ModuleMaster, c-a-d que seul le ArcGeoSim::ModuleMaster
  // peut aujourd'hui acceder aux delegues
  static T& delegate(Arcane::ModuleMaster& key)
  {
    return *m_unique_delegate;
  }

  //! Renvoie vrai si l'instance est le delegue courant
  bool isDelegate() const { return m_unique_delegate == this; }

protected:

  //! Reclame la delegation
  void claimDelegation() 
  {
    m_unique_delegate = this;
  }
  
private:

  static ARCGEOSIM_APPLI_EXPORT T* m_unique_delegate;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//#ifdef ARCGEOSIM_USE_EXPORT
// On initialise le static que à l'export
//#ifdef ArcGeoSim_appli_EXPORTS
//template<typename T>
//T* Delegate<T>::m_unique_delegate = NULL;
//#endif
//#else
//template<typename T>
//T* Delegate<T>::m_unique_delegate = NULL;
//#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_APPLI_ARCHITECTURE_DELEGATE_H */
