// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef FAD_H
#define FAD_H

/*!
 * \file Fad.h
 * \brief Differenciation automatique pour un tableau de type \a T.
 * \date 07/04/2008
 * \author Xavier TUNC
 */

/*!
 * \brief Fad : Differenciation automatique pour un tableau de type \a T.
 * 
 * Cette classe encapsule un Array et la valeur de sa derivee. \n 
 * La methode getSize() permet de recuperer la taille du tableau. \n
 * La methode getValue() permet de recuperer la valeur du tableau a l'index i. \n
 * La methode getDerivedValue() permet de recuperer la valeur de la derivee a l'index i,j. \n
 * La methode getPos() permet de recuperer la position de la variable, son "identifiant". \n
 * La methode evalDiff() permet "d'activer" le calcul de derivee. \n
 */

template <typename T>
class Fad {

public :
//! Type des elements de la derivee
  typedef typename T::value_type value_type ;

private :
//! Valeurs de la derivee
  value_type** m_dval ;
//! Valeurs de la derivee
  int* m_dPos ;
//! Reference vers le tableau
  T & m_array ;
//! Position de la variable		
  int m_pos ;
//! Nombre de variables
  int m_nbInconnues ;
//! Si la classe est owner de m_dval
  int m_own ;
//! Nombre de lignes de m_dval (pour delete)
  int m_nLigne ;

public :
//! @name Constructeurs 
//! @{

/*! \brief Construit un FAD a partir du nombre d'inconnues
 * 
 * m_pos est initialise a -2. \n
 * m_dval est initialise a 0.\n
 * m_nbInconnues est initialise a nbInconnues. \n
 * \param nbInconnues Nombre d'inconnues 
 */
inline Fad(int nbInconnues) : m_array(T::array_null)  
{
  m_pos = -2 ;
  m_dval = new value_type*[1] ;
  m_dval[0] = new value_type(0) ;
  m_nbInconnues = nbInconnues ;
  m_own = 1 ;
  m_nLigne = 0 ;
  m_dPos = new int(0) ;
}

/*! \brief Construit un FAD a partir d'un tableau et de son nombre d'inconnues
 * 
 * m_array reference le tableau passe en parametre. \n
 * m_pos est initialise a -2. \n
 * m_dval est initialise a 0 .\n
 * m_nbInconnues est initialise a nbInconnues
 * \param value Array de type T. 
 * \param nbInconnues Le nombre de parametres par rapport auquels on va deriver
 */
inline Fad(T & value,int nbInconnues,int own) : m_array(value) 
{
  m_own = own ;
  if(m_own)
  {
    m_dval = new value_type*[1] ;
    m_dval[0] = new value_type(0) ;
  }
  m_pos = -2 ;
  m_nbInconnues = nbInconnues ;
  m_nLigne = 0 ;
  m_dPos = new int(0) ;
}

/*! \brief Construit un FAD a partir d'un tableau, d'une matrice de derivee et de son nombre d'inconnues
 * 
 * m_array reference le tableau passe en parametre. \n
 * m_pos est initialise a -2. \n
 * m_dval est initialise a 0 .\n
 * m_nbInconnues est initialise a nbInconnues. \n
 * \param value Array de type T.
 * \param valueDer La matrice de derivees de value
 * \param nbInconnues Le nombre de parametres par rapport auquels on va deriver 
 */
inline Fad(T & value, value_type** valueDer, int nbInconnues) : m_dval(valueDer), m_array(value)
{
  m_dPos = new int(0) ;
  m_pos = -2 ;
  m_nbInconnues = nbInconnues ;
  m_own = 0 ;
  m_nLigne = 0 ;
}

/*! \brief Constructeur par recopie
 *
 * Recopie le FAD value passe en parametre. \n
 * Fait appel a la methode copy(). \n 
 * \param value FAD de type T
 */
inline Fad(const Fad<T> & value) : m_array(value.m_array) 
{
  m_pos = -2 ;
  m_own = 1 ;
  m_dval = new value_type*[1] ;
  m_dval[0] = new value_type(0) ;
  m_dPos = new int(0) ;
  copy(value) ;
}
//! @}

//! @name Destructeur
//! @{
inline ~Fad()
{

if (m_pos==-2 || m_pos ==-1) 
{
  delete m_dPos ;
}
else 
{
  delete[] m_dPos ;
}

if (m_own)
{
  deleteFad() ;
}
}
//! @}

//! @name Surcharges d'operateurs
//! @{

/*! \brief Affectation d'un Array de type T
 * 
 * m_array reference le tableau passer en parametre. \n
 * m_pos est initialise a -2. \n
 * m_dval est initialise a 0 .\n
 * \param value Array de type T. 
 */
inline void operator=(const T & value) 
{
  m_array = value ;
  m_pos = -2 ;
  if (m_own)
  {
    deleteFad() ;
  }
  m_dval = new value_type*[1] ;
  m_dval[0] = new value_type(0) ;
  m_own = 1 ;
  m_nLigne = 0 ;
  if (sizeof(m_dPos) == sizeof(int)) 
  {
    delete m_dPos ;
  }
  else 
  {
    delete[] m_dPos ;
  }
  m_dPos = new int(0) ;
}

inline void operator=(value_type ** value) 
{
  m_dval = value ;
}

/*! \brief Affectation d'une Expression de type <xpr1, xpr2, xpr3>
 * 
 * m_array reference le tableau de l'expression passe en parametre. \n
 * m_pos est initialise a -1. \n
 * m_dval est alloue (si necessaire) puis calcule. \n
 * \param value Expression de type <xpr1, xpr2, xpr3> 
 */
template <typename xpr1, typename xpr2, typename xpr3>
inline void operator=(const Expr<xpr1,xpr2,xpr3> & value)
{
  m_array = value ;
  m_nLigne = value.getSize() ;
  m_nbInconnues = value.getNbInconnues() ;
  if (m_own)
  {
    deleteFad() ;
    m_dval = new value_type*[m_nLigne] ;
    for (int i=m_nLigne-1;i>=0;--i)
    {
      m_dval[i] = new value_type[m_nbInconnues] ;
    }
  }
  m_pos = -1 ;
  switch(m_nbInconnues) 
  {
    case 1 :
    {
      register value_type tmp ; 
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp = value.getDerivedValue(k,0) ;
        m_dval[k][0] = tmp ;
      }
      break ;
    }
    case 2 : 
    {
      register value_type tmp1 ;
      register value_type tmp2 ;
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp1 = value.getDerivedValue(k,0) ;
        tmp2 = value.getDerivedValue(k,1) ;
        m_dval[k][0] = tmp1 ;
        m_dval[k][1] = tmp2 ;
      }
      break ;
    }
    case 3 : 
    {
      register value_type tmp1 ;
      register value_type tmp2 ;
      register value_type tmp3 ;
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp1 = value.getDerivedValue(k,0) ;
        tmp2 = value.getDerivedValue(k,1) ;
        tmp3 = value.getDerivedValue(k,2) ;
        m_dval[k][0] = tmp1 ;
        m_dval[k][1] = tmp2 ;
        m_dval[k][2] = tmp3 ;
      }
      break ;
    }
    default : 
    {
      register value_type tmp ; 
      for (int k=m_nLigne-1;k>=0;--k)
      {
        for (int l=m_nbInconnues-1;l>=0;--l)
        {
          tmp = value.getDerivedValue(k,l) ;
          m_dval[k][l] = tmp ;
        }
      }
      break ;
    }
  }
}

