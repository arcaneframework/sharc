// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#pragma once


#include <arcane/ISubDomain.h>

#include <alien/ref/data/block/BlockVector.h>
#include <alien/ref/handlers/block/BlockVectorWriter.h>
#include <alien/ref/handlers/block/BlockVectorReader.h>

#include "ArcGeoSim/Numerics/Discretization/Schemes/Stencil.h"
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C" Arcane::ISubDomain* _arcaneGetDefaultSubDomain();

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcNum {

  /*---------------------------------------------------------------------------*/
  /*---------------------------------------------------------------------------*/

  class Vector
  {
    struct InternalData
    { 
      InternalData(Alien::BlockVector& alien_block_vector,
		   Arcane::ConstArray2View<Arcane::Integer> all_indexes)
	: m_trace(_arcaneGetDefaultSubDomain()->traceMng())
	, m_i(-1)
	, m_equation(-1)
	, m_alien_block_vector(alien_block_vector)
	, m_all_indexes(all_indexes)
  , m_block_vw(alien_block_vector)
      {}
    Arcane::ITraceMng* m_trace;
    Arcane::Integer m_i;
    Arcane::Integer m_equation;
    Alien::BlockVector& m_alien_block_vector;
    Arcane::ConstArray2View<Arcane::Integer> m_all_indexes;
    Alien::BlockVectorWriter m_block_vw;
  };
  
public:
  
  struct VectorAdder
  {
    VectorAdder(InternalData& data)
      : m_data(data) {}
    
    void operator+=(const Arcane::Real& value) 
    {
      m_data.m_block_vw[m_data.m_i][m_data.m_equation] = m_data.m_block_vw[m_data.m_i][m_data.m_equation] + value;
    }
    
    void operator-=(const Arcane::Real& value) 
    {
      m_data.m_block_vw[m_data.m_i][m_data.m_equation] = m_data.m_block_vw[m_data.m_i][m_data.m_equation] - value;
    }
    
    void operator=(const Arcane::Real& value) 
    {
      m_data.m_block_vw[m_data.m_i][m_data.m_equation] = value;
    }

    void operator+=(const Contribution& array)
    {
      m_data.m_block_vw[m_data.m_i][m_data.m_equation] = m_data.m_block_vw[m_data.m_i][m_data.m_equation] + array.value();
    }

    void operator-=(const Contribution& array)
    {
      m_data.m_block_vw[m_data.m_i][m_data.m_equation] = m_data.m_block_vw[m_data.m_i][m_data.m_equation] - array.value();
    }

    void operator =(const Contribution& array)
    {
      m_data.m_block_vw[m_data.m_i][m_data.m_equation] = array.value();
    }

    operator const Arcane::Real&() const 
    {
      Alien::BlockVectorReader vr(m_data.m_alien_block_vector);
      return vr[m_data.m_i][m_data.m_equation];
    }

    InternalData& m_data;
  };
  
  class VectorElementProxy 
  {
  public:
    
    VectorElementProxy(InternalData& data)
      : m_data(data) {}
    
    VectorAdder operator[](const Arcane::Item& i) 
    {
      m_data.m_i = m_data.m_all_indexes[i.localId()][0]/m_data.m_alien_block_vector.block().size();
      return VectorAdder(m_data);
    }
    
    VectorAdder operator[](const Arcane::ItemEnumerator& i) 
    {
      m_data.m_i = m_data.m_all_indexes[i->localId()][0]/m_data.m_alien_block_vector.block().size();
      return VectorAdder(m_data);
    }
   
    VectorAdder operator[](const Stencil::Cell& i)
    {
      m_data.m_i = m_data.m_all_indexes[i.localId()][0]/m_data.m_alien_block_vector.block().size();
      return VectorAdder(m_data);
    }
   
  private:
    
    InternalData& m_data;
  };
  Vector(Alien::BlockVector& alien_block_vector,
	 Arcane::ConstArray2View<Arcane::Integer> indexes)
    : m_data(alien_block_vector, indexes) {}

  VectorElementProxy operator[](Arcane::Integer equation)
  {
    m_data.m_equation = equation;
    return VectorElementProxy(m_data);
  }

  Vector& operator=(Arcane::Real v)
  {
    {
      Alien::BlockVectorWriter vw(m_data.m_alien_block_vector);
      vw = v;
    }
    return *this;
  }

  const Alien::BlockVector& block() const
  {
    return m_data.m_alien_block_vector;
  }

private:
  
  InternalData m_data;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

