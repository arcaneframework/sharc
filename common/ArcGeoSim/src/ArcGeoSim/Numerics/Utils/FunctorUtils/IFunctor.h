#ifndef IFUNCTOR_H
#define IFUNCTOR_H

#include <utility>
namespace Fadd
{
#include "Numerics/Utils/FunctorUtils/Type/VarVect.h"
}

/*!
 * \file IFunctor.h
 * \brief Functor pour manipuler les differentes equations
 * \date 23/04/2008
 * \author Xavier TUNC
*/

/*!
 * \brief IFunctor : Functor pour manipuler les differentes equations
 * 
 * Classe mere des classes fonctions. \n
 * Permet de manipuler les differentes equations. \n
 * Implemente les methodes virtuelles eval et evalDiff. \n
 * \warning class virtuelle pure. \n
*/

namespace Fadd
{

template <typename dvar>
class Diff ;

template <typename VarVect>
class IFunctor
{
public :
  typedef double value_type ;
  typedef std::pair<value_type,value_type> PointType ;
//! @name Constructeur
//! @{
//! \brief Constructeur
  IFunctor() 
  {
    ;
  }
//! @}
//! @name Destructeur
//! @{
//! \brief Destructeur
  virtual ~IFunctor() 
  {
    ;
  }
//! @}
//! @name Autres
//! @{
/*! \brief Methode virtuelle pour evaluer une fonction
*/
  virtual void eval(VarVect &) = 0 ;

//! Methode virtuelle pour l'evaluation de fonction scalaire
  virtual value_type eval(value_type) = 0 ;

//! Methode virtuelle pour l'evaluation de fonction scalaire
  virtual value_type eval(value_type, value_type) = 0 ;

//! Methode virtuelle pour l'evaluation de fonction scalaire
  virtual value_type eval(value_type, value_type, value_type) = 0 ;
//! \brief Methode virtuelle pour evaluer une fonction et ses derivees

  virtual void evalDiff(Diff<VarVect> & ) = 0 ;

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual void evalDiff(value_type & , value_type * , value_type ) = 0 ;

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual void evalDiff(value_type & , value_type *, value_type, value_type) = 0 ;

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual void evalDiff(value_type & , value_type *, value_type, value_type, value_type) = 0 ;

//! \brief Methode virtuelle pour setter les bornes et les points d'extension (pour la classe "Hat")
  virtual void initParameter(PointType minB, PointType maxB,PointType leftExtB, PointType rightExtB) {} ;

//! \brief Methode virtuelle pour setter la valeur cle (pour la classe "Hat")
  virtual void setKeyVal(value_type * keyVal) {} ;

//! \brief Methode virtuelle pour setter la constante de l'equation (Krw et Pc)
  virtual void setDefaultCte(value_type ) {} ;

//! \brief Methode virtuelle pour setter l'exposant de l'equation  (Krw et Pc)
  virtual void setDefaultExposant(value_type) {} ;

//! \brief Methode virtuelle pour setter la valeur de Sgr (SeSw et SeSg)
  virtual void setDefaultSgr(value_type) {} ;

//! \brief Methode virtuelle pour setter la valeur de Swr (SeSw et SeSg)
  virtual void setDefaultSwr(value_type) {} ;

//! \brief Methode virtuelle pour savoir le type de l'evaluation (normale ou sur les parties lineaires)
  virtual bool evalNorm()
  {
    return true ;
  }
};

/*!
 * \brief comp : Classe de composition de fonction
 * 
 * Cette classe herite de la classe IFunctor et implemente donc toutes les methodes d'evaluation de fonction ainsi que celle des derivees. \n
 * Cette classe permet de composer deux fonctions, principalement une fonction de saturation et une fonction de permeabilite ou de pression capilaire. \n
 * Les fonctions correspondent aux parametres de templates de la classe comp. \n
 * La fonction de saturation est la fonction de type Fct2, l'autre etant de type Fct1. \n
 * On peut ainsi utiliser une nouvelle fonction de saturation ou de permeabilite/pression capillaire facilement.
*/

class VarVect ;

template<typename Fct1, typename Fct2>
class comp : public IFunctor<VarVect>
{
public:
//! \brief Type de valeurs manipulee
  typedef double value_type ;
private:
//! \brief Pointeur sur la premiere fonction
  IFunctor<VarVect> * m_F1 ;
//! \brief Pointeur sur la deuxieme fonction
  IFunctor<VarVect> * m_Se ;
//! \brief Taille de l'equation
  int m_size ;
//!\brief Nombre de variables de l'equation
  int m_nb_var ;
public :
//! @name Constructeur
//! @{
/*! \brief Construit une composition de fonction
 * 
 * \param size Taille de l'equation
 * \param nbVar Nombre de variables de l'equation
 */
  inline comp(int size, int nbVar) : m_size(size), m_nb_var(nbVar)
  {
    m_F1 = new Fct1(1,nbVar) ;
    m_Se = new Fct2(1,nbVar) ;
  }
//! @}
//! @name Destructeur
//! @{
//! \brief Detruit les fonctions 
  inline ~comp()
  {
    delete m_F1 ;
    delete m_Se ;
  }
//! @}
  
//! @name Autres
//! @{
  
//! \brief Methode virtuelle pour evaluer une fonction (vectorielle)
  virtual inline void eval(VarVect & v)
  {
    for (int i=m_size-1;i>=0;--i)
    {
      v.m_listVect[0][i] = m_F1->eval(m_Se->eval(v.m_listVect[1][i],v.m_listVect[2][i],v.m_listVect[3][i]) ) ;
    }
  }
  
