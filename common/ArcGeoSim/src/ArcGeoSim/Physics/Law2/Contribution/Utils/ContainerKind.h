// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef LAW_CONTAINERKIND_H
#define LAW_CONTAINERKIND_H
/* Author : desrozis at Thu Mar 26 10:03:19 2015
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

struct ContainerKind
{
  enum eType {
    Variable,
    PartialVariable,
    Array,
    Scalar
  };
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* LAW_CONTAINERKIND_H */