/*! \brief Affectation d'une Expression de type <xpr1, xpr2, xpr3>
 * 
 * m_array reference le tableau de l'expression passe en parametre. \n
 * m_pos est initialise a -1. \n
 * m_dval est alloue puis recopier. \n 
 * \param value Expression de type <xpr1, xpr2, xpr3>. 
 */
template <typename xpr1, typename xpr2, typename xpr3>
inline void operator=(const ExprD<xpr1,xpr2,xpr3> & value) {
  m_array = value;
  m_nbInconnues = value.getNbInconnues() ;
  m_nLigne = m_array.getSize() ;
  if (m_own)
  {
    deleteFad() ;
    m_dval = new value_type*[m_nLigne] ;
    
    for (int i=m_nLigne-1;i>=0;--i)
    {
      m_dval[i] = new value_type[m_nbInconnues] ;
    }
  }
  m_pos = -1 ;
  switch(m_nbInconnues) 
  {
    case 1 : 
    {
      register value_type tmp ; 
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp = value.getDerivedValue(k,0) ;
        m_dval[k][0] = tmp ;
      }
      break ;
    }
    case 2 : 
    {
      register value_type tmp1 ;
      register value_type tmp2 ;
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp1 = value.getDerivedValue(k,0) ;
        tmp2 = value.getDerivedValue(k,1) ;
        m_dval[k][0] = tmp1 ;
        m_dval[k][1] = tmp2 ;
      }
      break ;
    }
    case 3 : 
    {
      register value_type tmp1 ;
      register value_type tmp2 ;
      register value_type tmp3 ;
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp1 = value.getDerivedValue(k,0) ;
        tmp2 = value.getDerivedValue(k,1) ;
        tmp3 = value.getDerivedValue(k,2) ;
        m_dval[k][0] = tmp1 ;
        m_dval[k][1] = tmp2 ;
        m_dval[k][2] = tmp3 ;
      }
      break ;
    }
    default :
    {
      register value_type tmp ; 
      for (int k=m_nLigne-1;k>=0;--k)
      {
        for (int l=m_nbInconnues-1;l>=0;--l)
        {
          tmp = value.getDerivedValue(k,l) ;
          m_dval[k][l] = tmp ;
        }
      }
      break ;
    }
  }
}

