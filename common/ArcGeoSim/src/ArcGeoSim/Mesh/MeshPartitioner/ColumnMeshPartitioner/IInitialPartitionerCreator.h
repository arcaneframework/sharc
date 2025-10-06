/*---------------------------------------------------------------------------*/
/* IInitialPartitioner.h                                       (C) 2000-2009 */
/*                                                                           */
/* Interface d'un partitionneur initial.                                     */
/*---------------------------------------------------------------------------*/
#ifndef ARCANE_IINITIALPARTITIONERCREATOR_H
#define ARCANE_IINITIALPARTITIONERCREATOR_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/ArcaneTypes.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IMesh;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Interface d'un partitionneur initial.
 *
 * Le service impl�mentant cette interface est responsable du
 * partitionnement initial des maillages du cas. Ce partitionnement a lieu
 * uniquement lors du d�marrage du cas, juste avant l'initialisation
 * du cas.
 */
class IInitialPartitionerCreator
{
 public:

  virtual ~IInitialPartitionerCreator() {} //!< Lib�re les ressources.

 public:

  virtual void build() = 0;

  virtual void init() = 0;

};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  //ARCANE_IINITIALPARTITIONERCREATOR_H
