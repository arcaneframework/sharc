// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_UTILS_MESHVARIABLEARRAY2REFT_H
#define ARCGEOSIM_UTILS_MESHVARIABLEARRAY2REFT_H

/*
 * Classe permettant d'�muler les objets MeshVariableArray2RefT issus d'Arcane
 * (ie variable dim=2 dans .axl) qui ne sont pas encore dispos.
 * 
 * Classe � supprimer lors de l'ajout de la fonctionnalit� par le CEA/DAM
 */

namespace Arcane {}
using namespace Arcane;

#include <arcane/MeshVariable.h>

template<typename ItemType,typename DataType>
class MeshVariableArray2RefT
{
public:
  
  /** Constructeur de la classe */
  MeshVariableArray2RefT(MeshVariableArrayRefT<ItemType,DataType>& variable) :
    m_view(variable)
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~MeshVariableArray2RefT() {}
  
public:
  
  void resize(Integer size_x, Integer size_y)
  {
    m_size_x = size_x;
    m_size_y = size_y;
    
    m_view.resize(size_x*size_y);
  }
  
  void fill(const DataType& data)
  {
    m_view.fill(data);
  }
  
  const DataType& at(const ItemType& item,
                     Integer i, 
                     Integer j) const
    { return this->m_view[item.localId()][i*m_size_x+j]; }
  
  DataType& at(const ItemType& item,
               Integer i, 
               Integer j)
    { return this->m_view[item.localId()][i*m_size_x+j]; }
  
  const DataType& at(const ItemGroupRangeIteratorT<ItemType>& item,
                     Integer i, 
                     Integer j) const
    { return this->m_view[item.itemLocalId()][i*m_size_x+j]; }
  
  DataType& at(const ItemGroupRangeIteratorT<ItemType>& item,
               Integer i, 
               Integer j)
    { return this->m_view[item.itemLocalId()][i*m_size_x+j]; }
  
  const DataType& at(const ItemPairEnumeratorSubT<ItemType>& item,
                     Integer i, 
                     Integer j) const
    { return this->m_view[item.itemLocalId()][i*m_size_x+j]; }
  
  DataType& at(const ItemPairEnumeratorSubT<ItemType>& item,
               Integer i, 
               Integer j)
    { return this->m_view[item.itemLocalId()][i*m_size_x+j]; }
  
  const DataType& at(const ItemEnumeratorT<ItemType>& item,
                     Integer i, 
                     Integer j) const
    { return this->m_view[item.localId()][i*m_size_x+j]; }
  
  DataType& at(const ItemEnumeratorT<ItemType>& item,
               Integer i, 
               Integer j)
    { return this->m_view[item.localId()][i*m_size_x+j]; }
  
private:
  
  //! sizes
  Integer m_size_x;
  Integer m_size_y;
  
  //! Inner 1D view
  MeshVariableArrayRefT<ItemType,DataType>& m_view;
};

#endif /* ARCGEOSIM_UTILS_MESHVARIABLEARRAY2REFT_H */