/*! \brief Affectation d'une Expression de type <xpr1, xpr2, xpr3>
 * 
 * m_array reference le tableau de l'expression passe en parametre. \n
 * m_pos est initialise a -1. \n
 * m_dval est alloue puis recopier. \n 
 * \param value Expression de type <xpr1, xpr2, xpr3>. 
 */
template <typename xpr1, typename xpr2, typename xpr3>
inline void operator=(const ExprG<xpr1,xpr2,xpr3> & value) {
  m_array = value;
  m_nbInconnues = value.getNbInconnues() ;
  m_nLigne = m_array.getSize() ;
  if (m_own)
  {
    deleteFad() ;
    m_dval = new value_type*[m_nLigne] ;
    for (int i=m_nLigne-1;i>=0;--i)
    {
      m_dval[i] = new value_type[m_nbInconnues] ;
    }
  }
  m_pos = -1 ;
  switch(m_nbInconnues) 
  {
    case 1 : 
    {
      register value_type tmp ; 
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp = value.getDerivedValue(k,0) ;
        m_dval[k][0] = tmp ;
      }
      break ;
    }
    case 2 : 
    {
      register value_type tmp1 ;
      register value_type tmp2 ;
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp1 = value.getDerivedValue(k,0) ;
        tmp2 = value.getDerivedValue(k,1) ;
        m_dval[k][0] = tmp1 ;
        m_dval[k][1] = tmp2 ;
      }
      break ;
    }
    case 3 :
    {
      register value_type tmp1 ;
      register value_type tmp2 ;
      register value_type tmp3 ;
      for (int k=m_nLigne-1;k>=0;--k)
      {
        tmp1 = value.getDerivedValue(k,0) ;
        tmp2 = value.getDerivedValue(k,1) ;
        tmp3 = value.getDerivedValue(k,2) ;
        m_dval[k][0] = tmp1 ;
        m_dval[k][1] = tmp2 ;
        m_dval[k][2] = tmp3 ;
      }
      break ;
    }
    default :
    {
      register value_type tmp ; 
      for (int k=m_nLigne-1;k>=0;--k)
      {
        for (int l=m_nbInconnues-1;l>=0;--l)
        {
          tmp = value.getDerivedValue(k,l) ;
          m_dval[k][l] = tmp ;
        }
      }
      break ;
    }
  }
}

/*! \brief Affectation d'un FAD de type T
 * 
 * Recopie le FAD value passe en parametre. \n
 * Fait appel a la methode copy(). \n 
 * \param value FAD de type T
 */
inline void operator=(const Fad<T> & value) 
{
  copy(value) ;
}
//! @}

//! @name Accesseurs
//! @{
/*! \brief Retourne la taille du tableau.
 */
inline int getSize() const 
{
  return m_array.getSize() ;
}

/*! \brief Retourne la valeur du tableau a l'index i.
 * 
 * \param i Index de la valeur recherchee
 * \warning Ne verifie pas si i est valable pour le tableau
 */
inline value_type getValue(int i) const 
{
  return m_array.getValue(i) ;
}

/*! \brief Retourne la valeur de la derivee du tableau a l'index ligne,colonne.
 * 
 * \param ligne Premier index de la valeur recherchee
 * \param colonne Deuxieme index de la valeur recherchee
 * \warning Ne verifie pas si ligne et colonnes sont valables
 */
inline value_type getDerivedValue(int ligne, int colonne) const 
{
  if (m_pos!=-2 && m_pos!=-1)
  {
    return m_dPos[colonne] ;
  }
  else 
  {
    return 0 ;
  }
}

