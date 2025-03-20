// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_NUMERICS_LINEARALGEBRA2_DIRECTBUILDERSTATS_H
#define ARCGEOSIM_NUMERICS_LINEARALGEBRA2_DIRECTBUILDERSTATS_H


//#include <ALIEN/Scalar/DirectMatrixBuilder.h>
#include "ArcGeoSim/Numerics/AlienTools/IIndexManager.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_NAMESPACE(Alien)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline String stats(const DirectMatrixBuilder& builder, IIndexManager* manager)
{
  std::ostringstream oss;
  oss << "DirectMatrixBuilder statistics:\n";

  if (manager)
  {
    for(auto i = manager->enumerateEntry(); i.hasNext(); ++i)
    {
      oss << "=== Entry '" << (*i).getName() << "' ===\n";
      oss << builder.stats((*i).getOwnIndexes());
    }
    if (manager->enumerateEntry().count() > 1)
    {
      oss << "=== Entry Summary ===\n";
      oss << builder.stats();
    }
  }
  else
  {
    oss << "=== Global Entry ===\n";
    oss << builder.stats();
  }

  return String(oss.str());
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_NUMERICS_LINEARALGEBRA2_DIRECTBUILDERSTATS_H */
