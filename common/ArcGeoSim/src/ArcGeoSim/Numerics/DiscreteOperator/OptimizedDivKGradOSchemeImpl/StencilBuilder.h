// -*- C++ -*-
#ifndef OPTIMIZED_STENCILBUILDER_H
#define OPTIMIZED_STENCILBUILDER_H

#include <algorithm>
#include <vector>

#include "ArcGeoSim/Numerics/DiscreteOperator/CoefficientArray.h"

namespace Optimized {

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
class OrderedStencilBuilderT
{
public:
  
  typedef typename CoefficientArrayT<T>::StencilLocalIdType StencilLocalIdType;
  
public:
  OrderedStencilBuilderT(CoefficientArrayT<T> * a_coefficient_array)
    : m_coefficient_array(a_coefficient_array)
    , m_temporary(Arcane::VariableBuildInfo(a_coefficient_array->faceGroup().mesh(),
                                            IMPLICIT_UNIQ_NAME,
                                            IVariable::PPrivate | IVariable::PTemporary))
    , m_sizes(Arcane::VariableBuildInfo(a_coefficient_array->faceGroup().mesh(),
                                        IMPLICIT_UNIQ_NAME,
                                        IVariable::PPrivate | IVariable::PTemporary))
  {
    m_sizes.fill(0);
    Arcane::Integer max_size = 0;
    ENUMERATE_NODE(inode, a_coefficient_array->faceGroup().mesh()->allNodes()) {
      const Arcane::Integer size = inode->faces().size();
      max_size = (size > max_size) ? size : max_size;
    }
    m_temporary.resize(2*max_size);
  }

  void insert(const Face & a_face, const T & a_item);
  
  void finalize();

private:
  CoefficientArrayT<T> * m_coefficient_array;
  Arcane::VariableFaceArrayInteger m_temporary;
  Arcane::VariableFaceInteger m_sizes;
};

/*----------------------------------------------------------------------------*/

template<typename T>
void OrderedStencilBuilderT<T>::insert(const Face & a_face, const T & a_item)
{
  m_temporary[a_face][m_sizes[a_face]] = a_item.localId();
  m_sizes[a_face]++;
}

/*----------------------------------------------------------------------------*/

template<typename T>
void OrderedStencilBuilderT<T>::finalize()
{
  m_coefficient_array->init(m_sizes);
  
  const FaceGroup & faces = m_coefficient_array->faceGroup();
  ENUMERATE_FACE(iface, faces) {
    const Face & F = *iface;
    StencilLocalIdType stencil_face = m_coefficient_array->stencilLocalId(F);
    IntegerArrayView stencil_face_temp = m_temporary[iface];
    std::sort(stencil_face_temp.begin(), stencil_face_temp.begin() + m_sizes[iface]);
    std::copy(stencil_face_temp.begin(), stencil_face_temp.begin() + m_sizes[iface], stencil_face.begin());
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
}

#endif
