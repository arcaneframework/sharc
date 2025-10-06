#ifndef IFUNCTOR_H
#define IFUNCTOR_H

#include <utility>

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
/*! \brief Methode virtuelle pour evaluer une fonction et ses derivees
*/
  virtual void evalDiff(Diff<VarVect> & ) = 0 ;

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual void evalDiff(value_type & , value_type * , value_type ) = 0 ;

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual void evalDiff(value_type & , value_type *, value_type, value_type) = 0 ;

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
  virtual void evalDiff(value_type & , value_type *, value_type, value_type, value_type) = 0 ;

  virtual void initParameter(PointType minB, PointType maxB,PointType leftExtB, PointType rightExtB) {} ;
  
  virtual void setKeyVal(value_type * keyVal) {} ;
  
  virtual void setDefaultCte(value_type ) {} ;
  
  virtual void setDefaultExposant(value_type) {} ;
  
  virtual void setDefaultSgr(value_type) {} ;
  
  virtual void setDefaultSwr(value_type) {} ;
  
  virtual bool evalNorm()
  {
    return true ;
  }
};

class VarVect ;

template<typename Fct1, typename Fct2>
class comp : public IFunctor<VarVect>
{
  typedef double value_type ;
  IFunctor<VarVect> * m_F1 ;
  IFunctor<VarVect> * m_Se ;
  int m_size ;
  int m_nb_var ;
public :
  inline comp(int size, int nbVar) : m_size(size), m_nb_var(nbVar)
  {
    m_F1 = new Fct1(1,nbVar) ;
    m_Se = new Fct2(1,nbVar) ;
  }
  
  inline ~comp()
  {
    delete m_F1 ;
    delete m_Se ;
  }
  
  inline void eval(VarVect & v)
  {
    for (int i=m_size-1;i>=0;--i)
    {
      v.m_listVect[0][i] = m_F1->eval(m_Se->eval(v.m_listVect[1][i],v.m_listVect[2][i],v.m_listVect[3][i]) ) ;
    }
  }
  
  //! Methode virtuelle pour l'evaluation de fonction scalaire
    virtual value_type eval(value_type)
    {
      return 0 ;
    }

  //! Methode virtuelle pour l'evaluation de fonction scalaire
    virtual value_type eval(value_type, value_type)
    {
      return 0 ;
    }

  //! Methode virtuelle pour l'evaluation de fonction scalaire
    virtual value_type eval(value_type, value_type, value_type) 
    {
      return 0 ;
    }
  /*! \brief Methode virtuelle pour evaluer une fonction et ses derivees
  */
    virtual void evalDiff(Diff<VarVect> & dv) 
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
    virtual void evalDiff(value_type & valOut, value_type * dValOut, value_type valIn) 
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
    virtual void evalDiff(value_type & , value_type *, value_type, value_type) 
    {
      ;
    }

  //! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
    virtual void evalDiff(value_type & , value_type *, value_type, value_type, value_type) 
    {
      ;
    }
    
    virtual void initParameter(PointType minB, PointType maxB,PointType leftExtB, PointType rightExtB)
    {
      m_F1->initParameter(minB,maxB,leftExtB,rightExtB) ;
    }
    
    virtual void setKeyVal(value_type * keyVal)
    {
      m_F1->setKeyVal(keyVal) ;
    }

    virtual void setDefaultCte(value_type cte)
    {
      m_F1->setDefaultCte(cte) ;
    }
    
    virtual void setDefaultExposant(value_type exp)
    {
      m_F1->setDefaultExposant(exp) ;
    }
    
    virtual void setDefaultSgr(value_type sgr)
    {
      m_Se->setDefaultSgr(sgr) ;
    }
    
    virtual void setDefaultSwr(value_type swr)
    {
      m_Se->setDefaultSwr(swr) ;
    }
    
    virtual bool evalNorm()
    {
      return m_F1->evalNorm() ;
    }
};

#endif
