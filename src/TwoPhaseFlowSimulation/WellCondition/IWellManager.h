//
// Created by yousefs on 05/12/2022.
//

#ifndef WELLCONDITION_IWELLMANAGER_H
#define WELLCONDITION_IWELLMANAGER_H

#include "WellEnumerator.h"

#include "ArcRes/Entities/System.h"
#include "ArcGeoSim/Physics/Gump/Property.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IWellManager
{
public:

    typedef WellEnumerator Enumerator;

    virtual ~IWellManager() {}

public:

  // Initialisation de la condition
  // ie enregistrement des propriétés, création des variables et
  // remplissage au temps courant
  virtual void init(ArcRes::System& system, Law::VariableCellFolder& domain) = 0;

  // Mise à jour de la condition
  // ie remplissage au temps courant
  virtual void update(ArcRes::System& system) = 0;

  // Renvoie un enumérateur sur les wells
  virtual Enumerator enumerator() const = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define ENUMERATE_WELL(e, c) \
  for(IWellManager::Enumerator e = c; e.hasNext(); ++e)


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif //WELLCONDITION_IWELLMANAGER_H
