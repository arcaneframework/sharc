#ifndef HAT_H
#define HAT_H

/*!
 * \file Hat.h
 * \brief Evaluation conditionnelle de fonctions
 * \date 22/07/2008
 * \author Xavier TUNC
 */

/*!
 * \brief Hat : Evaluation conditionnelle de fonctions.
 * 
 * Cette classe permet d'evaluer une fonction et ses derivees, en fonction d'une valeur cle et de bornes. \n
 * Si la valeur cle est comprise entre les bornes, la fonction est evaluee normalement. \n
 * Sinon, une interpolation lineaire est etablit entre les bornes et les points d'extension, et sa valeur est evaluee. \n
 * Cette classe implemente une methode initParameter() permettant de specifier les bornes et les points d'extension. Il est necessaire d'appeler cette methode et d'initialiser les bornes avant d'evaluer la fonction. \n
 * La methode computeBounds() permet de calculer la pente entre les bornes et les points d'extension. \n
 * La methode setKeyVal() permet de specifier la valeur cle du programme. \n 
 * Les methodes setDefaultCte() et setDefaultExposant() permettent de specifier la constante et l'exposant de la fonction "normale". \n
 * La methode evalNorm() permet de savoir si l'evaluation a ete "normale" ou si l'on a evaluer la partie gauche ou droite de la courbe. Cette methode est necessaire pour assurer l'exactitude du calcul de la derivee dans le cas d'une composition de fonction .\n
 * Enfin, cette classe heritant de la classe IFunctor, elle implemente les differentes methodes d'evaluation d'une fonction et/ou de ses derivees. \n
 */

template<typename fctType>
class Hat : public Fadd::IFunctor<Fadd::VarVect>
{
public:
//! \brief Type de valeurs manipulees
  typedef double value_type ;  
private: 
//! \brief Taille de l'equation
  int m_size ;
//! \brief Nombre de variables dans l'equation
  int m_nb_var ;
//! \brief Pointeur sur la fonction "normale"
  fctType * m_func ;
//! \brief Pointeur sur la valeur cle
  value_type * m_key_val ;
//! \brief Borne inferieure (gauche)
  PointType m_left_bound ;
//! \brief Borne superieure (droite)
  PointType m_right_bound ;
//! \brief Point d'extension a gauche
  PointType m_left_ext_bound ;
//! \brief Point d'extension a droite
  PointType m_right_ext_bound ;
//! \brief Pente de la partie lineaire a gauche
  value_type m_left_slope ;
//! \brief Pente de la partie lineaire a droite
  value_type m_right_slope ;
//! \brief Booleen indiquant le type d'evaluation (normal ou pas)
  bool m_eval_norm ;
public:
//! @name Constructeur
//! @{
/*! \brief Construit un "Hat" de type fctType
 * 
 * Fait appel au constructeur de la fonction fctType avecpour taille 1 (evaluation scalaire) et nbVar. \n
 * m_key_val est initialise a 0. \n
 * \param size Taille de l'equation
 * \param nbVar Nombre de variables de l'equation
 * 
 * \warning Les bornes et les points d'extension ne sont pas initialises
 */
  inline Hat(int size, int nbVar) : m_size(size), m_nb_var(nbVar)
  {
    m_func = new fctType(1,nbVar) ;
    m_key_val = 0 ;
    m_eval_norm = true ;
  }
//! @}
//! @name Destructeur
//! @{
//! \brief Destructeur
  inline ~Hat()
  {
    delete m_func ;
  }
//! @}

//! @name Autres
//! @{
/*! \brief Calcul des pentes des droites entre les bornes et les points d'extension
 * 
 * Methode appelee automatiquement dans la methode initParameter(). \n
 * Necessite d'avoir initialiser les bornes et les points critiques.
 */
  inline void computeBounds()
  {
    m_left_slope  = (m_left_bound.second - m_left_ext_bound.second) / (m_left_bound.first - m_left_ext_bound.first);
    m_right_slope = (m_right_ext_bound.second - m_right_bound.second) / (m_right_ext_bound.first - m_right_bound.first);
  }
  
/*! \brief Initialisation des bornes et des points limites
 * 
 * Cette methode initialise les attributs m_left_bound, m_right_bound, m_left_ext_bound et m_right_ext_bound. \n
 * Ses parametres sont de type std::pair<double,double> \n
 * Fait appel a la methode computeBounds(). \n
 * 
 * \param minB Borne inferieure (gauche)
 * \param maxB Borne superieure (droite)
 * \param leftExt Point d'extension inferieur (gauche)
 * \param rightExt Point d'extension superieur (droite)
 */
  inline void initParameter(PointType minB, PointType maxB, PointType leftExt, PointType rightExt)
  {
    m_left_bound = minB ;
    m_right_bound = maxB ;
    m_left_ext_bound = leftExt ;
    m_right_ext_bound = rightExt ;
    computeBounds() ;
  }
/*! \brief Permet de setter la valeur cle
 * 
 * Permet de setter la valeur cle. \n
 * C'est cette valeur qui est testee par rapport aux bornes afin d'evaluer soir la partie normale, soit une des parties lineaire de la courbe. \n
 * 
 * \param keyVal Pointeur sur la valeur cle
 */
  inline void setKeyVal(value_type * keyVal)
  {
    m_key_val = keyVal ;
  }

//! \brief Methode pour evaluer une fonction (vectorielle)
  inline void eval(VarVect & var)
  {
    if(var.m_nbParam==4)
    {
      for (int i=var.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          m_eval_norm = false ;
          var.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          m_eval_norm = true ;
          var.m_listVect[0][i] = m_func->eval(var.m_listVect[1][i],var.m_listVect[2][i],var.m_listVect[3][i]) ;
        }
        else
        {
          m_eval_norm = false ;
          var.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
        }
      }
    }
    else if(var.m_nbParam==3)
    {
      for (int i=var.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          m_eval_norm = false ;
          var.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          m_eval_norm = true ;
          var.m_listVect[0][i] = m_func->eval(var.m_listVect[1][i],var.m_listVect[2][i]) ;
        }
        else
        {
          m_eval_norm = false ;
          var.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
        }
      }
    }
    else
    {
      for (int i=var.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          m_eval_norm = false ;
          var.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          m_eval_norm = true ;
          var.m_listVect[0][i] = m_func->eval(var.m_listVect[1][i]) ;
        }
        else
        {
          m_eval_norm = false ;
          var.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
        }
      }
    }
  }
  
