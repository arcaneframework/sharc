/*
 * CoefficientArray.h
 *
 *  Created on: Mar 11, 2016
 *      Author: yousefs
 */

#ifndef SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONCOEFFICIENTARRAY_H_
#define SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONCOEFFICIENTARRAY_H_

#include <algorithm>
#include <ostream>

#include <arcane/ArcaneTypes.h>
#include <arcane/ItemGroup.h>
#include <arcane/ItemTypes.h>
#include <arcane/utils/Array.h>
#include <arcane/ArcaneVersion.h>
#if (ARCANE_VERSION >= 12201)
#else /* ARCANE_VERSION */
#include <arcane/utils/CArray.h>
#endif /* ARCANE_VERSION */
#include <arcane/utils/String.h>

#include "ArcGeoSim/Utils/ItemGroupMap.h"
#include "ArcGeoSim/Utils/Utils.h"

#include "ArcGeoSim/Numerics/Discretization/DiscretizationGeometry.h"

using namespace Arcane;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
class DiscretizationCoefficientArrayT
{
public:
  /** @name Typedefs and enums
   */
  //@{
  //! Item group type
  typedef typename ItemTraitsT<T>::ItemGroupType ItemGroupType;
  //! Stencil local id type
  typedef ArrayView<Integer> StencilLocalIdType;
  //! Stencil type
  typedef ItemVectorView StencilType;
  //! Coefficients type
  typedef ArrayView<Real> CoefficientsType;
  //@}

  /** @name Constructors and destructors
   */
  //@{
  //! Default constructor
  DiscretizationCoefficientArrayT(DiscretizationConnectivity* dc, Discretization::ItemGroup face_group, ItemGroupT<T> item_group)
  : m_dc(dc), m_face_group(face_group)
{
  // Initialize item internal representation
  m_item_internal = item_group.itemFamily()->itemsInternal();
}
  //@}

  /** @name Constructors and destructors
   */
  //@{
  //! Initialize the coefficient array
  void init(const ItemGroupMapT<typename Discretization::ConvertD2DoF<Discretization::Face>::Type, Integer>& stencil_sizes);
  //! Initialize the coefficient array
  void init(const Array<std::pair<Discretization::ItemGroup, Integer> >& stencil_sizes);
  //! Initialize the coefficient array
  void init(const Arcane::VariableDoFInteger& stencil_sizes);
  //! Destructor
  virtual ~DiscretizationCoefficientArrayT() {};
  //@}

  /** @name Setters and getters
   */
  //@{
  //! Return the face group
//  const Discretization::ItemGroup & faceGroup() const
  const Discretization::ItemGroup & faceGroup() const
  {
    return m_face_group;
  }
  //! Return the number of coefficients stored
  Integer numberOfCoefficients() const;
  //! Return the size of the stencil
  Integer stencilSize(const Discretization::Face& F);
  //! Return the local id of the elements in the stencil of face F
  StencilLocalIdType stencilLocalId(const Discretization::Face& F);
  //! Return the stencil for face F (
  StencilType stencil(const Discretization::Face& F);
  //! Return the coefficients for face F
  CoefficientsType coefficients(const Discretization::Face& F);
  //@}

  /** @name Setters and getters (deprecated)
   */
  //@{
  //! Return the size of the stencil
  ARCANE_DEPRECATED inline Integer getStencilSize(const Discretization::Face& F)
  {
    return this->stencilSize(F);
  }
  //! Return the local id of the elements in the stencil of face F
  ARCANE_DEPRECATED inline StencilLocalIdType getStencilLocalId(const Discretization::Face& F)
  {
    return stencilLocalId(F);
  }
  //! Return the stencil for face F (
  ARCANE_DEPRECATED inline StencilType getStencil(const Discretization::Face& F)
  {
    return stencil(F);
  }
  //! Return the coefficients for face F
  ARCANE_DEPRECATED inline CoefficientsType getCoefficients(const Discretization::Face& F)
  {
    return coefficients(F);
  }
  //@}

