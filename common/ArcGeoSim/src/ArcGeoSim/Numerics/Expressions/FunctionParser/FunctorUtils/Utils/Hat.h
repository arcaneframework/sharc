#ifndef HAT_H
#define HAT_H

template<typename fctType>
class Hat : public IFunctor<VarVect>
{
private:
  typedef double value_type ;
  int m_size ;
  int m_nb_var ;
  fctType * m_func ;
  value_type * m_key_val ;
  PointType m_left_bound ;
  PointType m_right_bound ;
  PointType m_left_ext_bound ;
  PointType m_right_ext_bound ;
  value_type m_left_slope ;
  value_type m_right_slope ;
  bool m_eval_norm ;
public:

  inline Hat(int size, int nbVar) : m_size(size), m_nb_var(nbVar)
  {
    m_func = new fctType(1,nbVar) ;
    m_key_val = 0 ;
    m_eval_norm = true ;
  }
  
  ~Hat()
  {
    delete m_func ;
  }
  
  inline void computeBounds()
  {
    m_left_slope  = (m_left_bound.second - m_left_ext_bound.second) / (m_left_bound.first - m_left_ext_bound.first);
    m_right_slope = (m_right_ext_bound.second - m_right_bound.second) / (m_right_ext_bound.first - m_right_bound.first);
  }
  
  inline void initParameter(PointType minB, PointType maxB, PointType leftExt, PointType rightExt)
  {
    m_left_bound = minB ;
    m_right_bound = maxB ;
    m_left_ext_bound = leftExt ;
    m_right_ext_bound = rightExt ;
    computeBounds() ;
  }

  inline void setKeyVal(value_type* keyVal)
  {
    m_key_val = keyVal ;
  }
  
//  inferieur a left_bound
//  y  = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
//  dy = m_left_slope;

//  superieur a right bound
//  y  = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
//  dy = m_right_slope;
  
  inline void eval(VarVect & var)
  {
    if(var.m_nbParam==4)
    {
      for (int i=var.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          var.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
          m_eval_norm = false ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          var.m_listVect[0][i] = m_func->eval(var.m_listVect[1][i],var.m_listVect[2][i],var.m_listVect[3][i]) ;
          m_eval_norm = true ;
        }
        else
        {
          var.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
          m_eval_norm = false ;
        }
      }
    }
    else if(var.m_nbParam==3)
    {
      for (int i=var.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          var.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
          m_eval_norm = false ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
          var.m_listVect[0][i] = m_func->eval(var.m_listVect[1][i],var.m_listVect[2][i]) ;
          m_eval_norm = true ;
        }
        else
        {
          var.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
          m_eval_norm = false ;
        }
      }
    }
    else
    {
      for (int i=var.m_sizeVect[0]-1;i>=0;--i)
      {
        if (m_key_val[i]<m_left_bound.first)
        {
          var.m_listVect[0][i] = m_left_bound.second + (m_key_val[i] - m_left_bound.first) * m_left_slope ;
          m_eval_norm = false ;
        }
        else if (m_key_val[i]<=m_right_bound.first)
        {
            var.m_listVect[0][i] = m_func->eval(var.m_listVect[1][i]) ;
            m_eval_norm = true ;
        }
        else
        {
          var.m_listVect[0][i] = m_right_bound.second + (m_key_val[i] - m_right_bound.first) * m_right_slope ;
          m_eval_norm = false ;
        }
      }
    }
  }
  
//! Methode virtuelle pour l'evaluation de fonction scalaire
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

//! Methode virtuelle pour l'evaluation de fonction scalaire
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

//! Methode virtuelle pour l'evaluation de fonction scalaire
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
/*! \brief Methode virtuelle pour evaluer une fonction et ses derivees
*/
  
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

  // Pb : use m_key_val[0] or m_key_val[i] ???
//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
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
      valOut = m_right_bound.second + (m_left_ext_bound.first - m_key_val[0]) * m_right_slope ;
      *dvalOut = m_right_slope ; 
    }
  }

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
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
      valOut = m_right_bound.second + (m_left_ext_bound.first - m_key_val[0]) * m_right_slope ;
      *dvalOut = m_right_slope ; 
    }
  }

//! \brief Methode virtuelle pour evaluer une fonction et ses derivees scalaire
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
      valOut = m_right_bound.second + (m_left_ext_bound.first - m_key_val[0]) * m_right_slope ;
      *dvalOut = m_right_slope ; 
    }
  }
  
  virtual void setDefaultCte(value_type cte)
  {
    m_func->setDefaultCte(cte) ;
  }
  
  virtual void setDefaultExposant(value_type exp)
  {
    m_func->setDefaultExposant(exp) ;
  }
  
  virtual bool evalNorm()
  {
    return m_eval_norm ;
  }
};

#endif
