// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_NUMERICS_UTILS_CONTACTABSTRACTFAMILY_H
#define ARCGEOSIM_ARCGEOSIM_NUMERICS_UTILS_CONTACTABSTRACTFAMILY_H

#include "ArcGeoSim/Numerics/AlienTools/IIndexManager.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/ContactFamily.h"

#include <vector>

/*---------------------------------------------------------------------------*/

BEGIN_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/

/* Dans cette impl�mentation, implicitement le localId du i_�me item est i */
class ContactAbstractFamily
  : public IIndexManager::IAbstractFamily
{
public:
  //! Construit une famille abstraite � partir d'une famille d'Contact
  ContactAbstractFamily(const ContactFamily & family,
      IIndexManager * manager);

  virtual ~ContactAbstractFamily();

public:
  IIndexManager::IAbstractFamily * clone() const { return new ContactAbstractFamily(*this); }

public:
  Integer maxLocalId() const { return m_family.maxLocalId(); }
  void uniqueIdToLocalId(Int32ArrayView localIds, Int64ConstArrayView uniqueIds) const;
  IAbstractFamily::Item item(Integer localId) const;
  SafeConstArrayView<Integer> owners(Int32ConstArrayView localIds) const;
  SafeConstArrayView<Int64> uids(Int32ConstArrayView localIds) const;
  SafeConstArrayView<Int32> allLocalIds() const;

private:
  const ContactFamily & m_family;
  IIndexManager * m_manager;

private:
  static const Integer m_integer_size   = 29;
  static const Int64   m_integer_mask   = (Int64(1)<<m_integer_size)-1;
  static inline Int64 _form_unique_id(const ContactInternal & contact);
};

/*---------------------------------------------------------------------------*/

END_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_INDEXMANAGER_CONTACTABSTRACTFAMILY_H */