//! Methode pour l'evaluation de fonction scalaire
  inline value_type eval(value_type val)
  {
    if (m_key_val[0]<m_left_bound.first)
    {
      m_eval_norm = false ;
      return m_left_ext_bound.second + (m_key_val[0] - m_left_ext_bound.first) * m_left_slope;
    }
    else if (m_key_val[0]<=m_right_bound.first)
    {
      m_eval_norm = true ;
      return (m_func->eval(val)) ;
    }
    else
    {
      m_eval_norm = false ;
      return m_right_bound.second + (m_left_ext_bound.first - m_key_val[0]) * m_right_slope;
    }
  }

//! Methode pour l'evaluation de fonction scalaire
  inline value_type eval(value_type val1, value_type val2)
  {
    if (m_key_val[0]<m_left_bound.first)
    {
      m_eval_norm = false ;
      return m_left_ext_bound.second + (m_key_val[0] - m_left_ext_bound.first) * m_left_slope;
    }
    else if (m_key_val[0]<=m_right_bound.first)
    {
      m_eval_norm = true ;
      return (m_func->eval(val1,val2)) ;
    }
    else
    {
      m_eval_norm = false ;
      return m_right_bound.second + (m_left_ext_bound.first - m_key_val[0]) * m_right_slope;
    }
  }

//! Methode pour l'evaluation de fonction scalaire
  inline value_type eval(value_type val1, value_type val2, value_type val3)
  {
    if (m_key_val[0]<m_left_bound.first)
    {
      m_eval_norm = false ;
      return m_left_ext_bound.second + (m_key_val[0] - m_left_ext_bound.first) * m_left_slope;
    }
    else if (m_key_val[0]<=m_right_bound.first)
    {
      m_eval_norm = true ;
      return (m_func->eval(val1,val2,val3)) ;
    }
    else
    {
      m_eval_norm = false ;
      return m_right_bound.second + (m_left_ext_bound.first - m_key_val[0]) * m_right_slope;
    }
  }
