// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_PHYSICS_UNITS_SIUNITSCONVERTER_H
#define ARCGEOSIM_ARCGEOSIM_PHYSICS_UNITS_SIUNITSCONVERTER_H
/* Author : desrozis at Wed Nov  5 10:09:27 2014
 * Generated by createNew
 */

#ifdef WIN32
#include <ciso646>
#endif

#include "ArcGeoSim/Appli/AppService.h"

#include "ArcGeoSim/Physics/Units/IUnitsSystem.h"

#include "ArcGeoSim/Utils/Optional.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class SIUnitsConverter
{
public:
  
  SIUnitsConverter()
    : m_system(NULL)
    , m_unit(IUnitsSystemProperty::Unknown) {}

  SIUnitsConverter(IUnitsSystem* system)
    : m_system(system)
    , m_unit(IUnitsSystemProperty::Unknown)
  {
    ARCANE_ASSERT((m_system != NULL),("IUnitsSystem pointer null"));

    m_system->init();
  }
  
  SIUnitsConverter(const Arcane::CaseOptionMultiServiceT<IUnitsSystem>& option)
    : m_system(NULL)
    , m_unit(IUnitsSystemProperty::Unknown)
  {
    if(option.size() > 0) {
      m_system = option[0];
      m_system->init();
    }
  }
  
  void select(IUnitsSystem::UnitType unit)
  {
    m_unit = unit;
  }

  SIUnitsConverter& operator[](IUnitsSystem::UnitType unit)
  {
    select(unit);
    return *this;
  }

  Arcane::Real operator[](const Arcane::Real value)
  {
    m_system = _lazy();
    if (m_system)
      return m_system->convert(m_unit, value);
    return value;
  }

  Optional<Arcane::Real> operator[](const Optional<Arcane::Real>& value)
  {
    Optional<Arcane::Real> v;
    if(not value.null())
      v = this->operator[](value());
    return v;
  }

  bool isDefined()
  {
    m_system = _lazy();
    return m_system != NULL;
  }

  Arcane::String longName(IUnitsSystem::UnitType unit)
  {
    m_system = _lazy();
    if(m_system == NULL)
      throw Arcane::FatalErrorException("No units system defined");
    return m_system->getLongName(unit);
  }

  Arcane::Real factor(IUnitsSystem::UnitType unit)
  {
    m_system = _lazy();
    if(m_system == NULL)
      throw Arcane::FatalErrorException("No units system defined");
    return m_system->getMultFactor(unit);
  }

private:

  IUnitsSystem* _lazy()
  {
    if(m_system == NULL) {
      ArcGeoSim::OptionalAppService<IUnitsSystem> units;
      if(units.isAvailable()) {
        m_system = units;
        m_system->init();
      }
    }
    return m_system;
  }

private:

  IUnitsSystem* m_system;

  IUnitsSystem::UnitType m_unit;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_PHYSICS_UNITS_SIUNITSCONVERTER_H */
