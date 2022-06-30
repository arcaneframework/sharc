// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef BINARYOP_H
#define BINARYOP_H

#include <cmath>
/*!
 * \file BinaryOp.h
 * \brief Definitions des operateurs
 * \date 01/04/2008
 * \author Xavier TUNC
 * 
 * Definition de cinq classes pour traiter toutes les operations courantes sur des tableaux. \n
 * La classe BinaryAdd est la classe utilisee dans le cas d'une addition (membre a membre) entre deux tableaux/expressions. \n
 * La classe BinaryMinus est la classe utilisee dans le cas d'une soustraction (membre a membre) entre deux tableaux/expressions. \n
 * La classe BinaryMultiply est la classe utilisee dans le cas d'une multiplication (membre a membre) entre deux tableaux/expressions. \n
 * La classe BinaryDiv est la classe utilisee dans le cas d'une division (membre a membre) entre deux tableaux/expressions. \n
 * La classe BinaryPow est la classe utilisee dans le cas d'un tableau eleve a la puissance n. \n
 * Si on effectue une operation entre deux tableaux/expressions, cette operation s'effectuera toujours membre a membre. \n
 * Si on effectue une operation entre un tableau/expression et un scalaire, cette operation s'effectuera sur tous les membres du tableau/expression. \n 
*/

/*! \brief BinaryAdd : Operation d'addition entre 2 types
 * 
 * Definit une methode static getValue() qui renvoie la somme des deux elements passes par reference a l'index i
 */
template <typename L, typename R> 
class BinaryAdd
{
public :
//! \brief type de l'operande de gauche
typedef typename L::value_type value_typeL ;
//! \brief type de l'operande de droite
typedef typename R::value_type value_typeR ;
//! \brief type du resultat de l'operation
typedef typename Promote<value_typeL,value_typeR>::type value_type ;

/*! \brief Retourne le resultat de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param index Index
 * \warning No bounds check
 */
static inline value_type getValue(const L & left, const R & right, int index)
{
  return (left.getValue(index) + right.getValue(index)) ;
}

/*! \brief Retourne le resultat de la derivee de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param ligne Premier index
 * \param colonne Deuxieme index
 * \warning No bounds check
 */
static inline value_type getDerivedValue(const L & left, const R & right, int ligne, int colonne)
{
  return (left.getDerivedValue(ligne,colonne) + right.getDerivedValue(ligne,colonne)) ;
}
} ;

/*! \brief BinaryMinus : Operation de soustraction entre 2 types
 * 
 * Definit une methode static getValue() qui renvoie la difference entre les deux elements passes par reference a l'index i
 */
template <typename L, typename R>
class BinaryMinus
{
public :
//! \brief type de l'operande de gauche
typedef typename L::value_type value_typeL ;
//! \brief type de l'operande de droite
typedef typename R::value_type value_typeR ;
//! \brief type du resultat de l'operation
typedef typename Promote<value_typeL,value_typeR>::type value_type ;

/*! \brief Retourne le resultat de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param index Index
 * \warning No bounds check
 */
static inline value_type getValue(const L & left, const R & right, int index) 
{
  return (left.getValue(index) - right.getValue(index)) ;
}

/*! \brief Retourne le resultat de la derivee de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param ligne Premier index
 * \param colonne Deuxieme index
 * \warning No bounds check
 */
static inline value_type getDerivedValue(const L & left, const R & right, int ligne, int colonne)
{
  return (left.getDerivedValue(ligne,colonne) - right.getDerivedValue(ligne,colonne)) ;
}
} ;

/*! \brief BinaryMultiply : Operation de multiplication entre 2 types
 * 
 * Definit une methode static getValue() qui renvoie le produit entre les deux elements passes par reference a l'index i
 */
