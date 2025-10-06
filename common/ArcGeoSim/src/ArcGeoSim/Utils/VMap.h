// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_UTILS_VMAP_H
#define ARCGEOSIM_ARCGEOSIM_UTILS_VMAP_H

#include "ArcGeoSim/Utils/ArrayUtils.h"

#include <arcane/utils/Array.h>
#include <arcane/utils/ArcaneGlobal.h> // For ARCANE_OS_WIN32 below

#define VMAP_USE_REALLOC

/*------------------------------------------------------------------------------------------*/
#if (ARCANE_VERSION >= 22200 )
namespace Arccore {
template<>
class ArrayTraits<std::pair<Integer,Integer> >
{
 public:
  typedef const std::pair<Integer,Integer>& ConstReferenceType;
  typedef TrueType IsPODType;
};
}
#else
ARCANE_BEGIN_NAMESPACE
template<>
class ArrayTraits<std::pair<Integer,Integer> >
{
 public:
  typedef const std::pair<Integer,Integer>& ConstReferenceType;
  typedef TrueType IsPODType;
};

ARCANE_END_NAMESPACE
#endif

BEGIN_ARCGEOSIM_NAMESPACE

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
DataT * intrusive_vmap_insert(IndexT new_index,
                              Integer & hint_pos,
                              Integer & size,
                              Integer capacity,
                              IndexT * indexes,
                              DataT * data)
{
  hint_pos = ArcGeoSim::ArrayScan::dichotomicPositionScan(new_index, ConstArrayView<IndexT>(size, indexes));
  if (hint_pos < size and indexes[hint_pos] == new_index) 
    {
      return &data[hint_pos];
    }
  else if (size < capacity)
    {
      for(Integer i=size;i>hint_pos;--i)
        {
          data[i] = data[i-1];
          indexes[i] = indexes[i-1];
        }
      indexes[hint_pos] = new_index;
      data[hint_pos] = DataT();
      ++size;
      return &data[hint_pos];
    }
  else
    {
      return NULL;
    }
}

/*------------------------------------------------------------------------------------------*/

#ifdef ARCANE_OS_WIN32
#ifndef NO_USER_WARNING
#ifndef WIN32
#warning "TODO: SafeCopy check not yet implemented under Win32"
#endif
#endif /* NO_USER_WARNING */
#if (ARCANE_VERSION >= 22200)
template<typename T1, typename T2, 
         typename IsPOD1 = typename Arccore::ArrayTraits<T1>::IsPODType, 
         typename IsPOD2 = typename Arccore::ArrayTraits<T2>::IsPODType> struct SafeCopy { struct Check { }; };
#else /*ARCANE_VERSION */
template<typename T1, typename T2, 
         typename IsPOD1 = typename Arcane::ArrayTraits<T1>::IsPODType, 
         typename IsPOD2 = typename Arcane::ArrayTraits<T2>::IsPODType> struct SafeCopy { struct Check { }; };
#endif
#else /* ARCANE_OS_WIN32 */
#if (ARCANE_VERSION >= 22200)
template<typename T1, typename T2, 
         typename IsPOD1 = typename Arccore::ArrayTraits<T1>::IsPODType, 
         typename IsPOD2 = typename Arccore::ArrayTraits<T2>::IsPODType> struct SafeCopy { };
template<typename T1, typename T2> struct SafeCopy<T1, T2, Arccore::TrueType, Arccore::TrueType> { struct Check { }; };
template<typename T1> struct SafeCopy<T1,std::pair<Integer,Integer>,Arccore::TrueType, Arccore::TrueType > { struct Check { }; };
#else
template<typename T1, typename T2, 
         typename IsPOD1 = typename Arcane::ArrayTraits<T1>::IsPODType, 
         typename IsPOD2 = typename Arcane::ArrayTraits<T2>::IsPODType> struct SafeCopy { };
template<typename T1, typename T2> struct SafeCopy<T1, T2, Arcane::TrueType, Arcane::TrueType> { struct Check { }; };


