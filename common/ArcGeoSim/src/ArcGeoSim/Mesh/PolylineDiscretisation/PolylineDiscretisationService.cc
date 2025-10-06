// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// Interface du service 
#include "IPolylineDiscretisation.h"
#include "PolylineDiscretisation_axl.h"

#include "ArcGeoSim/Mesh/PolylineDiscretisation/PolylineDiscretisationImpl/PolylineDiscretisation.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class PolylineDiscretisationService: public ArcanePolylineDiscretisationObject
{
public:

  /** Constructeur de la classe */
  PolylineDiscretisationService(const Arcane::ServiceBuildInfo & sbi) :
      ArcanePolylineDiscretisationObject(sbi)
  {
  }

  /** Destructeur de la classe */
  ~PolylineDiscretisationService()
  {
  }

public:
  //! Apply service function
  void init(ArcGeoSim::IMeshAccessorToolsMng* mng);
  //! Apply service function
  void compute(PolylineArray& polylineList,PolylineDiscretisationDataArray& result);

private:
  std::unique_ptr<PolylineDiscretisation> m_discretized_polyline;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void
PolylineDiscretisationService::
init(ArcGeoSim::IMeshAccessorToolsMng* mng)
{
  m_discretized_polyline.reset(new PolylineDiscretisation(mng));
  m_discretized_polyline->init();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


void PolylineDiscretisationService::compute(PolylineArray& polylineList,PolylineDiscretisationDataArray& result)
{

  for (PolylineArray::iterator polyline = polylineList.begin(); polyline != polylineList.end(); polyline++)
  {
	m_discretized_polyline->run(*polyline);
    result.add(m_discretized_polyline->getDiscretisationData());
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_POLYLINEDISCRETISATION(PolylineDiscretisation, PolylineDiscretisationService);
