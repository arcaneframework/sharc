/*
 * DiscretizationStencilBuilder.h
 *
 *  Created on: Mar 11, 2016
 *      Author: yousefs
 */

#ifndef SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONSTENCILBUILDER_H_
#define SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONSTENCILBUILDER_H_

#include <algorithm>
#include <vector>

#include "DiscretizationCoefficientArray.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/BinarySearch.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/LinearSearch.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
class DiscretizationStencilSizesBuilderT
{
public:
  DiscretizationStencilSizesBuilderT(DiscretizationCoefficientArrayT<T> * a_coefficient_array)
    : m_coefficient_array(a_coefficient_array)
  {
    m_sizes.init(a_coefficient_array->faceGroup());
    m_sizes.fill(0);
  }

  void insert(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0.);
  void finalize();
private:
  DiscretizationCoefficientArrayT<T> * m_coefficient_array;
  ItemGroupMapT<typename Discretization::ConvertD2DoF<Discretization::Face>::Type, Integer> m_sizes;
};

/*----------------------------------------------------------------------------*/
template<typename T>
void DiscretizationStencilSizesBuilderT<T>::insert(const Discretization::Face & a_face, const T & a_item, const Real & a_value)
{
  m_sizes[a_face]++;
}

/*----------------------------------------------------------------------------*/

template<typename T>
void DiscretizationStencilSizesBuilderT<T>::finalize()
{
  m_coefficient_array->init(m_sizes);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
class OrderedDiscretizationStencilBuilderT
{
  typedef std::vector<Integer> TemporaryDiscretizationStencilType;

public:
  typedef typename DiscretizationCoefficientArrayT<T>::StencilLocalIdType DiscretizationStencilLocalIdType;
  typedef BinarySearchT<DiscretizationStencilLocalIdType> SearchAlgorithm;

public:
  OrderedDiscretizationStencilBuilderT(DiscretizationCoefficientArrayT<T> * a_coefficient_array)
    : m_coefficient_array(a_coefficient_array),
      m_temporary(a_coefficient_array->faceGroup())
  {
    const Discretization::ItemGroup & faces = a_coefficient_array->faceGroup();
    ENUMERATE_DISCRETIZATION_FACE(iface, faces) {
      const Discretization::Face & F = *iface;
      m_temporary[F].reserve(a_coefficient_array->stencilSize(F));
    }
  }

  void insert(const Discretization::Face & a_face, const T & a_item, const Real & a_value = 0.);
  void finalize();
private:
  DiscretizationCoefficientArrayT<T> * m_coefficient_array;
  ItemGroupMapT<typename Discretization::ConvertD2DoF<Discretization::Face>::Type, TemporaryDiscretizationStencilType> m_temporary;
};

/*----------------------------------------------------------------------------*/

template<typename T>
void OrderedDiscretizationStencilBuilderT<T>::insert(const Discretization::Face & a_face, const T & a_item, const Real & a_value)
{
  TemporaryDiscretizationStencilType & stencil_face_temp = m_temporary[a_face];
  TemporaryDiscretizationStencilType::iterator i =
    std::lower_bound(stencil_face_temp.begin(), stencil_face_temp.end(), a_item.localId());
  if(i==stencil_face_temp.end() || a_value<*i)
    stencil_face_temp.insert(i, a_item.localId());
}

/*----------------------------------------------------------------------------*/

template<typename T>
void OrderedDiscretizationStencilBuilderT<T>::finalize()
{
  const Discretization::ItemGroup & faces = m_coefficient_array->faceGroup();
  ENUMERATE_DISCRETIZATION_FACE(iface, faces) {
    const Discretization::Face & F = *iface;
    DiscretizationStencilLocalIdType stencil_face = m_coefficient_array->stencilLocalId(F);
    const TemporaryDiscretizationStencilType & stencil_face_temp = m_temporary[F];
    copy(stencil_face_temp.begin(), stencil_face_temp.end(), stencil_face.begin());
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

class CooresCellDiscretizationStencilBuilder
{
  typedef std::vector<Integer> TemporaryDiscretizationStencilType;

public:
  typedef DiscretizationCoefficientArrayT<typename Discretization::ConvertD2DoF<Discretization::Cell>::Type>::StencilLocalIdType DiscretizationStencilLocalIdType;
  typedef LinearSearchT<DiscretizationStencilLocalIdType> SearchAlgorithm;

public:
  CooresCellDiscretizationStencilBuilder(DiscretizationCoefficientArrayT<typename Discretization::ConvertD2DoF<Discretization::Cell>::Type> * a_coefficient_array,
                                         DiscretizationConnectivity* dc)
    : m_coefficient_array(a_coefficient_array),m_dc(dc),
      m_temporary(a_coefficient_array->faceGroup())
  {
    const Discretization::ItemGroup & faces = a_coefficient_array->faceGroup();
    ENUMERATE_DISCRETIZATION_FACE(iface, faces) {
      const Discretization::Face & F = *iface;
      TemporaryDiscretizationStencilType & stencil_face_temp = m_temporary[F];
      if(m_dc->isSubDomainBoundary(F))
        stencil_face_temp.reserve(a_coefficient_array->stencilSize(F)-1);
      else
        stencil_face_temp.reserve(a_coefficient_array->stencilSize(F)-2);
    }
  }

  void insert(const Discretization::Face & a_face, const Discretization::Cell & a_item, const Real & a_value = 0.);
  void finalize();
private:
  DiscretizationCoefficientArrayT<typename Discretization::ConvertD2DoF<Discretization::Cell>::Type> * m_coefficient_array;
  DiscretizationConnectivity* m_dc ;
  ItemGroupMapT<typename Discretization::ConvertD2DoF<Discretization::Face>::Type, TemporaryDiscretizationStencilType> m_temporary;
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


#endif /* SRC_SCHEMEAPI2016_TOOLS_DISCRETIZATIONSTENCILBUILDER_H_ */
