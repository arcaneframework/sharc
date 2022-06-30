// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_PHYSICS_GUMP_ICONVERTER_H
#define ARCGEOSIM_PHYSICS_GUMP_ICONVERTER_H
/* Author : desrozis at Mon Jun 29 16:05:37 2015
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Gump {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IConverter
{
protected:

  IConverter() {}

public:

  virtual ~IConverter() {}

  virtual Arcane::Integer undefined() const = 0;

  virtual Arcane::Integer convert(Arcane::String label) const = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_PHYSICS_GUMP_ICONVERTER_H */
