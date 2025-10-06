// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_ICONTACTVARIABLE_H
#define ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_ICONTACTVARIABLE_H

class IContactVariable
{
public:
  /** Constructeur de la classe */
  IContactVariable() { }
  
  /** Destructeur de la classe */
  virtual ~IContactVariable() { }

  //! D�tache de la famille associ�
  /*! Utilis� dans le cas des destructions de la famille avant les variables */
  virtual void detachFromFamily() = 0;
};

#endif /* ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_ICONTACTVARIABLE_H */