/*! \brief Retourne le nombre d'inconnues
 */
inline int getNbInconnues() const
{
  return m_nbInconnues ;
}
//! @}

//! @name Autres
//! @{
/*! \brief "Active" le calcul de la derivee
 * 
 * Active le calcul de la derivee en affectant une value autre que -2 a m_pos. \n
 * \param pos Position de la variable
 */
inline void evalDiff(int pos) 
{
  if (m_pos==-2 || m_pos==-1)
  {
    delete m_dPos ;
  }
  else
  {
    delete[] m_dPos ;
  }
  m_pos = pos ;
  m_dPos = new int[m_nbInconnues] ;
  for (int i=m_nbInconnues-1;i>=0;--i) 
  {
    m_dPos[i] = 0 ;
  }
  m_dPos[pos] = 1 ;
}

//! \brief Affichage du FAD
inline void affiche() const 
{
  m_array.affiche();
  if (m_pos==-2) 
  {
    ;
  }
  else 
  {
    for (int i=0;i<m_array.getSize();i++) 
      {
        for (int j=0;j<m_nbInconnues;j++) 
        {
          std::cout << m_dval[i][j] << " \t " ;
        }
      std::cout << std::endl ;
    }
  }
}

//! Fonction pour delete le m_dval
inline void deleteFad()
{
  if (m_pos==-1) 
  {
    for (int i=m_nLigne-1;i>=0;--i) 
    {
      delete[] m_dval[i] ;
    }
    delete[] m_dval ;
  }
  else 
  {
    delete m_dval[0] ;
    delete[] m_dval ; 
  }
}

//! \brief Copie du Fad value
inline void copy(const Fad<T> & value) 
{
  m_array = value.m_array ;
  m_nbInconnues = value.m_nbInconnues ;
  m_nLigne = value.getSize() ;
  if (m_own)
  {
    deleteFad() ;
  }
  m_own = 1 ;
  if (m_pos == -1 || m_pos == -2)
  {
    delete m_dPos ;
  }
  else
  {
    delete[] m_dPos ;
  }
  m_pos = value.m_pos ;
  if (m_pos == -1 || m_pos == -2)
  {
    m_dPos = new int(0) ;
  }
  else 
  {
    m_dPos = new int[m_nbInconnues] ;
    for (int i=m_nbInconnues-1;i>=0;--i) 
    {
      m_dPos[i] = value.m_dPos[i] ;
    }
  }
  if (m_pos != -1) 
  {
    m_dval = new value_type*[1] ;
    m_dval[0] = new value_type(0) ;
  }
  else 
  {
    m_dval = new value_type*[m_nLigne] ;
    for (int i=m_nLigne-1;i>=0;--i) 
    {
      m_dval[i] = new value_type[m_nbInconnues] ;
    }
    switch(m_nbInconnues) 
    {
      case 1 : 
      {
        register value_type tmp ; 
        for (int k=m_nLigne-1;k>=0;--k)
        {
          tmp = value.getDerivedValue(k,0) ;
          m_dval[k][0] = tmp ;
        }
        break ;
      }
      case 2 : 
      {
        register value_type tmp1 ;
        register value_type tmp2 ;
        for (int k=m_nLigne-1;k>=0;--k)
        {
          tmp1 = value.getDerivedValue(k,0) ;
          tmp2 = value.getDerivedValue(k,1) ;
          m_dval[k][0] = tmp1 ;
          m_dval[k][1] = tmp2 ;
        }
        break ;
      }
      case 3 :
      {
        register value_type tmp1 ;
        register value_type tmp2 ;
        register value_type tmp3 ;
        for (int k=m_nLigne-1;k>=0;--k)
        {
          tmp1 = value.getDerivedValue(k,0) ;
          tmp2 = value.getDerivedValue(k,1) ;
          tmp3 = value.getDerivedValue(k,2) ;
          m_dval[k][0] = tmp1 ;
          m_dval[k][1] = tmp2 ;
          m_dval[k][2] = tmp3 ;
        }
        break ;
      }
      default :
      {
        register value_type tmp ; 
        for (int k=m_nLigne-1;k>=0;--k)
        {
          for (int l=m_nbInconnues-1;l>=0;--l)
          {
            tmp = value.getDerivedValue(k,l) ;
            m_dval[k][l] = tmp ;
          }
        }
        break ;
      }
    }
  }
}
//! @}
};

#endif