template <typename L, typename R>
class BinaryMultiply
{
public :
//! \brief type de l'operande de gauche
typedef typename L::value_type value_typeL ;
//! \brief type de l'operande de droite
typedef typename R::value_type value_typeR ;
//! \brief type du resultat de l'operation
typedef typename Promote<value_typeL,value_typeR>::type value_type ;

/*! \brief Retourne le resultat de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param index Index
 * \warning No bounds check
 */
static inline value_type getValue(const L & left, const R & right, int index)
{
  return (left.getValue(index) * right.getValue(index)) ;
}

/*! \brief Retourne le resultat de la derivee de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param ligne Premier index
 * \param colonne Deuxieme index
 * \warning No bounds check
 */
static inline value_type getDerivedValue(const L & left, const R & right, int ligne, int colonne) 
{
  return ((left.getDerivedValue(ligne,colonne)*right.getValue(ligne)) + (left.getValue(ligne)*right.getDerivedValue(ligne,colonne))) ;
}
} ;

/*! \brief BinaryDiv : Operation de division entre 2 types
 * 
 * Definit une methode static getValue() qui renvoie le rapport entre les deux elements passes par reference a l'index i
 */
template <typename L, typename R>
class BinaryDiv
{
public :
//! \brief type de l'operande de gauche
typedef typename L::value_type value_typeL ;
//! \brief type de l'operande de droite
typedef typename R::value_type value_typeR ;
//! \brief type du resultat de l'operation
typedef typename Promote<value_typeL,value_typeR>::type value_type ;

/*! \brief Retourne le resultat de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param index Index
 * \warning No bounds check
 */
static inline value_type getValue(const L & left, const R & right, int index)
{
  return (left.getValue(index) / right.getValue(index)) ;
}

/*! \brief Retourne le resultat de la derivee de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param ligne Premier index
 * \param colonne Deuxieme index
 * \warning No bounds check
 */
static inline value_type getDerivedValue(const L & left, const R & right, int ligne, int colonne) 
{
  return (left.getDerivedValue(ligne,colonne)*right.getValue(ligne) - left.getValue(ligne)*right.getDerivedValue(ligne,colonne))/(right.getValue(ligne) * right.getValue(ligne) ) ;
}
} ;

/*! \brief BinaryPow : Operation de puissance entre 2 types
 * 
 * Definit une methode static getValue() qui renvoie le premier element eleve a la puissance du deuxieme element a l'index i. \n
 * Si la puissance est entiere, on calcule la puissance sans faire appel a la fonction pow de cmath
 */
