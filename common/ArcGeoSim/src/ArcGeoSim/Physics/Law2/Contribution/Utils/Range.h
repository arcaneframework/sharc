// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef LAW_RANGE_H
#define LAW_RANGE_H
/* Author : desrozis at Mon Mar 30 14:16:00 2015
 * Generated by createNew
 */

#include <arcane/utils/ArcaneGlobal.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Range
{
public:

  Range():
    m_begin(0),
    m_end(0) {}

  Range(Arcane::Integer begin, Arcane::Integer end):
    m_begin(begin),
    m_end(end) {}

  Arcane::Integer begin() const { return m_begin; }
  Arcane::Integer end() const { return m_end; }

  Arcane::Integer size() const { return m_end - m_begin; }

public:

  Arcane::Integer m_begin;
  Arcane::Integer m_end;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* LAW_RANGE_H */