template<typename T1> struct SafeCopy<T1,std::pair<Integer,Integer>,Arcane::TrueType, Arcane::TrueType > { struct Check { }; };
#endif

#endif /* ARCANE_OS_WIN32 */

/*------------------------------------------------------------------------------------------*/

/*! Tableau associatif pour des clefs et donn�es POD
 *
 *
 */
template<typename IndexT, typename DataT>
class VMap 
  : SafeCopy<IndexT, DataT>::Check
{
public:
  class const_iterator;
  class iterator {
  public:
    iterator(VMap & vmap, Integer index) : m_vmap(vmap), m_index(index) { }
    DataT & value() { return m_vmap.m_data[m_index]; }
    const IndexT & key() const { return m_vmap.m_indexes[m_index]; }
    void operator++() { ++m_index; }
    bool operator==(const iterator & i) const { return (m_index == i.m_index); }
    bool operator!=(const iterator & i) const { return (m_index != i.m_index); }
  private:
    VMap & m_vmap;
    Integer m_index;
  private:
    friend class const_iterator;
  };

  class const_iterator {
  public:
    const_iterator(const VMap & vmap, Integer index) : m_vmap(vmap), m_index(index) { }
    const_iterator(const iterator & itr) : m_vmap(itr.m_vmap), m_index(itr.m_index) { }
    const DataT & value() const { return m_vmap.m_data[m_index]; }
    const IndexT & key() const { return m_vmap.m_indexes[m_index]; }
    void operator++() { ++m_index; }
    bool operator==(const const_iterator & i) const { return (m_index == i.m_index); }
    bool operator!=(const const_iterator & i) const { return (m_index != i.m_index); }
  private:
    const VMap & m_vmap;
    Integer m_index;
  };

public:
  VMap(const Integer first_capacity = 4);
  virtual ~VMap();
  VMap(const VMap & vmap);
  VMap & operator=(const VMap & vmap);

public:
  DataT & operator[](const IndexT index);
  iterator find(const IndexT index);
  const_iterator find(const IndexT index) const;
  std::pair<iterator,bool> insert(const IndexT index);
  const_iterator begin() const { return const_iterator(*this, 0); }
  const_iterator end() const { return const_iterator(*this, m_size); }
  iterator begin() { return iterator(*this, 0); }
  iterator end() { return iterator(*this, m_size); }
  Integer size() const { return m_size; }

private:
  Integer m_size, m_capacity;
  IndexT * m_indexes;
  DataT * m_data;
  void * m_memory_pool;

private:
  static Integer new_capacity(const Integer capacity);
};

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
VMap<IndexT,DataT>::
VMap(const Integer first_capacity)
  : m_size(0)
  , m_capacity(first_capacity)
{
  m_memory_pool = malloc(m_capacity*(sizeof(IndexT)+sizeof(DataT)));
  m_data = (DataT*)m_memory_pool;
  m_indexes = (IndexT*)(m_data+m_capacity);
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
VMap<IndexT,DataT>::
VMap(const VMap & vmap)
  : m_size(vmap.m_size)
  , m_capacity(vmap.m_capacity)
{
  m_memory_pool = malloc(m_capacity*(sizeof(IndexT)+sizeof(DataT)));
  m_data = (DataT*)m_memory_pool;
  m_indexes = (IndexT*)(m_data+m_capacity);
  memcpy(m_memory_pool, vmap.m_memory_pool, m_capacity*(sizeof(IndexT)+sizeof(DataT)));
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
VMap<IndexT,DataT> &
VMap<IndexT,DataT>::
operator=(const VMap & vmap)
{
  m_size = vmap.m_size;
  m_capacity = vmap.m_capacity;
  free(m_memory_pool);
  m_memory_pool = malloc(m_capacity*(sizeof(IndexT)+sizeof(DataT)));
  memcpy(m_memory_pool, vmap.m_memory_pool, m_capacity*(sizeof(IndexT)+sizeof(DataT)));
  m_data = (DataT*)m_memory_pool;
  m_indexes = (IndexT*)(m_data+m_capacity);
  return *this;
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
VMap<IndexT,DataT>::
~VMap()
{
  free(m_memory_pool);
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
DataT &
VMap<IndexT,DataT>::
operator[](const IndexT index)
{
  return insert(index).first.value();
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
typename VMap<IndexT,DataT>::iterator
VMap<IndexT,DataT>::
find(const IndexT index)
{
  const Integer col = ArcGeoSim::ArrayScan::dichotomicScan(index,ConstArrayView<IndexT>(m_size, m_indexes));
  if (col == -1)
    return end();
  else
    return iterator(*this, col);
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
typename VMap<IndexT,DataT>::const_iterator
VMap<IndexT,DataT>::
find(const IndexT index) const
{
  const Integer col = ArcGeoSim::ArrayScan::dichotomicScan(index,ConstArrayView<IndexT>(m_size, m_indexes));
  if (col == -1)
    return end();
  else
    return const_iterator(*this, col);
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
std::pair<typename VMap<IndexT,DataT>::iterator, bool>
VMap<IndexT,DataT>::
insert(const IndexT index)
{
  Integer old_size = m_size;

  Integer hint_pos = 0;
  DataT * data = intrusive_vmap_insert(index, hint_pos,
                                       m_size, m_capacity, 
                                       m_indexes, m_data);
  if (data == NULL)
    { // Too small: needs resize
      Integer old_capacity = m_capacity;
      m_capacity = new_capacity(m_capacity);
      // std::cout << "Extending from " << old_capacity << " to " << m_capacity << "\n";

#ifdef VMAP_USE_REALLOC
      m_memory_pool = realloc(m_memory_pool, m_capacity*(sizeof(IndexT)+sizeof(DataT)));
      m_data = (DataT*)m_memory_pool;
      m_indexes = (IndexT*)(m_data+m_capacity);

      DataT * old_data = (DataT*)m_memory_pool;
      IndexT * old_indexes = (IndexT*)(old_data+old_capacity);

      for(Integer i=old_capacity; i>hint_pos;--i)
        m_indexes[i] = old_indexes[i-1];
      m_indexes[hint_pos] = index;
      for(Integer i=hint_pos; --i>=0;)
        m_indexes[i] = old_indexes[i];
      for(Integer i=old_capacity; i>hint_pos;--i)
        m_data[i] = old_data[i-1];

      // Pas de copie des premi�res valeurs de data qui sont d�j� bien plac�es
#else /* VMAP_USE_REALLOC */
      void * old_memory_pool = m_memory_pool;
      DataT * old_data = (DataT*)old_memory_pool;
      IndexT * old_indexes = (IndexT*)(old_data+old_capacity);

      m_memory_pool = malloc(m_capacity*(sizeof(IndexT)+sizeof(DataT)));
      // memset(m_memory_pool, -1, m_capacity*(sizeof(IndexT)+sizeof(DataT))); // raw debug

      m_data = (DataT*)m_memory_pool;
      m_indexes = (IndexT*)(m_data+m_capacity);

      for(Integer i=old_capacity; i>hint_pos;--i)
        m_indexes[i] = old_indexes[i-1];
      m_indexes[hint_pos] = index;
      for(Integer i=hint_pos; --i>=0;)
        m_indexes[i] = old_indexes[i];
      for(Integer i=old_capacity; i>hint_pos;--i)
        m_data[i] = old_data[i-1];
      for(Integer i=0;i<hint_pos;++i)
        m_data[i] = old_data[i];

      free(old_memory_pool);
#endif /* VMAP_USE_REALLOC */

      ++m_size;
      m_data[hint_pos] = DataT();
    }

  return std::pair<iterator,bool>(iterator(*this, hint_pos), m_size!=old_size);
}

/*------------------------------------------------------------------------------------------*/

template<typename IndexT, typename DataT>
Integer
VMap<IndexT,DataT>::
new_capacity(const Integer capacity)
{
  if (capacity < 20)
    return capacity + 4;
  else if (capacity < 4096)
    return 2*capacity + 12;
  else
    return capacity + 4096;
}

/*------------------------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*------------------------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_UTILS_VMAP_H */
