// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef VARVECT_H
#define VARVECT_H

/*!
 * \file VarVect.h
 * \brief Structures pour evaluer une fonction ou pour evaluer une fonction et ses derivees.
 * \date 17/04/2008
 * \author Xavier TUNC
 * 
 * La classe VarVect est une structure permettant d'evaluer une fonction. \n
 * La classe dVarVect est une structure permettant d'evaluer une fonction et ses derivees .\n
 */

/*!
 * \brief VarVect : Structure permettant d'evaluer une fonction
 * 
 * La classe VarVect encapsule la liste des valeurs des parametres, ainsi que la valeur de la variable que l'on souhaite calculer. \n
 * L'attribut m_listVect est une matrice encapsulant ces valeurs. \n
 * La premiere ligne correspond systematiquement a la variable que l'on veut evaluer. \n
 * Les autres lignes correspondent aux differentes valeurs des parametres necessaires pour evaluer la variable. \n
 * Cet attribut doit etre de taille [1+nbParametres]x[taille du vecteur] (le 1 vient de la variable). \n
 * La taille du vecteur peut etre 1, dans le cas d'une evaluation scalaire. \n
 * L'attribut m_sizeVect est un tableau contenant la taille de la variable et de tous les parametres. \n
 * Sa taille est [1+nbParametres]. \n
 * La premiere valeur correspond a la taille de la variable et les autres a celles des differents parametres. \n
 */
class VarVect
{
public :
//! Matrice encapsulant les valeurs de la variable et des parametres
  double** m_listVect ;
//! Tableau encapsulant la taille de la variable et des parametres
  int* m_sizeVect ;
//! Nombre d'arguments  
  int m_nbParam ;
//! @name Constructeurs
//! @{
//! \brief Constructeur
  VarVect(int nbParam,double** listeV,int* size) :m_listVect(listeV), m_sizeVect(size),  m_nbParam(nbParam)
  {
    ;
  }
//! @}
//! @name Destructeur
//! @{
  ~VarVect()
  {
    ;
  }
//! @}

};

/*!
 * \brief Diff : Structure permettant d'evaluer une fonction et ses derivees
 * 
 * La classe Diff encapsule la liste des valeurs des parametres, la valeur de la variable que l'on souhaite calculer, la valeur de la derivee que l'on va calculer, le nombre de parametres par rapport auxquels on va deriver, ainsi que leur identifiant. \n
 * L'attribut m_listVect est une matrice encapsulant ces valeurs. \n
 * La premiere ligne correspond systematiquement a la variable que l'on veut evaluer. \n
 * Les autres lignes correspondent aux differentes valeurs des parametres necessaires pour evaluer la variable. \n
 * Cet attribut doit etre de taille [1+nbParametres]x[taille du vecteur] (le 1 vient de la variable). \n
 * La taille du vecteur peut etre 1, dans le cas d'une evaluation scalaire. \n
 * L'attribut m_sizeVect est un tableau contenant la taille de la variable et de tous les parametres. \n
 * Sa taille est [1+nbParametres]. \n
 * La premiere valeur correspond a la taille de la variable et les autres a celles des differents parametres. \n
 * L'attribut m_derivedValue est une matrice correspondant a la derivee de la variable. \n
 * Sa taille est [taille du vecteur]x[m_nbVar] .\n
 * L'attribut m_nbVar indique le nombre de parametres par rapport auxquels on va deriver. \n
 * L'attribut m_listDerived precise quels sont les parametres par rapport auxquels on derive. \n
 * Sa taille est [m_nbVar]. Si m_listDerived[i] vaut 1, on derive par rapport a cette variable, s'il vaut 0, on ne derive pas. \n
 */

template<typename Var>
class Diff
{
public :
//! La matrice encapsulant la variable et les parametres
  Var m_varVect ;
//! La matrice contenant la valeur de la derivee
  double** m_derivedValue ;
//! Le tableau indiquant par rapport a quels parametres on va deriveer
  int* m_listDerived ;
//! Le nombre de parametres par rapport auxquels on va deriver
  int m_nbVar ;

//! @name Constructeur
//! @{
  Diff(Var listeV, int* listD, double** derivedV, int nbV)
  : m_varVect(listeV), m_derivedValue(derivedV), m_listDerived(listD), m_nbVar(nbV)
  {
    ;
  }
//! @}
//! @name Destructeur
//! @{
  ~Diff()
  {
    ;
  }
//! @}
};

#endif
