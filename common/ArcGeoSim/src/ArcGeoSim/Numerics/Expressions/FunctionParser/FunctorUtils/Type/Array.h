// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifdef __INTEL_COMPILER
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable:981)
// remark #869: parameter "j" was never referenced
#pragma warning(disable:869)
// remark #177: variable "puissance" was declared but never referenced
#pragma warning(disable:177)
// remark #1572: floating-point equality and inequality comparisons are unreliable
#pragma warning(disable:1572)

// TMP
#pragma warning(disable:444)
#pragma warning(disable:1011)
#pragma warning(disable:1224)
#endif

#ifndef ARRAY_H
#define ARRAY_H

/*!
 * \file Array.h
 * \brief Tableau de type \a T.
 * \date 01/04/2008
 * \author Xavier TUNC
 */

/*!
 * \brief Array : tableau de type \a T.
 * 
 * Cette classe encapsule un tableau et son nombre d'elements. \n
 * L'acces a ses elements se fait par l'operateur operator[]() pour les modifier, et l'operateur operator()() pour les recuperer. \n
 * La methode getSize() permet de recuperer la taille du tableau. \n
 * La methode getValue() permet de recuperer la valeur a l'index i. \n
 * Cette classe gere la memoire du pointeur, en l'allouant lors du passage dans le constructeur et en l'effacant a la fin de l'instance. \n
 * En cas d'affectation ou de recopie de tableau, si la taille du tableau est superieure a la memoire reservee, le tableau est efface et re-alloue a la "bonne" taille. \n
 * En mode debug, aucune verification de depassement de tableau n'est effectuee, c'est a l'utilisateur de s'en assurer. (a ajouter ?) \n
 */

#include <iostream>

