// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#pragma once

#include "ArcGeoSim/Numerics/Discretization/Schemes/TwoPointsStencil.h"

#include "ArcGeoSim/Numerics/Discretization/Schemes/MultiPointsStencil.h"

#include <alien/ref/data/block/BlockMatrix.h>
#include <alien/ref/data/block/VBlockMatrix.h>
#include <alien/ref/handlers/profiler/MatrixProfiler.h>
#include <alien/ref/handlers/block/ProfiledBlockMatrixBuilder.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcNum {

  /*---------------------------------------------------------------------------*/
  /*---------------------------------------------------------------------------*/

  class Bound;

  /*---------------------------------------------------------------------------*/
  /*---------------------------------------------------------------------------*/
  class Matrix
  {
    struct InternalData
    { 
      InternalData(Alien::BlockMatrix& alien_block_matrix,
                   Arcane::ConstArray2View<Arcane::Integer> all_indexes)
    : m_alien_block_matrix(alien_block_matrix)
    , m_all_indexes(all_indexes)
    , m_alien_builder(m_alien_block_matrix, Alien::ProfiledBlockMatrixBuilderOptions::eResetValues)
    {
      m_blk_size = alien_block_matrix.block().size() ;
      Alien::allocateData(m_values_2d, m_blk_size, m_blk_size );
      m_cache_derivatives.resize(m_max_stencil_size*m_blk_size) ;
    }

    void updateMaxStencilSize(Integer size)
    {
      if(size>m_max_stencil_size)
      {
          m_max_stencil_size = size ;
          m_cache_derivatives.resize(m_blk_size*m_max_stencil_size) ;
      }
    }
    Arcane::Integer m_i = -1;
    Arcane::Integer m_j = -1;
    Arcane::Integer m_equation = -1;
    Arcane::Integer m_well_index = -1;
    Arcane::Integer m_blk_size = 0;
    Arcane::Integer m_max_stencil_size = 2 ;

    Alien::BlockMatrix& m_alien_block_matrix;
    Alien::UniqueArray2<Real> m_values_2d;
    Alien::ProfiledBlockMatrixBuilder m_alien_builder;
    Arcane::ConstArray2View<Arcane::Integer> m_all_indexes;
    Arcane::UniqueArray<Real> m_cache_derivatives;
  };
  
public:

  struct MatrixAdder
  {
    MatrixAdder(InternalData& data)
    : m_data(data)
    {}

    void operator+=(const Contribution& values)
    {
#ifdef LOLD
      Alien::fill(m_data.m_values_2d, 0.);
      const Arcane::RealConstArrayView contrib(values.size(),values.derivatives());
      const Arcane::Integer size = contrib.size();
      //auto& contrib = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(size);
      //::audi::unpack(contrib, values);
      //const Arcane::Integer size = m_data.m_blk_size;
      for(Arcane::Integer k = 0; k < size; ++k) {
        m_data.m_values_2d[m_data.m_equation][k] += contrib[k];
      }
      m_data.m_alien_builder(m_data.m_i, m_data.m_j) += Alien::view(m_data.m_values_2d);
#else
      Alien::fill(m_data.m_values_2d, 0.);
      Arcane::RealArrayView row = m_data.m_values_2d[m_data.m_equation];
      const Arcane::Integer size = row.size();
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(size);
      ::audi::unpack(_cache_derivatives, values);
      for(Arcane::Integer k = 0; k < size; ++k) {
        row[k] += _cache_derivatives[k];
      }
      m_data.m_alien_builder(m_data.m_i, m_data.m_j) += Alien::view(m_data.m_values_2d);
      /*
      std::cout<<"MATRIX ADD+("<<m_data.m_i<<","<<m_data.m_j<<"):";
      for(int ii=0;ii<3;++ii)
        for(int jj=0;jj<3;++jj)
          std::cout<<m_data.m_values_2d[ii][jj]<<",";
      std::cout<<std::endl ;
      */
#endif
    }

    // TODO (XT)
    void operator-=(const Contribution& values)
    {
#ifdef LOLD
      const Arcane::RealConstArrayView contrib(values.size(),values.derivatives());
      const Arcane::Integer size = contrib.size();
      //ARCANE_ASSERT((size == row.size()),("Sizes are not equal"));
      //for(Arcane::Integer k = 0; k < size; ++k) {
      //m_data.m_values_2d[m_data.m_equation][k] -= contrib[k];
      //}
#else
      Alien::fill(m_data.m_values_2d, 0.);
      Arcane::RealArrayView row = m_data.m_values_2d[m_data.m_equation];
      const Arcane::Integer size = row.size();
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(size);
      ::audi::unpack(_cache_derivatives, values);
      for(Arcane::Integer k = 0; k < size; ++k) {
        row[k] -= _cache_derivatives[k];
      }
      m_data.m_alien_builder(m_data.m_i, m_data.m_j) += Alien::view(m_data.m_values_2d);
      /*
      std::cout<<"MATRIX ADD+("<<m_data.m_i<<","<<m_data.m_j<<"):";
      for(int ii=0;ii<3;++ii)
        for(int jj=0;jj<3;++jj)
          std::cout<<m_data.m_values_2d[ii][jj]<<",";
      std::cout<<std::endl ;
      */
#endif
    }
    // TODO (XT)
    void operator=(const Contribution& values)
    {
#ifdef LOLD
      const Arcane::RealConstArrayView contrib(values.size(),values.derivatives());
      const Arcane::Integer size = contrib.size();
      //ARCANE_ASSERT((size == row.size()),("Sizes are not equal"));
      //for(Arcane::Integer k = 0; k < size; ++k) {
      //m_data.m_values_2d[m_data.m_equation][k] = contrib[k];
      //
#else
      //Arcane::RealArray2View block = m_data.m_alien_builder(m_data.m_i,m_data.m_j);
      //Arcane::RealArrayView row = block[m_data.m_equation];
      Alien::fill(m_data.m_values_2d, 0.);
      Arcane::RealArrayView row = m_data.m_values_2d[m_data.m_equation];
      const Arcane::Integer size = row.size();
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(size);
      ::audi::unpack(_cache_derivatives, values);
      for(Arcane::Integer k = 0; k < size; ++k) {
        row[k] = _cache_derivatives[k];
      }
      m_data.m_alien_builder(m_data.m_i, m_data.m_j) = Alien::view(m_data.m_values_2d);
#endif
    }
    InternalData& m_data;
  };
  
  struct MatrixAdderFV
  {
    MatrixAdderFV(InternalData& data,
                  const ArcNum::TwoPointsStencil& stencil)
    : m_data(data)
    , m_stencil(stencil) {}
    
    void operator+=(const Contribution& values)
    {
#ifdef LOLD
      Alien::fill(m_data.m_values_2d, 0.);
      const Arcane::RealConstArrayView v(values.size(),values.derivatives());
      //auto& v = m_data.m_cache_derivatives;
      //::audi::unpack(v, values);
      {
        const Arcane::Integer size = m_data.m_values_2d.dim1Size();
        const Arcane::Integer j = m_data.m_all_indexes[m_stencil.back().cell().localId()][0]/size;
        const Arcane::RealConstArrayView contrib = v.subConstView(0, size);
        for(Arcane::Integer k = 0; k < size; ++k) {
           m_data.m_values_2d[m_data.m_equation][k] += contrib[k];
        }
        m_data.m_alien_builder(m_data.m_i, j) += Alien::view(m_data.m_values_2d);
      }
      {
	Alien::fill(m_data.m_values_2d, 0.);
        const Arcane::Integer size = m_data.m_values_2d.dim1Size();
        const Arcane::Integer j = m_data.m_all_indexes[m_stencil.front().cell().localId()][0]/size;
        const Arcane::RealConstArrayView contrib = v.subConstView(size, size);
        for(Arcane::Integer k = 0; k < size; ++k) {
           m_data.m_values_2d[m_data.m_equation][k] += contrib[k];
        }
        m_data.m_alien_builder(m_data.m_i, j) += Alien::view(m_data.m_values_2d);
      }
#else
      //GEOXIM
      //const Arcane::Integer size = size_b + size_f;
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(size);
      ::audi::unpack(_cache_derivatives, values);

      // back
      Alien::fill(m_data.m_values_2d, 0.);
      const Arcane::Integer size = m_data.m_values_2d.dim1Size();
      const Arcane::Integer b = m_data.m_all_indexes[m_stencil.back().cell().localId()][0]/size;
      //Arcane::RealArray2View block_b = m_data.m_block_matrix(m_data.m_i,b);
      Arcane::RealArrayView row_b = m_data.m_values_2d[m_data.m_equation];
      const Arcane::Integer size_b = row_b.size();
      //
      {
        const Arcane::RealConstArrayView contrib = _cache_derivatives.subConstView(0, size_b);
        for(Arcane::Integer k = 0; k < size_b; ++k) {
          row_b[k] += contrib[k];
        }
      }
      m_data.m_alien_builder(m_data.m_i, b) += Alien::view(m_data.m_values_2d);
      /*
      std::cout<<"MATRIX FVADD+("<<m_data.m_i<<","<<b<<"):";
      for(int ii=0;ii<3;++ii)
        for(int jj=0;jj<3;++jj)
          std::cout<<m_data.m_values_2d[ii][jj]<<",";
      std::cout<<std::endl ;
      */

      // front
      Alien::fill(m_data.m_values_2d, 0.);
      const Arcane::Integer f = m_data.m_all_indexes[m_stencil.front().cell().localId()][0]/size;
      //Arcane::RealArray2View block_f = m_data.m_block_matrix(m_data.m_i,f);
      Arcane::RealArrayView row_f = m_data.m_values_2d[m_data.m_equation];
      const Arcane::Integer size_f = row_f.size();
      {
        const Arcane::RealConstArrayView contrib = _cache_derivatives.subConstView(size_b, size_f);
        for(Arcane::Integer k = 0; k < size_f; ++k) {
          row_f[k] += contrib[k];
        }
      }
      m_data.m_alien_builder(m_data.m_i, f) += Alien::view(m_data.m_values_2d);
      /*
      std::cout<<"MATRIX FVADD+("<<m_data.m_i<<","<<f<<"):";
      for(int ii=0;ii<3;++ii)
        for(int jj=0;jj<3;++jj)
          std::cout<<m_data.m_values_2d[ii][jj]<<",";
      std::cout<<std::endl ;
      */
#endif
    }
    
    void operator-=(const Contribution& values)
    {
#ifdef LOLD
      const Arcane::RealConstArrayView v(values.size(),values.derivatives());
      //auto& v = m_data.m_cache_derivatives;
      //::audi::unpack(v, values);
      {
        Alien::fill(m_data.m_values_2d, 0.);
        const Arcane::Integer size = m_data.m_values_2d.dim1Size();
        const Arcane::Integer j = m_data.m_all_indexes[m_stencil.back().cell().localId()][0]/size;
        const Arcane::RealConstArrayView contrib = v.subConstView(0, size);
        for(Arcane::Integer k = 0; k < size; ++k) {
           m_data.m_values_2d[m_data.m_equation][k] -= contrib[k];
        }
        m_data.m_alien_builder(m_data.m_i, j) += Alien::view(m_data.m_values_2d);
      }
      {
        Alien::fill(m_data.m_values_2d, 0.);
        const Arcane::Integer size = m_data.m_values_2d.dim1Size();
        const Arcane::Integer j = m_data.m_all_indexes[m_stencil.front().cell().localId()][0]/size;
        const Arcane::RealConstArrayView contrib = v.subConstView(size, size);
        for(Arcane::Integer k = 0; k < size; ++k) {
          m_data.m_values_2d[m_data.m_equation][k] -= contrib[k];
        }
        m_data.m_alien_builder(m_data.m_i, j) += Alien::view(m_data.m_values_2d);
      }
#else
      //GEOXIM
      //const Arcane::Integer size = size_b + size_f;
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(size);
      ::audi::unpack(_cache_derivatives, values);

      // back
      Alien::fill(m_data.m_values_2d, 0.);
      const Arcane::Integer size = m_data.m_values_2d.dim1Size();
      const Arcane::Integer b = m_data.m_all_indexes[m_stencil.back().cell().localId()][0]/size;
      //Arcane::RealArray2View block_b = m_data.m_block_matrix(m_data.m_i,b);
      Arcane::RealArrayView row_b = m_data.m_values_2d[m_data.m_equation];
      const Arcane::Integer size_b = row_b.size();
      //
      {
        const Arcane::RealConstArrayView contrib = _cache_derivatives.subConstView(0, size_b);
        for(Arcane::Integer k = 0; k < size_b; ++k) {
          row_b[k] -= contrib[k];
        }
      }
      m_data.m_alien_builder(m_data.m_i, b) += Alien::view(m_data.m_values_2d);
      /*
      std::cout<<"MATRIX ADD-("<<m_data.m_i<<","<<b<<"):";
      for(int ii=0;ii<3;++ii)
        for(int jj=0;jj<3;++jj)
          std::cout<<m_data.m_values_2d[ii][jj]<<",";
      std::cout<<std::endl ;
      */

      // front
      Alien::fill(m_data.m_values_2d, 0.);
      const Arcane::Integer f = m_data.m_all_indexes[m_stencil.front().cell().localId()][0]/size;
      //Arcane::RealArray2View block_f = m_data.m_block_matrix(m_data.m_i,f);
      Arcane::RealArrayView row_f = m_data.m_values_2d[m_data.m_equation];
      const Arcane::Integer size_f = row_f.size();
      {
        const Arcane::RealConstArrayView contrib = _cache_derivatives.subConstView(size_b, size_f);
        for(Arcane::Integer k = 0; k < size_f; ++k) {
          row_f[k] -= contrib[k];
        }
      }
      m_data.m_alien_builder(m_data.m_i, f) += Alien::view(m_data.m_values_2d);
      /*
      std::cout<<"MATRIX ADD-("<<m_data.m_i<<","<<f<<"):";
      for(int ii=0;ii<3;++ii)
        for(int jj=0;jj<3;++jj)
          std::cout<<m_data.m_values_2d[ii][jj]<<",";
      std::cout<<std::endl ;
      */
#endif
    }

    // TODO : To complete (and test) (XT)
    void operator=(const Contribution& values)
    {
#ifdef LOLD
      const Arcane::RealConstArrayView v(values.size(),values.derivatives());// = values.derivatives();
      {
        const Arcane::Integer size = m_data.m_values_2d.dim1Size();
        const Arcane::Integer j = m_data.m_all_indexes[m_stencil.back().cell().localId()][0]/size;
        const Arcane::RealConstArrayView contrib = v.subConstView(0, size);
        for(Arcane::Integer k = 0; k < size; ++k) {
          //row[k] = contrib[k];
        }
      }
      {
        const Arcane::Integer size = m_data.m_values_2d.dim1Size();
        const Arcane::Integer j = m_data.m_all_indexes[m_stencil.front().cell().localId()][0]/size;
        const Arcane::RealConstArrayView contrib = v.subConstView(size, size);
        for(Arcane::Integer k = 0; k < size; ++k) {
          //row[k] = contrib[k];
        }
      }
#endif

    }
    InternalData& m_data;
    const ArcNum::TwoPointsStencil& m_stencil;
  };
#ifndef LOLD
  struct MatrixAdderFVMultipoints
  {
    MatrixAdderFVMultipoints(InternalData& data,
                             std::shared_ptr<ArcNum::MultiPointsStencil> stencil)
    : m_data(data)
    , m_stencil(stencil)
    {
      m_data.updateMaxStencilSize(m_stencil->size()) ;
    }

    inline Arcane::RealArrayView get_row(const Stencil::Cell& law_cell)
    {
      const Arcane::Integer index = m_data.m_all_indexes[law_cell.cell().localId()][0];
      //Arcane::RealArray2View block = m_data.m_alien_builder(m_data.m_i,index);
      return m_data.m_values_2d[m_data.m_equation] ;
      //return block[m_data.m_equation];
    }

    // a revoir ok pour blocks fixes
    inline Arcane::Integer get_all_contribution_size()
    {
      m_stencil -> begin();
      Arcane::RealArrayView row = get_row(m_stencil->cell());
      const Arcane::Integer all_contribution_size = row.size()*m_stencil->size();
      return all_contribution_size;
    }

    void operator+=(const Contribution& values)
    {
      // unpack sparse derivatives in one array
      //const Arcane::Integer all_contrib_size = get_all_contribution_size();
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(all_contrib_size);
      ::audi::unpack(_cache_derivatives, values);

      for(m_stencil -> begin(); not m_stencil -> end() ; m_stencil -> next()) {
        const auto& lawCell(m_stencil -> cell()) ;
        Arcane::RealArrayView current_row = get_row(lawCell);
        const Arcane::Integer current_size = current_row.size();
        const Arcane::Integer index = lawCell.index() ;
        // a revoir ok pour blocks fixes
        const Arcane::RealConstArrayView contrib = _cache_derivatives.subConstView(index * current_size, current_size);
        for(Arcane::Integer k = 0; k < current_size; ++k) {
          current_row[k] += contrib[k];
        }
      }
    }

    void operator-=(const Contribution& values)
    {
      // unpack sparse derivatives in one array
      //const Arcane::Integer all_contrib_size = get_all_contribution_size();
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(all_contrib_size);
      ::audi::unpack(_cache_derivatives, values);

      for(m_stencil -> begin() ; not m_stencil -> end() ; m_stencil -> next()) {
        const auto& lawCell(m_stencil -> cell()) ;
        Arcane::RealArrayView current_row = get_row(lawCell);
        const Arcane::Integer current_size = current_row.size();
        const Arcane::Integer index = lawCell.index() ;
        // a revoir ok pour blocks fixes
        const Arcane::RealConstArrayView contrib = _cache_derivatives.subConstView(index * current_size, current_size);
        for(Arcane::Integer k = 0; k < current_size; ++k) {
          current_row[k] -= contrib[k];
        }
      }
    }

    void operator=(const Contribution& values)
    {
      // unpack sparse derivatives in one array
      //const Arcane::Integer all_contrib_size = get_all_contribution_size();
      auto& _cache_derivatives = m_data.m_cache_derivatives;
      //_cache_derivatives.resize(all_contrib_size);
      ::audi::unpack(_cache_derivatives, values);

      for(m_stencil -> begin() ; not m_stencil -> end() ; m_stencil -> next()) {
        const auto& lawCell(m_stencil -> cell()) ;
        Arcane::RealArrayView current_row = get_row(lawCell);
        const Arcane::Integer current_size = current_row.size();
        const Arcane::Integer index = lawCell.index() ;
        // a revoir ok pour blocks fixes
        const Arcane::RealConstArrayView contrib = _cache_derivatives.subConstView(index * current_size, current_size);
        for(Arcane::Integer k = 0; k < current_size; ++k) {
          current_row[k] = contrib[k];
        }
      }
    }

    InternalData& m_data;
    std::shared_ptr<ArcNum::MultiPointsStencil> m_stencil;
  };
#endif


  struct MatrixProxyCol
  {
    MatrixProxyCol(InternalData& data)
      : m_data(data) {}    
    MatrixAdder operator[](const Arcane::Item& j) 
    {
      m_data.m_j = m_data.m_all_indexes[j.localId()][0]/m_data.m_alien_block_matrix.block().size();
      return MatrixAdder(m_data);
    }
    MatrixAdder operator[](const Arcane::ItemEnumerator& j)
    {
      m_data.m_j = m_data.m_all_indexes[j->localId()][0]/m_data.m_alien_block_matrix.block().size();
      return MatrixAdder(m_data);
    }
    InternalData& m_data;
  };
  
  struct MatrixProxyColFV
  {
    MatrixProxyColFV(InternalData& data)
      : m_data(data) {}

    MatrixAdderFV operator[](const ArcNum::TwoPointsStencil& j)
    {
      return MatrixAdderFV(m_data,j);
    }
#ifndef LOLD
    MatrixAdderFVMultipoints operator[](std::shared_ptr<ArcNum::MultiPointsStencil> stencil)  {
      return MatrixAdderFVMultipoints(m_data, stencil) ;
    }
#endif
    InternalData& m_data;
  };

  struct MatrixProxyRow
  {
    MatrixProxyRow(InternalData& data)
      : m_data(data) {}

    MatrixProxyCol operator[](const Arcane::Item& i) 
    {
      m_data.m_i = m_data.m_all_indexes[i.localId()][0]/m_data.m_alien_block_matrix.block().size();
      return MatrixProxyCol(m_data);
    }

    MatrixProxyCol operator[](const Arcane::ItemEnumerator& i)
    {
      m_data.m_i = m_data.m_all_indexes[i->localId()][0]/m_data.m_alien_block_matrix.block().size();
      return MatrixProxyCol(m_data);
    }

    MatrixProxyColFV operator[](const Stencil::Cell& i)
    {
      m_data.m_i = m_data.m_all_indexes[i.localId()][0]/m_data.m_alien_block_matrix.block().size();
      return MatrixProxyColFV(m_data);
    }

    InternalData& m_data;
  };
  
public:
  
  Matrix(Alien::BlockMatrix& alien_block_matrix,
     Arcane::ConstArray2View<Arcane::Integer> indexes)
    : m_data(alien_block_matrix, indexes) {}
  
public:

  MatrixProxyRow operator[](const Arcane::Integer equation) {
    m_data.m_equation = equation;
    return MatrixProxyRow(m_data);
  }

  Arcane::Real& operator()(const Arcane::Integer i,
               const Arcane::Cell k,
               const Arcane::Integer j,
               const Arcane::Cell l)
  {
    return m_data.m_values_2d[0][0];
  }

  Arcane::Real& operator()(const Arcane::Integer i,
               const Arcane::CellEnumerator k,
               const Arcane::Integer j,
               const Arcane::CellEnumerator l)
  {
    return m_data.m_values_2d[0][0];
  }

  Arcane::ConstArray2View<Arcane::Integer> allIndexes() const
  {
    return m_data.m_all_indexes;
  }

  const Alien::BlockMatrix& alienBlockMatrix() const
  {
    return m_data.m_alien_block_matrix;
  }

private:
 
  InternalData m_data;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}
