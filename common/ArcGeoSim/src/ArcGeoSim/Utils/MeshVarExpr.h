// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef MESHVAREXPR_H_
#define MESHVAREXPR_H_

using Arcane::ItemEnumeratorT;
using Arcane::ItemGroupRangeIteratorT;

class ScalarMeshVar
{
public :
  ScalarMeshVar(Real a)
  : m_a(a)
  {}

  template<typename ItemT>
  Real operator[](const ItemEnumeratorT<ItemT>& icell) const
  {
    return m_a ;
  }
  template<typename ItemT>
  Real operator[](const ItemGroupRangeIteratorT<ItemT>& icell) const
  {
    return m_a ;
  }
  template<typename ItemT>
  Real operator[](const ItemT& cell) const
  {
    return m_a ;
  }
private :
  Real m_a;
} ;

template<class L,class R>
class MeshVarBinaryAdd ;

template<class R>
class MeshVarBinaryAdd<Real,R>
{
public :
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  {
    return l + r[icell] ; 
  }
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
  {
    return l + r[icell] ; 
  }
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemT& cell)
  {
    return l + r[cell] ;
  }
} ;

template<class L>
class MeshVarBinaryAdd<L,Real>
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const Real& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] + r ; }

  template<typename ItemT>
   static Real eval(const L& l,const Real& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l[icell] + r ; }

  template<typename ItemT>
   static Real eval(const L& l,const Real& r,const ItemT& cell)
   { return l[cell] + r ; }
} ;

template<class L,class R>
class MeshVarBinaryAdd
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] + r[icell] ; }

  template<typename ItemT>
   static Real eval(const L& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l[icell] + r[icell] ; }

  template<typename ItemT>
   static Real eval(const L& l,const R& r,const ItemT& cell)
   { return l[cell] + r[cell] ; }
} ;

template<class L,class R>
class MeshVarBinaryMinus ;

template<class R>
class MeshVarBinaryMinus<Real,R>
{
public :
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  {
    return l - r[icell] ;
  }
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
  {
    return l - r[icell] ;
  }
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemT& cell)
  {
    return l - r[cell] ;
  }
} ;

template<class L>
class MeshVarBinaryMinus<L,Real>
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const Real& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] - r ; }

  template<typename ItemT>
  static Real eval(const L& l,const Real& r,const ItemGroupRangeIteratorT<ItemT>& icell)
  { return l[icell] - r ; }

  template<typename ItemT>
  static Real eval(const L& l,const Real& r,const ItemT& cell)
  { return l[cell] - r ; }
} ;

template<class L,class R>
class MeshVarBinaryMinus
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] - r[icell] ; }

  template<typename ItemT>
  static Real eval(const L& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
  { return l[icell] - r[icell] ; }

  template<typename ItemT>
  static Real eval(const L& l,const R& r,const ItemT& cell)
  { return l[cell] - r[cell] ; }
} ;

template<class L,class R>
class MeshVarBinaryMult ;

template<class R>
class MeshVarBinaryMult<Real,R>
{
public :
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  { return l * r[icell] ; }

  template<typename ItemT>
   static Real eval(const Real& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l * r[icell] ; }

  template<typename ItemT>
   static Real eval(const Real& l,const R& r,const ItemT& cell)
   { return l * r[cell] ; }
} ;

template<class L>
class MeshVarBinaryMult<L,Real>
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const Real& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] * r ; }

  template<typename ItemT>
   static Real eval(const L& l,const Real& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l[icell] * r ; }

  template<typename ItemT>
   static Real eval(const L& l,const Real& r,const ItemT& cell)
   { return l[cell] * r ; }
} ;

template<class L,class R>
class MeshVarBinaryMult
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] * r[icell] ; }

  template<typename ItemT>
   static Real eval(const L& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l[icell] * r[icell] ; }

  template<typename ItemT>
   static Real eval(const L& l,const R& r,const ItemT& cell)
   { return l[cell] * r[cell] ; }
} ;


template<class L,class R>
class MeshVarBinaryDiv ;

template<class R>
class MeshVarBinaryDiv<Real,R>
{
public :
  template<typename ItemT>
  static Real eval(const Real& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  { return l / r[icell] ; }

  template<typename ItemT>
   static Real eval(const Real& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l / r[icell] ; }

  template<typename ItemT>
   static Real eval(const Real& l,const R& r,const ItemT& cell)
   { return l / r[cell] ; }
} ;

template<class L>
class MeshVarBinaryDiv<L,Real>
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const Real& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] / r ; }

  template<typename ItemT>
   static Real eval(const L& l,const Real& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l[icell] / r ; }

  template<typename ItemT>
   static Real eval(const L& l,const Real& r,const ItemT& cell)
   { return l[cell] / r ; }
} ;

template<class L,class R>
class MeshVarBinaryDiv
{
public :
  template<typename ItemT>
  static Real eval(const L& l,const R& r,const ItemEnumeratorT<ItemT>& icell)
  { return l[icell] / r[icell] ; }

  template<typename ItemT>
   static Real eval(const L& l,const R& r,const ItemGroupRangeIteratorT<ItemT>& icell)
   { return l[icell] / r[icell] ; }

  template<typename ItemT>
   static Real eval(const L& l,const R& r,const ItemT& cell)
   { return l[cell] / r[cell] ; }
} ;
template<class L,class Op,class R>
class MeshVarExpr
{
public :
  MeshVarExpr(const L& l,const R& r)
  : m_l(l)
  , m_r(r)
  {}
  template<typename ItemT>
  Real operator[](const ItemEnumeratorT<ItemT>& icell) const
  {
    return Op::eval(m_l,m_r,icell) ;
  }
  template<typename ItemT>
  Real operator[](const ItemGroupRangeIteratorT<ItemT>& icell) const
  {
    return Op::eval(m_l,m_r,icell) ;
  }
  template<typename ItemT>
  Real operator[](const ItemT& cell) const
  {
    return Op::eval(m_l,m_r,cell) ;
  }
private :
  const L& m_l ;
  const R& m_r ;
} ;

namespace MeshVariableOperator
{
  template<class L,class R>
  inline MeshVarExpr<L,MeshVarBinaryAdd<L,R>,R> operator+(const L& l, const R& r)
  {
    return MeshVarExpr<L,MeshVarBinaryAdd<L,R>,R>(l,r) ;
  }
  
  template<class L,class R>
  inline MeshVarExpr<L,MeshVarBinaryMinus<L,R>,R> operator-(const L& l, const R& r)
  {
    return MeshVarExpr<L,MeshVarBinaryMinus<L,R>,R>(l,r) ;
  }

  template<class L,class R>
  inline MeshVarExpr<L,MeshVarBinaryMult<L,R>,R> operator*(const L& l, const R& r)
  {
    return MeshVarExpr<L,MeshVarBinaryMult<L,R>,R>(l,r) ;
  }
  
  template<class L,class R>
  inline MeshVarExpr<L,MeshVarBinaryDiv<L,R>,R> operator/(const L& l, const R& r)
  {
    return MeshVarExpr<L,MeshVarBinaryDiv<L,R>,R>(l,r) ;
  }
}

#endif /*MESHVAREXPR_H_*/