template <typename T>
class Array
{
private :
//! Valeurs des elements stockes dans le tableau.
T* m_value ;
//! Si la classe est owner de m_dval
int m_own ;
//! Nombre d'elements.
int m_size ;

public :
//! Tableau nul statique utilise dans la classe FAD
static Array<T> array_null ;
//! Type des elements du tableau. 
typedef T value_type ;

//! @name Constructeurs 
//! @{

/*! \brief Construit un tableau vide.
 */
inline Array() : m_size(0)
{
  m_value = 0 ;
  m_own = 1 ;
}

/*! \brief Construit un tableau de taille size avec value
 * 
 * Remplit le tableau avec size fois la valeur value.
 * \param size taille du tableau
 * \param value scalaire utilise pour initialiser l'ensemble des elements du tableau
 */
//inline Array(int size, T value) : m_size(size)
//{
//  m_value = new T[m_size] ;
//  for (int i=m_size-1;i>=0;--i)
//  {
//    m_value[i] = value ;
//  }
//  m_own = 1 ;
//}

/*! \brief Construit un tableau de taille size avec le tableau value
 * 
 * \param size taille du tableau
 * \param value tableau transmis
 *  \warning Le tableau construit est de taille size, si size est plus petit que la taille reelle de value, value ne sera copie que jusqu'a value[size-1]
 *  \warning Le tableau construit est de taille size, si size est plus grand que la taille reelle de value, il risque d'y avoir un depassement memoire
 */
inline Array(int size, T * value) : m_size(size)
{
  m_value = value ;
  m_own = 0 ;
}

/*! \brief Construit un tableau de taille size et initialise ses elements a 0 si l'instance own le tableau.
 * 
 * \param size taille du tableau
 */
inline Array(int size, int own) : m_own(own),m_size(size)
{
  if (m_own==1)
  {
    m_value = new T[m_size] ;
    for (int i=m_size-1;i>=0;--i)
    {
      m_value[i] = 0 ;
    }
  }
}

/*! \brief Construit un tableau a partir d'une expression.
 * 
 * \param exp Expression servant pour construire le tableau
 */
template <typename xpr1,typename xpr2, typename xpr3>
Array(const Expr<xpr1,xpr2,xpr3> & exp)
{
  m_size = exp.getSize() ;
  m_value = new T[m_size] ;
  for (int i=m_size-1;i>=0;--i)
  {
    m_value[i] = exp.getValue(i) ;
  }
  m_own = 1 ;
}

/*! \brief Construit un tableau a partir d'une expression.
 * 
  * \param exp Expression servant pour construire le tableau
 */
template <typename xpr1,typename xpr2, typename xpr3>
Array(const ExprG<xpr1,xpr2,xpr3> & exp)
{
  m_size = exp.getSize() ;
  m_value = new T[m_size] ;
  for (int i=m_size-1;i>=0;--i)
  {
    m_value[i] = exp.getValue(i) ;
  }
  m_own = 1 ;
}

/*! \brief Construit un tableau a partir d'une expression.
 * 
 * \param exp Expression servant pour construire le tableau
 */
template <typename xpr1,typename xpr2, typename xpr3>
Array(const ExprD<xpr1,xpr2,xpr3> & exp)
{
  m_size = exp.getSize() ;
  m_value = new T[m_size] ;
  for (int i=m_size-1;i>=0;--i)
  {
    m_value[i] = exp.getValue(i) ;
  }
  m_own = 1 ;
}

/*! \brief Constructeur par copie.
 * 
 * Fait appel a la methode copy(const Array<T> &).\n
 * La memoire est allouee (ou re-allouee) si necessaire (dans la methode copy(const Array<T> &)).\n
 * \param arr Le tableau a construire
 */ 
inline Array(const Array<T> & arr) 
{
  m_size = 0 ;
  copy(arr) ;
}

//! @}

//! @name Destructeur 
//! @{ 	
//! \brief Delete le tableau de valeurs.
inline ~Array() 
{
  if(m_own)
  {
    delete[] m_value ;
  }
}
//! @}

//! @name Surcharges d'operateurs
//! @{
/*! \brief Operateur d'affectation pour une expression.
 * 
 * \param xp Expression servant pour construire le tableau
 */ 
template <typename xpr1, typename xpr2, typename xpr3>
inline Array<T> & operator=(const Expr<xpr1,xpr2,xpr3> & xp )
{
  if (m_own) 
  {
    if(m_size==0) 
    {
      m_size = xp.getSize() ;
      m_value = new T[m_size] ;
    }
    else
    {
      if(m_size!=xp.getSize())
      {
        delete [] m_value ;
        m_size = xp.getSize() ;
        m_value = new T[m_size] ;
      }
    }
  }
  register value_type tmp ;
  for (int i=m_size-1;i>=0;--i)
  {
    tmp = xp.getValue(i) ;
    m_value[i] = tmp ;
  }
  return (*this) ;
}


inline Array<T> & operator=(IExpr<T> * xp )
{
  if (m_own) 
  {
    if(m_size==0) 
    {
      m_size = xp->getSize() ;
      m_value = new T[m_size] ;
    }
    else
    {
      if(m_size!=xp->getSize())
      {
        delete [] m_value ;
        m_size = xp->getSize() ;
        m_value = new T[m_size] ;
      }
    }
  }
  xp->getValue(m_size,m_value) ;
  return (*this) ;
}


/*! \brief Operateur d'affectation pour une expression.
 * 
 * \param xp Expression servant pour construire le tableau
 */ 
template <typename xpr1, typename xpr2, typename xpr3>
inline Array<T> & operator=(const ExprG<xpr1,xpr2,xpr3> & xp )
{
 if (m_own) 
  {
    if(m_size==0) 
    {
      m_size = xp.getSize() ;
      m_value = new T[m_size] ;
    }
    else
    {
      if(m_size!=xp.getSize())
      {
        delete [] m_value ;
        m_size = xp.getSize() ;
        m_value = new T[m_size] ;
      }
    }
  }
  register value_type tmp ;
  for (int i=m_size-1;i>=0;--i)
  {
    tmp = xp.getValue(i) ;
    m_value[i] = tmp ;
  }
  return (*this) ;
}
	
/*! \brief Operateur d'affectation pour une expression.
 * 
 * \param xp Expression servant pour construire le tableau
 */ 
template <typename xpr1, typename xpr2, typename xpr3>
inline Array<T> & operator=(const ExprD<xpr1,xpr2,xpr3> & xp )
{
 if (m_own) 
  {
    if(m_size==0) 
    {
      m_size = xp.getSize() ;
      m_value = new T[m_size] ;
    }
    else
    {
      if(m_size!=xp.getSize())
      {
        delete [] m_value ;
        m_size = xp.getSize() ;
        m_value = new T[m_size] ;
      }
    }
  }
  register value_type tmp ;
  for (int i=m_size-1;i>=0;--i)
  {
    tmp = xp.getValue(i) ;
    m_value[i] = tmp ;
  }
  return (*this) ;
}

/*! \brief Operateur d'affectation pour un tableau (appel a la fonction copy(const Array<T> &)).
 * 
 * \param arr Tableau qui va etre copie
 */
inline void operator=(const Array<T> & arr)
{
  copy(arr) ;
}

inline void operator=(T * arr)
{
  if(m_own)
  {
    for(int i=m_size-1;i>=0;--i)
    {
      m_value[i] = arr[i] ;
    }
  }
  else
  {
    m_value = arr ;
  }
}
/*! \brief Operateur operator()(int).
 * 
 * Acces a la i_ieme valeur du tableau en lecture.
 * 
 * \param i Index de la valeur dans le tableau
 * \warning Ne verifie pas si i est en dehors du tableau
 */
inline T operator() (int i)
{
  return m_value[i] ;
}

/*! \brief Operateur operator[](int).
 * 
 * Acces a la i_ieme valeur du tableau en ecriture.
 * \param i Index de la valeur dans le tableau
 * \warning Ne verifie pas si i est en dehors du tableau
 */
inline T & operator[] (int i)
{
  return m_value[i] ;
}
//! @}

//! @name Accesseurs 

//! @{ 
/*! \brief Recupere la taille du tableau (m_size).
*/
inline int getSize() const
{
  return m_size ;
}

inline int getOwn() const
{
  return m_own ;
}

/*! \brief Recupere la i_ieme valeur du tableau (m_value[i]).
 * 
 * \param i Index de la valeur dans le tableau
 * \warning Ne verifie pas si i est en dehors du tableau
 */
inline T getValue(int i) const
{
  return m_value[i] ;
}

inline bool isContainer() const
{
  return false ;
}
//! @name Autres
//! @{ 
/*! \brief Fonction de copie d'un tableau.
 * 
 * Fonction appelee dans le constructeur par recopie et dans l'operateur operator=(const Array<T> &). \n
 * Verifie si la memoire est bien allouee sinon l'alloue. \n
 * \param arr Le tableau a copier
 */
inline void copy(const Array<T> & arr)
{
  std::cout << "!!!COPY!!!\n" ;
  m_own = 1 ;
  if(m_size==0) 
  {
    m_size = arr.m_size ;
    m_value = new T[m_size] ;
  }
  else
  {
    if(m_size!=arr.m_size) 
    {
      delete[] m_value ;
      m_size = arr.m_size ;
      m_value = new T[m_size] ;
    }
  }
  register value_type tmp ;
  for (int i=m_size-1;i>=0;--i)
  {
    tmp = arr.m_value[i] ;
    m_value[i] = tmp ;
  }
}

/*! \brief Fonction pour afficher le tableau.
 */
inline void affiche() const 
{
  std::cout << "m_size : " << m_size << std::endl;
  std::cout << "Affiche" << std::endl;
  for (int i=0;i<m_size;i++) 
  {
    std::cout << m_value[i] << std::endl;
  }
  std::cout << "---------------------" << std::endl;
}
//! @}
};

template <typename T>
Array<T> Array<T>::array_null;

#endif