  /** @name Methods
   */
  //@{
  //! Reset coefficients to zero keeping stencil information
  void resetCoefficients();
  //! Print
  void print(std::ostringstream & ostr);
  //! Print
  template<typename TraceMessageT>
  void print(TraceMessageT trace);
  //@}

private:
  DiscretizationConnectivity* m_dc ;
  Discretization::ItemGroup m_face_group;
  SharedArray<Real> m_coefficients;
  SharedArray<Integer> m_stencils;
  SharedArray<Integer> m_positions;
  SharedArray<Integer> m_lengths;
  ItemInternalList m_item_internal;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*
template<typename T>
DiscretizationCoefficientArrayT<T>::DiscretizationCoefficientArrayT(const DiscretizationConnectivity& dc, Discretization::ItemGroup face_group,
                                        ItemGroupT<T> item_group)
  : m_dc(dc), m_face_group(face_group)
{
  // Initialize item internal representation
  m_item_internal = item_group.itemFamily()->itemsInternal();
}
*/
/*---------------------------------------------------------------------------*/

template<typename T>
void DiscretizationCoefficientArrayT<T>::init(const ItemGroupMapT<typename Discretization::ConvertD2DoF<Discretization::Face>::Type, Integer>& stencil_sizes)
{
  // Compute the maximum number of elements to store
  // and find maximum id
  Integer num_el = 0;
  Integer max_id = 0;
  ENUMERATE_DISCRETIZATION_FACE(iF, m_face_group) {
    const Discretization::Face & F = *iF;
    num_el += stencil_sizes[F];
    max_id  = std::max(max_id, F.localId());
  }

  // Make room for positions vector
  m_positions.resize(max_id + 1);
  // Make room for lengths vector
  m_lengths.resize(max_id+1);
  // Make room for coefficients vector
  m_coefficients.resize(num_el);
  // Make room for items vector
  m_stencils.resize(num_el);

  // Fill stencil sizes
  Integer curr_el = 0;
  ENUMERATE_DISCRETIZATION_FACE(iF, m_face_group) {
    Integer stencil_size_iF = stencil_sizes[*iF];

    m_positions[iF->localId()] = curr_el;
    m_lengths[iF->localId()]   = stencil_size_iF;
    curr_el                   += stencil_size_iF;
  }
  this->resetCoefficients();
}

/*---------------------------------------------------------------------------*/

template<typename T>
void DiscretizationCoefficientArrayT<T>::init(const Arcane::VariableDoFInteger& stencil_sizes)
{
  // Compute the maximum number of elements to store
  // and find maximum id
  Integer num_el = 0;
  Integer max_id = 0;
  ENUMERATE_DISCRETIZATION_FACE(iF, m_face_group) {
    const Discretization::Face & F = *iF;
    num_el += stencil_sizes[F];
    max_id  = std::max(max_id, F.localId());
  }

  // Make room for positions vector
  m_positions.resize(max_id + 1);
  // Make room for lengths vector
  m_lengths.resize(max_id+1);
  // Make room for coefficients vector
  m_coefficients.resize(num_el);
  // Make room for items vector
  m_stencils.resize(num_el);

  // Fill stencil sizes
  Integer curr_el = 0;
  ENUMERATE_DISCRETIZATION_FACE(iF, m_face_group) {
    Integer stencil_size_iF = stencil_sizes[*iF];

    m_positions[iF->localId()] = curr_el;
    m_lengths[iF->localId()]   = stencil_size_iF;
    curr_el                   += stencil_size_iF;
  }
  this->resetCoefficients();
}

/*---------------------------------------------------------------------------*/

template<typename T>
void DiscretizationCoefficientArrayT<T>::init(const Array<std::pair<Discretization::ItemGroup, Integer> >& stencil_sizes)
{
  // Compute the maximum number of elements to store
  // and find maximum id
  Integer num_el = 0;
  Integer max_id = 0;
  for(SharedArray<std::pair<Discretization::ItemGroup, Integer> >::const_iterator i = stencil_sizes.begin(); i != stencil_sizes.end(); i++) {
    num_el += i->first.size() * i->second;
    ENUMERATE_DISCRETIZATION_FACE(iF, i->first) {
      max_id = std::max(max_id, iF->localId());
    }
  }

  // Make room for positions vector
  m_positions.resize(max_id + 1);
  // Make room for lengths vector
  m_lengths.resize(max_id+1);
  // Make room for coefficients vector
  m_coefficients.resize(num_el);
  // Make room for items vector
  m_stencils.resize(num_el);

  // Fill stencil sizes
  Integer curr_el = 0;
  for(SharedArray<std::pair<Discretization::ItemGroup, Integer> >::const_iterator i = stencil_sizes.begin(); i != stencil_sizes.end(); i++) {
    ENUMERATE_DISCRETIZATION_FACE(iF, i->first) {
      m_positions[iF->localId()] = curr_el;
      m_lengths[iF->localId()]   = i->second;
      curr_el                   += i->second;
    }
  }
  this->resetCoefficients();
}

/*---------------------------------------------------------------------------*/

template<typename T>
Integer DiscretizationCoefficientArrayT<T>::stencilSize(const Discretization::Face& F)
{
  return m_lengths[F.localId()];
}

/*---------------------------------------------------------------------------*/

template<typename T>
Integer DiscretizationCoefficientArrayT<T>::numberOfCoefficients() const
{
  return m_coefficients.size();
}

/*---------------------------------------------------------------------------*/

template<typename T>
typename DiscretizationCoefficientArrayT<T>::StencilLocalIdType DiscretizationCoefficientArrayT<T>::stencilLocalId(const Discretization::Face& F)
{
  StencilLocalIdType local_id;
  Integer base   = m_positions[F.localId()];
  Integer length = m_lengths[F.localId()];
  if(base<m_stencils.size())
    return StencilLocalIdType(length, &m_stencils[base]);
  //else cout<<" SOS face: "<<F.localId()<<", isBoundary : "<<((m_dc)->isCellGroupBoundary(F) || (m_dc)->isSubDomainBoundary(F)) <<"\n";
  return local_id;
}

/*---------------------------------------------------------------------------*/

template<typename T>
typename DiscretizationCoefficientArrayT<T>::StencilType DiscretizationCoefficientArrayT<T>::stencil(const Discretization::Face& F)
{
  return StencilType(m_item_internal, stencilLocalId(F));
}

/*---------------------------------------------------------------------------*/

template<typename T>
typename DiscretizationCoefficientArrayT<T>::CoefficientsType DiscretizationCoefficientArrayT<T>::coefficients(const Discretization::Face& F)
{
  CoefficientsType local_id;
  Integer base   = m_positions[F.localId()];
  Integer length = m_lengths[F.localId()];
  if(base<m_stencils.size())
    return CoefficientsType(length, &m_coefficients[base]);
  return local_id;
}

/*---------------------------------------------------------------------------*/

template<typename T>
void DiscretizationCoefficientArrayT<T>::resetCoefficients()
{
  m_coefficients.fill(0.);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void DiscretizationCoefficientArrayT<T>::print(std::ostringstream & ostr)
{
  ENUMERATE_DISCRETIZATION_FACE(iF, m_face_group) {
    const Discretization::Face & F = *iF;
    ostr << FORMATF(59,'-') << endl;
    ostr << "DOF "<< F.localId() << "Face "<< (m_dc->face(F)).localId() << std::endl;
    const bool FIsBoundary = (m_dc)->isCellGroupBoundary(F) || (m_dc)->isSubDomainBoundary(F) ;
    if(FIsBoundary)
      ostr << FORMATW(20) << "Boundary cell: " << m_dc->boundaryCell(F).localId() << std::endl;
    else {
      ostr << FORMATW(20) << "Back cell: " << m_dc->backCell(F).localId() << std::endl;
      ostr << FORMATW(20) << "Front cell: " << m_dc->frontCell(F).localId() << std::endl;
    }
    ostr << FORMATW(20) << "Stencil: " << std::flush;
    StencilLocalIdType stencil = this->stencilLocalId(F);
    for(StencilLocalIdType::const_iterator iI=stencil.begin(); iI!=stencil.end(); iI++)
      ostr << *iI << " " << std::flush;
    ostr << std::endl;

    ostr << FORMATW(20) << "Coefficients: " << std::flush;
    CoefficientsType coefficients = this->coefficients(F);
    for(CoefficientsType::const_iterator taui=coefficients.begin(); taui!=coefficients.end(); taui++)
      ostr << *taui << " " << std::flush;
    ostr<< std::endl;
  }
  ostr << FORMATF(59,'-') << endl;
}



template<typename T>
template<typename TraceMessageT>
void DiscretizationCoefficientArrayT<T>::print(TraceMessageT trace)
{
  std::ostringstream ostr;
  this->print(ostr);
  trace << ostr.str();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#

#endif /* SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONCOEFFICIENTARRAY_H_ */
