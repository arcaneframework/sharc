// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "StencilBuilder.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void CooresCellStencilBuilder::insert(const Face & a_face, const Cell & a_item, const Real & a_value)
{
  // Boundary, back and front cell will be inserted later
  if(a_face.isSubDomainBoundary()&&(a_item==a_face.boundaryCell())) 
    return;
  if(!a_face.isSubDomainBoundary()&&(a_item == a_face.backCell() || a_item == a_face.frontCell()))
    return;
  TemporaryStencilType & stencil_face_temp = m_temporary[a_face];
  TemporaryStencilType::iterator i = 
    std::lower_bound(stencil_face_temp.begin(), stencil_face_temp.end(), a_item.localId());
  if(i==stencil_face_temp.end() || a_value<*i)
    stencil_face_temp.insert(i, a_item.localId());
}

/*----------------------------------------------------------------------------*/

void CooresCellStencilBuilder::finalize()
{
  const FaceGroup & faces = m_coefficient_array->faceGroup();
  ENUMERATE_FACE(iface, faces) {
    const Face & F = *iface;
    StencilLocalIdType stencil_face = m_coefficient_array->stencilLocalId(F);
    StencilLocalIdType::iterator it = stencil_face.begin();
    if(F.isSubDomainBoundary()) {
      *(it++) = F.boundaryCell().localId();
    } else {
      *(it++) = F.backCell().localId();
      *(it++) = F.frontCell().localId();
    }
    const TemporaryStencilType & stencil_face_temp = m_temporary[F];
    copy(stencil_face_temp.begin(), stencil_face_temp.end(), it);
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