template <typename T, typename power>
class BinaryPow
{
public :
//! \brief type de l'operande de gauche
typedef typename T::value_type value_typeL ;
//! \brief type de l'operande de droite
typedef typename power::value_type value_typeR ;
//! \brief type du resultat de l'operation
typedef typename Promote<value_typeL,value_typeR>::type value_type ;

/*! \brief Retourne le resultat de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param index Index
 * \warning No bounds check
 */


static inline value_type getValue(const T & left, const Cst<int> & right, int index)
{
  return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ;
}

static inline value_type getValue(const T & left, const Array<int> & right, int index)
{
  return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ;
}

static inline value_type getValue(const T & left, const Fad<Cst<int> > & right, int index)
{
  return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ;
}

static inline value_type getValue(const T & left, const Fad<Array<int> > & right, int index)
{
  return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ;
}


static inline value_type getValue(const T & left, const int right, int index)
{
  if (right == 1)
  {
    return left.getValue(index) ;
  }
  else if (right != 0)
  {
    return left.getValue(index)*BinaryPow::getValue(left,right-1,index) ;
  }
  else 
  {
    return 1;
  }
}

//! \brief Macro pour generaliser la fonction getValue() dans les autres cas (class BinaryPow)
#define PUISSANCE(power) \
static inline value_type getValue(const T & left, const power & right, int index) \
{ \
  if (right.getValue(index)==(unsigned int) right.getValue(index)) \
  { \
    return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ; \
  } \
  else \
  { \
    return std::pow(left.getValue(index),right.getValue(index)) ; \
  } \
} \

PUISSANCE(Cst<double>)
PUISSANCE(Cst<float>)
PUISSANCE(Array<double>)
PUISSANCE(Array<float>)
PUISSANCE(Fad<Cst<double> >)
PUISSANCE(Fad<Cst<float> >)
PUISSANCE(Fad<Array<double> >)
PUISSANCE(Fad<Array<float> >)
PUISSANCE(IExpr<int>)
PUISSANCE(IExpr<float>)
PUISSANCE(IExpr<double>)

#undef PUISSANCE

template <typename xpr1,typename xpr2,typename xpr3>
static inline value_type getValue(const T & left, const Expr<xpr1,xpr2,xpr3> & right, int index)
{
  if (right.getValue(index)==(unsigned int) right.getValue(index))
  {
  return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ;
  }
  else 
  {
    return std::pow(left.getValue(index), right.getValue(index)) ;
  }
}

template <typename xpr1,typename xpr2,typename xpr3>
static inline value_type getValue(const T & left, const ExprG<xpr1,xpr2,xpr3> & right, int index)
{
  if (right.getValue(index)==(unsigned int)right.getValue(index))
  {
  return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ;
  }
  else 
  {
    return std::pow(left.getValue(index), right.getValue(index)) ;
  }
}

template <typename xpr1,typename xpr2,typename xpr3>
static inline value_type getValue(const T & left, const ExprD<xpr1,xpr2,xpr3> & right, int index)
{
if (right.getValue(index)==(unsigned int)right.getValue(index))
  {
  return left.getValue(index)*BinaryPow::getValue(left,right.getValue(index)-1,index) ;
  }
  else 
  {
    return std::pow(left.getValue(index), right.getValue(index)) ;
  }
}

/*! \brief Retourne le resultat de la derivee de l'operation
 * 
 * \param left Operande de gauche
 * \param right Operande de droite
 * \param ligne Premier index
 * \param colonne Deuxieme index
 * \warning No bounds check
 */
static inline value_type getDerivedValue(const T & left, const power & right, int ligne, int colonne)
{
    return right.getValue(ligne)*left.getDerivedValue(ligne,colonne)* std::pow(left.getValue(ligne),right.getValue(ligne)-1) ; //+ right.getDerivedValue(ligne,colonne)*log(left.getValue(ligne))*exp(right.getValue(ligne)*log(left.getValue(ligne))) ;
}

} ;

/*! \brief BinarySum : Operation d'addition entre deux Real3 (et/ou expression)
 * 
 * Definit une methode static getX() (resp getY() et getZ()) qui renvoie la somme des x (resp. y et z) des deux elements passes en parametre. \n
 */
class BinarySum
{
public :
  typedef double value_type ;

#define BINSUM(type,type2) \
static inline double type(const Real3 & re1, const Real3 & re2) \
  { \
    return re1.type2 + re2.type2 ; \
  } \
template <typename xpr1,typename xpr2, typename xpr3> \
static inline double type(const Expr<xpr1,xpr2,xpr3> & re1, const Real3 & re2) \
  { \
    return re1.type() + re2.type2 ; \
  } \
template <typename xpr1,typename xpr2, typename xpr3> \
static inline double type(const Real3 & re1, const Expr<xpr1,xpr2,xpr3> & re2) \
  { \
    return re1.type2 + re2.type() ; \
  } \
template <typename xpr1,typename xpr2, typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
static inline double type(const Expr<xpr1,xpr2,xpr3> & re1, const Expr<xpr4,xpr5,xpr6> & re2) \
  { \
    return re1.type() + re2.type() ; \
  }\

BINSUM(getX,m_x)
BINSUM(getY,m_y)
BINSUM(getZ,m_z)

#undef BINSUM

};

