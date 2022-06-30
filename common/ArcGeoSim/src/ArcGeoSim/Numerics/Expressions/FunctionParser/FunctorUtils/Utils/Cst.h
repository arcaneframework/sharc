// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef CST_H
#define CST_H

/*!
 * \file Cst.h
 * \brief Manipulation des scalaires
 * \date 01/04/2008
 * \author Xavier TUNC
 */

/*!
 * \brief Cst : Manipulation des scalaires.
 * 
 * Cette classe encapsule les scalaires. \n
 * On definit sa taille egale a -1, pour les differencier des tableaux nuls et des tableaux a un element. \n
 * La methode getSize() retourne -1. \n
 * La methode getValue() retourne la valeur de la constante. \n
 * Cette classe est utilise en interne, il n'est pas conseille de l'utiliser directement. \n
 */

template <typename T>
class Cst
{
private :
//! \brief Valeur de la constante	
T m_value ;

public :
//! \brief Type de la constante	
typedef T value_type ;

//! @name Constructeurs
//! @{
/*! \brief Constructeur
*  \param val valeur de la constante
*/
inline Cst(const T & val)
: m_value(val)
{
  ;
}

//! @}

//! @name Destructeur
//! @{
inline ~Cst()
{
  ;
}
//! @}

//! @name Accesseurs
//! @{

//! \brief Set la valeur de la constante
inline T & operator[](int i)
{
  return m_value ;
}

//! \brief Retourne la taille de la constante (-1)
inline int getSize() const
{
  return -1 ;
}

/*! \brief Retourne le nombre d'inconnues de l'expression
 */
inline int getNbInconnues() const 
{
    return 0 ;
}

/*!	\brief Retourne la valeur de la constante
 * \param i Index de la valeur recherche (fictif ici)
 */ 
inline T getValue(int i) const
{
  return m_value ;
}

inline int getOwn() const
{
  return 1 ;
}

/*! \brief Retourne la valeur de la derivee de la constante (0)
 * \param ligne (fictif)
 * \param colonne (fictif)
 */
inline value_type getDerivedValue(int ligne, int colonne) const
{
  return 0 ;
}

inline bool isContainer() const
{
  return false ;
}
//! @}
};

#endif