//! \brief Methode virtuelle pour evaluer une fonction et ses derivees (vectorielle)
  inline void evalDiff(Diff<VarVect> & dv)
  {
    if(dv.m_nbVar==3) 
    {
      for (int i=dv.m_varVect.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          m_eval_norm = false ;
          dv.m_varVect.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
          dv.m_derivedValue[i][0] = m_left_slope ;
          dv.m_derivedValue[i][1] = m_left_slope ;
          dv.m_derivedValue[i][2] = m_left_slope ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          m_eval_norm = true ;
          m_func->evalDiff(dv.m_varVect.m_listVect[0][i],dv.m_derivedValue[i], dv.m_varVect.m_listVect[1][i],dv.m_varVect.m_listVect[2][i],dv.m_varVect.m_listVect[3][i]) ;
        }
        else
        {
          m_eval_norm = false ;
          dv.m_varVect.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
          dv.m_derivedValue[i][0] = m_right_slope ;
          dv.m_derivedValue[i][1] = m_right_slope ;
          dv.m_derivedValue[i][2] = m_right_slope ;
        }
      }
    }
    else if(dv.m_nbVar==2) 
    {
      for (int i=dv.m_varVect.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          m_eval_norm = false ;
          dv.m_varVect.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
          dv.m_derivedValue[i][0] = m_left_slope ;
          dv.m_derivedValue[i][1] = m_left_slope ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          m_eval_norm = true ;
          m_func->evalDiff(dv.m_varVect.m_listVect[0][i],dv.m_derivedValue[i], dv.m_varVect.m_listVect[1][i],dv.m_varVect.m_listVect[2][i]) ;
        }
        else
        {
          m_eval_norm = false ;
          dv.m_varVect.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
          dv.m_derivedValue[i][0] = m_right_slope ;
          dv.m_derivedValue[i][1] = m_right_slope ;
        }
      }
    }
    else 
    {
      for (int i=dv.m_varVect.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          m_eval_norm = false ;
          dv.m_varVect.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
          dv.m_derivedValue[i][0] = m_left_slope ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          m_eval_norm = true ;
          m_func->evalDiff(dv.m_varVect.m_listVect[0][i],dv.m_derivedValue[i], dv.m_varVect.m_listVect[1][i],dv.m_varVect.m_listVect[2][i],dv.m_varVect.m_listVect[3][i]) ;
        }
        else
        {
          m_eval_norm = false ;
          dv.m_varVect.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
          dv.m_derivedValue[i][0] = m_right_slope ;
        }
      }
    }
  }

//! \brief Methode pour evaluer une fonction et ses derivees (scalaire)
  inline void evalDiff(value_type & valOut, value_type * dvalOut, value_type valIn)
  {
    if (m_key_val[0]<m_left_bound.first)
    {
      m_eval_norm = false ;
      valOut = m_left_ext_bound.second + (m_key_val[0] - m_left_ext_bound.first) * m_left_slope ;
      *dvalOut = m_left_slope ;
    }
    else if (m_key_val[0]<=m_right_bound.first)
    {
      m_eval_norm = true ;
      m_func->evalDiff(valOut,dvalOut,valIn) ;
    }
    else
    {
      m_eval_norm = false ;
      valOut = m_right_bound.second + (m_key_val[0] - m_right_bound.first) * m_right_slope ;
      *dvalOut = m_right_slope ;
    }
  }

//! \brief Methode pour evaluer une fonction et ses derivees (scalaire)
  inline void evalDiff(value_type & valOut, value_type *dvalOut, value_type valIn, value_type valIn1)
  {
    if (m_key_val[0]<m_left_bound.first)
    {
      m_eval_norm = false ;
      valOut = m_left_ext_bound.second + (m_key_val[0] - m_left_ext_bound.first) * m_left_slope ;
      *dvalOut = m_left_slope ;
    }
    else if (m_key_val[0]<=m_right_bound.first)
    {
      m_eval_norm = true ;
      m_func->evalDiff(valOut,dvalOut,valIn, valIn1) ;
    }
    else
    {
      m_eval_norm = false ;
      valOut = m_right_bound.second + (m_key_val[0] - m_right_bound.first) * m_right_slope ;
      *dvalOut = m_right_slope ; 
    }
  }

//! \brief Methode pour evaluer une fonction et ses derivees (scalaire)
  inline void evalDiff(value_type & valOut, value_type * dvalOut, value_type valIn, value_type valIn1, value_type valIn2)
  {
    if (m_key_val[0]<m_left_bound.first)
    {
      m_eval_norm = false ;
      valOut = m_left_ext_bound.second + (m_key_val[0] - m_left_ext_bound.first) * m_left_slope ;
      *dvalOut = m_left_slope ;
    }
    else if (m_key_val[0]<=m_right_bound.first)
    {
      m_eval_norm = true ;
      m_func->evalDiff(valOut,dvalOut,valIn, valIn1, valIn2) ;
    }
    else
    {
      m_eval_norm = false ;
      valOut = m_right_bound.second + (m_key_val[0] - m_right_bound.first) * m_right_slope ;
      *dvalOut = m_right_slope ; 
    }
  }
  
//! \brief Methode pour setter la constante de l'equation normale
  virtual void setDefaultCte(value_type cte)
  {
    m_func->setDefaultCte(cte) ;
  }

//! \brief Methode pour setter l'exposant de l'equation normale
  virtual void setDefaultExposant(value_type exp)
  {
    m_func->setDefaultExposant(exp) ;
  }

//! \brief Methode permettant de savoir quel type d'evaluation a ete effectue
  virtual bool evalNorm()
  {
    return m_eval_norm ;
  }
//! @}
};

#endif