//! \brief Macro pour generer les differentes operations pour tous les types possibles (tableaux, expressions, etc).
#define BinOp(op, type) \
template <typename A, typename B> \
inline Expr<Array<A>, type<Array<A>,Array<B> > , Array<B> > \
op (const Array<A> & L , const Array<B> & R) \
{ \
  typedef type<Array<A>, Array<B> > value_type ; \
  return Expr< Array<A>, value_type, Array<B> > (L,R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<Array<A>, type<Array<A>, Expr<xpr1,xpr2,xpr3> >, Expr<xpr1,xpr2,xpr3> > \
op (const Array<A> & L , const Expr<xpr1,xpr2,xpr3> & R) \
{ \
  typedef type<Array<A>, Expr<xpr1,xpr2,xpr3> > value_type ; \
  return Expr< Array<A> , value_type, Expr<xpr1, xpr2, xpr3> > (L, R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<Expr<xpr1,xpr2,xpr3>, type<Expr<xpr1,xpr2,xpr3>,Array<A> >, Array<A> > \
op (const Expr<xpr1,xpr2,xpr3> & L , const Array<A> & R) \
{ \
  typedef type<Expr<xpr1,xpr2,xpr3>, Array<A> > value_type ; \
  return Expr< Expr<xpr1, xpr2, xpr3>, value_type, Array<A> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<Expr<xpr1,xpr2,xpr3>, type<Expr<xpr1,xpr2,xpr3>, Expr<xpr4,xpr5,xpr6> >, Expr<xpr4,xpr5,xpr6> > \
op (const Expr<xpr1, xpr2, xpr3> & L , const Expr<xpr4,xpr5,xpr6> & R) \
{ \
  typedef type<Expr<xpr1,xpr2,xpr3>,Expr<xpr4,xpr5,xpr6> > value_type ; \
  return Expr< Expr<xpr1, xpr2, xpr3>, value_type, Expr<xpr4,xpr5,xpr6> > (L, R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<Array<A>, type<Array<A>, ExprG<xpr1,xpr2,xpr3> >, ExprG<xpr1,xpr2,xpr3> > \
op (const Array<A> & L , const ExprG<xpr1,xpr2,xpr3> & R) \
{ \
  typedef type<Array<A>, ExprG<xpr1,xpr2,xpr3> > value_type ; \
  return Expr< Array<A> , value_type, ExprG<xpr1, xpr2, xpr3> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<Expr<xpr1,xpr2,xpr3>, type<Expr<xpr1,xpr2,xpr3>, ExprG<xpr4,xpr5,xpr6> >, ExprG<xpr4,xpr5,xpr6> > \
op (const Expr<xpr1, xpr2, xpr3> & L , const ExprG<xpr4,xpr5,xpr6> & R) \
{ \
  typedef type<Expr<xpr1,xpr2,xpr3>,ExprG<xpr4,xpr5,xpr6> > value_type ; \
  return Expr< Expr<xpr1, xpr2, xpr3>, value_type, ExprG<xpr4,xpr5,xpr6> > (L, R) ; \
} \
template <typename A, typename xpr1,typename xpr2, typename xpr3> \
inline ExprG<Cst<A>, type<Cst<A>, ExprG<xpr1,xpr2,xpr3> >, ExprG<xpr1,xpr2,xpr3> > \
op (const A L , const ExprG<xpr1,xpr2,xpr3> & R) \
{ \
  typedef type<Cst<A>,ExprG<xpr1,xpr2,xpr3> > value_type ; \
  return ExprG< Cst<A>, value_type, ExprG<xpr1,xpr2,xpr3> > (Cst<A>(L), R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<ExprG<xpr1,xpr2,xpr3>, type<ExprG<xpr1,xpr2,xpr3>,Array<A> >, Array<A> > \
op (const ExprG<xpr1,xpr2,xpr3> & L , const Array<A> & R) \
{ \
  typedef type<ExprG<xpr1,xpr2,xpr3>, Array<A> > value_type ; \
  return Expr< ExprG<xpr1, xpr2, xpr3> , value_type, Array<A> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<ExprG<xpr4,xpr5,xpr6>, type<ExprG<xpr4,xpr5,xpr6>, Expr<xpr1,xpr2,xpr3> >, Expr<xpr1,xpr2,xpr3> > \
op (const ExprG<xpr4,xpr5,xpr6> & L , const Expr<xpr1, xpr2, xpr3> & R) \
{ \
  typedef type<ExprG<xpr4,xpr5,xpr6> , Expr<xpr1,xpr2,xpr3> > value_type ; \
  return Expr< ExprG<xpr4,xpr5,xpr6> , value_type, Expr<xpr1, xpr2, xpr3> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<ExprG<xpr1,xpr2,xpr3>, type<ExprG<xpr1,xpr2,xpr3>, ExprG<xpr4,xpr5,xpr6> >, ExprG<xpr4,xpr5,xpr6> > \
op (const ExprG<xpr1, xpr2, xpr3> & L , const ExprG<xpr4,xpr5,xpr6> & R) \
{ \
  typedef type<ExprG<xpr1,xpr2,xpr3>,ExprG<xpr4,xpr5,xpr6> > value_type ; \
  return Expr< ExprG<xpr1, xpr2, xpr3>, value_type, ExprG<xpr4,xpr5,xpr6> > (L, R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<Array<A>, type<Array<A>, ExprD<xpr1,xpr2,xpr3> >, ExprD<xpr1,xpr2,xpr3> > \
op (const Array<A> & L , const ExprD<xpr1,xpr2,xpr3> & R) \
{ \
  typedef type<Array<A>, ExprD<xpr1,xpr2,xpr3> > value_type ; \
  return Expr< Array<A> , value_type, ExprD<xpr1, xpr2, xpr3> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<Expr<xpr1,xpr2,xpr3>, type<Expr<xpr1,xpr2,xpr3>, ExprD<xpr4,xpr5,xpr6> >, ExprD<xpr4,xpr5,xpr6> > \
op (const Expr<xpr1, xpr2, xpr3> & L , const ExprD<xpr4,xpr5,xpr6> & R) \
{ \
  typedef type<Expr<xpr1,xpr2,xpr3>,ExprD<xpr4,xpr5,xpr6> > value_type ; \
  return Expr< Expr<xpr1, xpr2, xpr3>, value_type, ExprD<xpr4,xpr5,xpr6> > (L, R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<ExprD<xpr1,xpr2,xpr3>, type<ExprD<xpr1,xpr2,xpr3>,Array<A> >, Array<A> > \
op (const ExprD<xpr1,xpr2,xpr3> & L , const Array<A> & R) \
{ \
  typedef type<ExprD<xpr1,xpr2,xpr3>, Array<A> > value_type ; \
  return Expr< ExprD<xpr1, xpr2, xpr3> , value_type, Array<A> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<ExprD<xpr4,xpr5,xpr6>, type<ExprD<xpr4,xpr5,xpr6>, Expr<xpr1,xpr2,xpr3> >, Expr<xpr1,xpr2,xpr3> > \
op (const ExprD<xpr4,xpr5,xpr6> & L , const Expr<xpr1, xpr2, xpr3> & R) \
{ \
  typedef type<ExprD<xpr4,xpr5,xpr6>,Expr<xpr1,xpr2,xpr3> > value_type ; \
  return Expr< ExprD<xpr4,xpr5,xpr6> , value_type, Expr<xpr1, xpr2, xpr3> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<ExprD<xpr1,xpr2,xpr3>, type<ExprD<xpr1,xpr2,xpr3>, ExprD<xpr4,xpr5,xpr6> >, ExprD<xpr4,xpr5,xpr6> > \
op (const ExprD<xpr1, xpr2, xpr3> & L , const ExprD<xpr4,xpr5,xpr6> & R) \
{ \
  typedef type<ExprD<xpr1,xpr2,xpr3>,ExprD<xpr4,xpr5,xpr6> > value_type ; \
  return Expr< ExprD<xpr1, xpr2, xpr3>, value_type, ExprD<xpr4,xpr5,xpr6> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<ExprG<xpr1,xpr2,xpr3>, type<ExprG<xpr1,xpr2,xpr3>, ExprD<xpr4,xpr5,xpr6> >, ExprD<xpr4,xpr5,xpr6> > \
op (const ExprG<xpr1, xpr2, xpr3> & L , const ExprD<xpr4,xpr5,xpr6> & R) \
{ \
  typedef type<ExprG<xpr1,xpr2,xpr3>,ExprD<xpr4,xpr5,xpr6> > value_type ; \
  return Expr< ExprG<xpr1, xpr2, xpr3>, value_type, ExprD<xpr4,xpr5,xpr6> > (L, R) ; \
} \
template <typename xpr1, typename xpr2,typename xpr3, typename xpr4,typename xpr5, typename xpr6> \
inline Expr<ExprD<xpr1,xpr2,xpr3>, type<ExprD<xpr1,xpr2,xpr3>, ExprG<xpr4,xpr5,xpr6> >, ExprG<xpr4,xpr5,xpr6> > \
op (const ExprD<xpr1, xpr2, xpr3> & L , const ExprG<xpr4,xpr5,xpr6> & R) \
{ \
  typedef type<ExprD<xpr1,xpr2,xpr3>, ExprG<xpr4,xpr5,xpr6> > value_type ; \
  return Expr< ExprD<xpr1, xpr2, xpr3>, value_type, ExprG<xpr4,xpr5,xpr6> > (L, R) ; \
} \

BinOp(operator+,BinaryAdd)
BinOp(operator-,BinaryMinus)
BinOp(operator*,BinaryMultiply)
BinOp(operator/,BinaryDiv)
BinOp(pow,BinaryPow)

#undef BINOP

//! \brief Macro pour generer les differentes operations pour tous les types possibles (tableaux, expressions, etc).
#define BINOPSCG(op,type) \
template <typename A,typename B> \
inline ExprG<Cst<B>, type<Cst<B>, Array<A> >, Array<A> > \
op (const B L , const Array<A> & R) \
{ \
  typedef type<Cst<B>, Array<A> > value_type ; \
  return ExprG< Cst<B>, value_type, Array<A> > (Cst<B>(L) ,R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline ExprG< Cst<A>, type<Cst<A>, Expr<xpr1,xpr2,xpr3> > , Expr<xpr1,xpr2,xpr3> > \
op (const A L , const Expr<xpr1,xpr2,xpr3> & R) \
{ \
  typedef type<Cst<A>, Expr<xpr1,xpr2,xpr3> > value_type ; \
  return ExprG< Cst<A>, value_type , Expr<xpr1,xpr2,xpr3> > (Cst<A> (L), R) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline ExprG< Cst<A>, type<Cst<A>, ExprD<xpr1,xpr2,xpr3> > , ExprD<xpr1,xpr2,xpr3> > \
op (const A L , const ExprD<xpr1,xpr2,xpr3> & R) \
{ \
  typedef type<Cst<A>, ExprD<xpr1,xpr2,xpr3> > value_type ; \
  return ExprG< Cst<A>, value_type , ExprD<xpr1,xpr2,xpr3> > (Cst<A> (L), R) ; \
} \

BINOPSCG(operator+,BinaryAdd)
BINOPSCG(operator-,BinaryMinus)
BINOPSCG(operator*,BinaryMultiply)
BINOPSCG(operator/,BinaryDiv)
BINOPSCG(pow,BinaryPow)
#undef BINOPSCG

//! \brief Macro pour generer les differentes operations pour tous les types possibles (tableaux, expressions, etc).
#define BINOPSCD(op,type) \
template <typename A,typename B> \
inline ExprD<Array<A>, type<Array<A>, Cst<B> >, Cst<B> > \
op (const Array<A> & L , const B R) \
{ \
  typedef type<Array<A>, Cst<B> > value_type ; \
  return ExprD<Array<A> , value_type, Cst<B> > ( L,Cst<B>(R)) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline ExprD< Expr<xpr1,xpr2,xpr3>, type<Expr<xpr1,xpr2,xpr3>,Cst<A> > , Cst<A> > \
op (const ExprD<xpr1,xpr2,xpr3> & L , const A R) \
{ \
  typedef type<Expr<xpr1,xpr2,xpr3> ,Cst<A> > value_type ; \
  return ExprD< Expr<xpr1,xpr2,xpr3>, value_type , Cst<A> > (L,Cst<A> (R)) ; \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline ExprD< ExprG<xpr1,xpr2,xpr3>, type<ExprG<xpr1,xpr2,xpr3>,Cst<A> > , Cst<A> > \
op (const ExprG<xpr1,xpr2,xpr3> & L , const A R) \
{ \
  typedef type<ExprG<xpr1,xpr2,xpr3> ,Cst<A> > value_type ; \
  return ExprD< ExprG<xpr1,xpr2,xpr3>, value_type , Cst<A> > (L,Cst<A> (R)) ; \
} \

BINOPSCD(operator+,BinaryAdd)
BINOPSCD(operator-,BinaryMinus)
BINOPSCD(operator*,BinaryMultiply)
BINOPSCD(operator/,BinaryDiv)
BINOPSCD(pow,BinaryPow)
#undef BINOPSCD

//! \brief Macro pour generer les differentes operations pour tous les types possibles (tableaux, expressions, etc).
#define BINOP_FAD(op,type) \
template <typename A, typename B> \
inline Expr<Fad<A>, type< Fad<A>, Fad<B>  >, Fad<B> > op (const Fad<A> & L , const Fad<B> & R) { \
return Expr<Fad<A>, type< Fad<A>, Fad<B> >, Fad<B> > (L, R); \
} \
template <typename xpr1, typename xpr2, typename xpr3, typename B> \
inline Expr<Expr<xpr1,xpr2,xpr3>, type< Expr<xpr1,xpr2,xpr3>, Fad<B> >, Fad<B> > op (const Expr<xpr1,xpr2,xpr3> & L , const Fad<B> & R) { \
return Expr< Expr<xpr1,xpr2,xpr3>, type< Expr<xpr1,xpr2,xpr3>, Fad<B> >, Fad<B> > (L, R); \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<Fad<A>, type< Fad<A>, Expr<xpr1,xpr2,xpr3> > , Expr<xpr1,xpr2,xpr3> > op (const Fad<A> & L , const Expr<xpr1,xpr2,xpr3> & R) { \
return Expr< Fad<A>, type< Fad<A>, Expr<xpr1,xpr2,xpr3> >, Expr<xpr1,xpr2,xpr3> > (L, R); \
} \
template <typename A, typename B> \
inline Expr<Fad<A>, type< Fad<A>, Array<B> >, Array<B> > op (const Fad<A> & L , const Array<B> & R) { \
return Expr< Fad<A>, type< Fad<A>, Array<B> >, Array<B> > (L, R); \
} \
template <typename A, typename B> \
inline Expr<Array<A>, type< Array<A>, Fad<B> >, Fad<B> > op (const Array<A> & L , const Fad<B> & R) { \
return Expr< Array<A>, type< Array<A>, Fad<B> > ,Fad<B> > (L, R); \
} \
template <typename A, typename B> \
inline ExprD<Fad<A>, type< Fad<A>, Cst<B> >, Cst<B> > op (const Fad<A> & L , const B R) { \
return ExprD< Fad<A>, type< Fad<A>, Cst<B> >, Cst<B> > (L, Cst<B> (R)); \
} \
template <typename A, typename B> \
inline ExprG<Cst<A>, type< Cst<A>, Fad<B> >, Fad<B> > op (const A L , const Fad<B> & R) { \
return ExprG< Cst<A>, type< Cst<A>, Fad<B> >, Fad<B> > (Cst<A> (L), R ); \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<Fad<A>, type< Fad<A>, ExprD<xpr1,xpr2,xpr3> > , ExprD<xpr1,xpr2,xpr3> > op (const Fad<A> & L , const ExprD<xpr1,xpr2,xpr3> & R) { \
return Expr< Fad<A>, type< Fad<A>, ExprD<xpr1,xpr2,xpr3> >, ExprD<xpr1,xpr2,xpr3> > (L, R); \
} \
template <typename A, typename xpr1, typename xpr2, typename xpr3> \
inline Expr<Fad<A>, type< Fad<A>, ExprG<xpr1,xpr2,xpr3> > , ExprG<xpr1,xpr2,xpr3> > op (const Fad<A> & L , const ExprG<xpr1,xpr2,xpr3> & R) { \
return Expr< Fad<A>, type< Fad<A>, ExprG<xpr1,xpr2,xpr3> >, ExprG<xpr1,xpr2,xpr3> > (L, R); \
} \
template <typename xpr1, typename xpr2, typename xpr3, typename B> \
inline Expr<ExprD<xpr1,xpr2,xpr3>, type< ExprD<xpr1,xpr2,xpr3>, Fad<B> >, Fad<B> > op (const ExprD<xpr1,xpr2,xpr3> & L , const Fad<B> & R) { \
return Expr< ExprD<xpr1,xpr2,xpr3>, type< ExprD<xpr1,xpr2,xpr3>, Fad<B> >, Fad<B> > (L, R); \
} \
template <typename xpr1, typename xpr2, typename xpr3, typename B> \
inline Expr<ExprG<xpr1,xpr2,xpr3>, type< ExprG<xpr1,xpr2,xpr3>, Fad<B> >, Fad<B> > op (const ExprG<xpr1,xpr2,xpr3> & L , const Fad<B> & R) { \
return Expr< ExprG<xpr1,xpr2,xpr3>, type< ExprG<xpr1,xpr2,xpr3>, Fad<B> >, Fad<B> > (L, R); \
} \

BINOP_FAD(operator+,BinaryAdd)
BINOP_FAD(operator-,BinaryMinus)
BINOP_FAD(operator*,BinaryMultiply)
BINOP_FAD(operator/,BinaryDiv)
BINOP_FAD(pow,BinaryPow)
#undef BINOP_FAD

     /*

inline Expr<Real3,BinarySum,Real3> sum(const Real3 & re1, const Real3 & re2) 
{
  return Expr<Real3,BinarySum,Real3> (re1,re2) ;
}

template <typename xpr1,typename xpr2, typename xpr3>
inline Expr<Expr<xpr1,xpr2,xpr3>,BinarySum,Real3> sum(const Expr<xpr1,xpr2,xpr3> &  re1,const Real3 & re2)
{
  return Expr<Expr<xpr1,xpr2,xpr3>, BinarySum, Real3> (re1,re2) ;
}

template <typename xpr1,typename xpr2, typename xpr3>
inline Expr<Real3,BinarySum,Expr<xpr1,xpr2,xpr3> > sum(const Real3  & re1, const Expr<xpr1,xpr2,xpr3> & re2)
{
  return Expr<Real3, BinarySum, Expr<xpr1,xpr2,xpr3> > (re1,re2) ;
}

inline Real3 operator+(const Real3 & re1, const Real3 & re2) 
{
  Real3 tmp ;
  tmp.m_x = re1.m_x + re2.m_x ;
  tmp.m_y = re1.m_y + re2.m_y ;
  tmp.m_z = re1.m_z + re2.m_z ;
  return tmp ;
}
*/
#endif