  //! Methode virtuelle pour l'evaluation de fonction (scalaire)
  virtual inline value_type eval(value_type)
  {
    return 0 ;
  }

  //! Methode virtuelle pour l'evaluation de fonction (scalaire)
  virtual inline value_type eval(value_type, value_type)
  {
    return 0 ;
  }

  //! Methode virtuelle pour l'evaluation de fonction (scalaire)
  virtual inline value_type eval(value_type, value_type, value_type) 
  {
    return 0 ;
  }

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees
  virtual inline void evalDiff(Diff<VarVect> & dv) 
  {
    register value_type dtmp ;
    if(dv.m_nbVar==3) 
    {
      for (int i=m_size-1;i>=0;--i)
      {
        m_Se->evalDiff(dv.m_varVect.m_listVect[0][i],dv.m_derivedValue[i], dv.m_varVect.m_listVect[1][i],dv.m_varVect.m_listVect[2][i],dv.m_varVect.m_listVect[3][i]) ;
        m_F1->evalDiff(dv.m_varVect.m_listVect[0][i], &dtmp, dv.m_varVect.m_listVect[0][i]) ;
        if (m_F1->evalNorm())
        {
          for(int j=dv.m_nbVar-1;j>=0;--j) 
          {
            dv.m_derivedValue[i][j] *= dtmp ;
          }
        }
        else
        {
          for(int j=dv.m_nbVar-1;j>=0;--j)
          {
            dv.m_derivedValue[i][j] = dtmp ;
          }
        }
      }
    }
    else if(dv.m_nbVar==2) 
    {
      for (int i=m_size-1;i>=0;--i)
      {
        m_Se->evalDiff(dv.m_varVect.m_listVect[0][i],dv.m_derivedValue[i], dv.m_varVect.m_listVect[1][i],dv.m_varVect.m_listVect[2][i]) ;
        m_F1->evalDiff(dv.m_varVect.m_listVect[0][i], &dtmp, dv.m_varVect.m_listVect[0][i]) ;
        if (m_F1->evalNorm())
        {
          for(int j=dv.m_nbVar-1;j>=0;--j) 
          {
            dv.m_derivedValue[i][j] *= dtmp ;
          }
        }
        else
        {
          for(int j=dv.m_nbVar-1;j>=0;--j)
          {
            dv.m_derivedValue[i][j] = dtmp ;
          }
        }
      }
    }
    else //if(dv.m_nbVar==1) tmp
    {
      for (int i=m_size-1;i>=0;--i)
      {
        m_Se->evalDiff(dv.m_varVect.m_listVect[0][i],dv.m_derivedValue[i], dv.m_varVect.m_listVect[1][i]) ;
        m_F1->evalDiff(dv.m_varVect.m_listVect[0][i], &dtmp, dv.m_varVect.m_listVect[0][i]) ;
        if (m_F1->evalNorm())
        {
          for(int j=dv.m_nbVar-1;j>=0;--j) 
          {
            dv.m_derivedValue[i][j] *= dtmp ;
          }
        }
        else
        {
          for(int j=dv.m_nbVar-1;j>=0;--j)
          {
            dv.m_derivedValue[i][j] = dtmp ;
          }
        }
      }
    }
  }

  //! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual inline void evalDiff(value_type & valOut, value_type * dValOut, value_type valIn) 
  {
    register value_type dtmp ;
    m_Se->evalDiff(valOut,dValOut,valIn) ;
    m_F1->evalDiff(valOut,&dtmp,valOut) ;
    if(m_F1->evalNorm())
    {
      *dValOut *= dtmp ;
    }
    else
    {
      *dValOut = dtmp ;
    }
  }

  //! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual inline void evalDiff(value_type & , value_type *, value_type, value_type) 
  {
    ;
  }

  //! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual inline void evalDiff(value_type & , value_type *, value_type, value_type, value_type) 
  {
    ;
  }

//! \brief Methode virtuelle pour setter les bornes et les points d'extension (classe Hat)
  virtual inline void initParameter(PointType minB, PointType maxB,PointType leftExtB, PointType rightExtB)
  {
    m_F1->initParameter(minB,maxB,leftExtB,rightExtB) ;
  }

//! \brief Methode virtuelle pour setter la valeur cle (classe Hat)
  virtual inline void setKeyVal(value_type * keyVal)
  {
    m_F1->setKeyVal(keyVal) ;
  }

//! \brief Methode virtuelle pour setter la constante (fonctions Krw et Pc)
  virtual inline void setDefaultCte(value_type cte)
  {
    m_F1->setDefaultCte(cte) ;
  }

//! \brief Methode virtuelle pour setter l'exposant (fonctions Krw et Pc)
  virtual inline void setDefaultExposant(value_type exp)
  {
    m_F1->setDefaultExposant(exp) ;
  }

//! \brief Methode virtuelle pour setter la valeur de sgr (fonctions SeSg et SeSw)
  virtual inline void setDefaultSgr(value_type sgr)
  {
    m_Se->setDefaultSgr(sgr) ;
  }

//! \brief Methode virtuelle pour setter la valeur de swr (fonctions SeSg et SeSw)
  virtual inline void setDefaultSwr(value_type swr)
  {
    m_Se->setDefaultSwr(swr) ;
  }

//! \brief Methode virtuelle pour recuperer le type d'evaluation effectuee (classe Hat)
  virtual inline bool evalNorm()
  {
    return m_F1->evalNorm() ;
  }
};

} // namespace Fadd
#endif
