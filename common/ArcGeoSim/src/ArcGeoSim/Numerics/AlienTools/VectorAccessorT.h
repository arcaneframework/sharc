// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_ACCESSOR_VECTORACCESSORT_H
#define ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_ACCESSOR_VECTORACCESSORT_H
/*---------------------------------------------------------------------------*/

#ifdef ARCGEOSIM_LINEARALGEBRA2_INCLUDE_TEMPLATE_IN_CC 
// Si on est en debug ou qu'on ne souhaite pas l'inlining, VectorAccessorT 
// est inclus dans VectorAccessor.cc
#ifndef ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_ACCESSOR_VECTORACCESSOR_CC 
#error "This file must be used by inclusion in VectorAccessor.cc file"
#endif /* ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_ACCESSOR_VECTORACCESSOR_CC */
#endif /* ARCGEOSIM_LINEARALGEBRA2_INCLUDE_TEMPLATE_IN_CC */

// Tout autre inclusion cr�era une erreur de compilation

/*---------------------------------------------------------------------------*/


#ifdef USE_ALIEN_V1
#include <ALIEN/Alien-IFPEN.h>
#endif

#include <arcane/IParallelMng.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename ValueT>
#ifdef USE_ALIEN_V1
VectorAccessorBase<ValueT>::
VectorAccessorBase(IVector & vector)
#endif
  : m_vector_impl(NULL)
  , m_finalized(false)
  , m_changed(false)
{ 
  SimpleCSRVector<ValueT> & v = vector.impl()->get<BackEnd::tag::simplecsr>(false);
  m_values = v.values();
  m_vector_impl = &v;
}

/*---------------------------------------------------------------------------*/

template<typename ValueT>
VectorAccessorBase<ValueT>::
~VectorAccessorBase()
{ 
  end(); 
}

/*---------------------------------------------------------------------------*/

template<typename ValueT>
void
VectorAccessorBase<ValueT>::
end()
{
#ifdef USE_ALIEN_V1
  auto* parallelMng = m_vector_impl->distribution().parallelMng();
#endif
  if (parallelMng) m_changed = parallelMng->reduce(Arcane::Parallel::ReduceMax, m_changed);
  if (m_finalized or !m_changed) return;
  m_finalized = true;
  m_vector_impl->updateTimestamp();
}

/*---------------------------------------------------------------------------*/

template<typename ValueT>
void
VectorAccessorBase<ValueT>::
operator=(const ValueType v)
{
  m_changed = true;
  for(Integer i=0,is=m_values.size();i<is;++i)
    m_values[i] = v;
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename ValueT>
#ifdef USE_ALIEN_V1
ItemVectorAccessorT<ValueT>::
ItemVectorAccessorT(IVector & vector)
#endif
  : VectorAccessorBase<ValueT>(vector)
  , m_space(vector.space())

{ 

}

/*---------------------------------------------------------------------------*/

template<typename ValueT>
ItemVectorAccessorT<ValueT>::
~ItemVectorAccessorT()
{ 
  ;
}

/*---------------------------------------------------------------------------*/

template<typename ValueT>
typename ItemVectorAccessorT<ValueT>::VectorElement
ItemVectorAccessorT<ValueT>::
operator()(const IIndexManager::Entry & entry,typename ItemVectorAccessorT<ValueT>::eSubBlockExtractingPolicyType policy)
{

#ifdef USE_ALIEN_V1
  auto* vblock = this->m_vector_impl->vblock();
  if(vblock) {
    Alien::VBlockImpl vb(*vblock, this->m_vector_impl->distribution());
    return VectorElement(*this,entry, this->m_values,vb.offsetOfLocalIndex(),policy==FirstContiguousIndexes);
  }
#endif
  else
    return VectorElement(*this,entry, this->m_values);
}

/*---------------------------------------------------------------------------*/

template<typename ValueT>
ItemVectorAccessorT<ValueT>::VectorElement::
VectorElement(ItemVectorAccessorT<ValueT> & accessor, const IIndexManager::Entry & entry, ArrayView<ValueT> values)
  : m_entry(entry)
  , m_main_accessor(accessor)
  , m_values(values)
  , m_first_contiguous(true)
{
  ;
}


template<typename ValueT>
ItemVectorAccessorT<ValueT>::VectorElement::
VectorElement(ItemVectorAccessorT<ValueT> & accessor,
              const IIndexManager::Entry & entry,
              ArrayView<ValueT> values,
              ConstArrayView<Integer> values_ptr,
              bool first_contiguous_indexes_policy)
  : m_entry(entry)
  , m_main_accessor(accessor)
  , m_values(values)
  , m_values_ptr(values_ptr)
  , m_first_contiguous(first_contiguous_indexes_policy)
{
  ;
}
/*---------------------------------------------------------------------------*/

template<typename ValueT>
ItemVectorAccessorT<ValueT>::VectorElement::
~VectorElement() 
{
  ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename ValueT>
void 
ItemVectorAccessorT<ValueT>::VectorElement::
operator=(const Real & value) 
{
#ifdef USE_ALIEN_V1
  const auto& dist = m_main_accessor.m_vector_impl->distribution();
  ConstArrayView<Integer> indices = m_entry.getOwnIndexes();
  const Integer offset = dist.offset();
  const Integer nIndex = indices.size();
  auto*  block = m_main_accessor.m_vector_impl->block();
  auto* vblock = m_main_accessor.m_vector_impl->vblock();
  if(block)
  {
    const Integer fix_block_size = block->size();
    for(Integer i=0;i<nIndex;++i)
      for(Integer j=0;j<fix_block_size;++j)
        m_values[fix_block_size*(indices[i]-offset)+j] = value;
  }
  else if(vblock)
  {
    for(Integer i=0;i<nIndex;++i)
    {
      Integer index = indices[i]-offset ;
      Integer ptr = this->m_values_ptr[index] ;
      Integer block_size = this->m_values_ptr[index+1] - ptr ;
      for(Integer j=0;j<block_size;++j)
        m_values[ptr+j] = value;
    }
  }
  else {
    for(Integer i=0;i<nIndex;++i)
      m_values[indices[i]-offset] = value;
  }
  m_main_accessor.m_changed = true;
#endif
}

/*---------------------------------------------------------------------------*/

END_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /*ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_ACCESSOR_VECTORACCESSORT_H*/
