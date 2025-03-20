/*
 * DiscretizationStencilBuilder.cc
 *
 *  Created on: Mar 11, 2016
 *      Author: yousefs
 */


#include "DiscretizationStencilBuilder.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void CooresCellDiscretizationStencilBuilder::insert(const Discretization::Face & a_face, const Discretization::Cell & a_item, const Real & a_value)
{
  // Boundary, back and front cell will be inserted later
  if(m_dc->isSubDomainBoundary(a_face)&&(a_item == m_dc->boundaryCell(a_face)))
    return;
  if(!m_dc->isSubDomainBoundary(a_face)&&(a_item == m_dc->backCell(a_face) || a_item == m_dc->frontCell(a_face)))
    return;
  TemporaryDiscretizationStencilType & stencil_face_temp = m_temporary[a_face];
  TemporaryDiscretizationStencilType::iterator i =
    std::lower_bound(stencil_face_temp.begin(), stencil_face_temp.end(), a_item.localId());
  if(i==stencil_face_temp.end() || a_value<*i)
    stencil_face_temp.insert(i, a_item.localId());
}

/*----------------------------------------------------------------------------*/

void CooresCellDiscretizationStencilBuilder::finalize()
{
  const Discretization::ItemGroup & faces = m_coefficient_array->faceGroup();
  ENUMERATE_DISCRETIZATION_FACE(iface, faces) {
    const Discretization::Face & F = *iface;
    DiscretizationStencilLocalIdType stencil_face = m_coefficient_array->stencilLocalId(F);
    DiscretizationStencilLocalIdType::iterator it = stencil_face.begin();
    if(m_dc->isSubDomainBoundary(F)) {
      *(it++) = m_dc->boundaryCell(F).localId();
    } else {
      *(it++) = m_dc->backCell(F).localId();
      *(it++) = m_dc->frontCell(F).localId();
    }
    const TemporaryDiscretizationStencilType & stencil_face_temp = m_temporary[F];
    copy(stencil_face_temp.begin(), stencil_face_temp.end(), it);
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


