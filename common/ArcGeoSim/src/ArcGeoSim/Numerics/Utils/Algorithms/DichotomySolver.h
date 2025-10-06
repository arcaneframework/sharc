#ifndef DICHOTOMYSOLVER_H_
#define DICHOTOMYSOLVER_H_
#define DEFDICHOTOMYMAXITER 500
#define DEFDICHOTOMYEPSILON 1e-6

class DichotomySolver
{
public:
  DichotomySolver()
  {
    m_max_iter = DEFDICHOTOMYMAXITER ;
    m_eps = DEFDICHOTOMYEPSILON ;
    m_xeps = DEFDICHOTOMYEPSILON ;
  }
  virtual ~DichotomySolver() {}
  void setParameter(Integer max_iter, Real precision)
  {
    m_max_iter = max_iter ;
    m_eps = precision ;
  }
  void setPrecisionX(Real precision_x)
  {
    m_xeps = precision_x ;
  }
  Real getPrecision() { return m_eps ; }
  Integer getMaxIter() { return m_max_iter ; }
  typedef enum {
    OK,
    XMIN_REACHED,
    XMAX_REACHED,
    ITERMAX_REACHED,
    UNDEFINED,
  } ErrorCodeType ;
  
  class Info
  {
  public :
    Info()
    {
      m_norme = -1 ;
      m_dx = -1 ;
      m_num_iter = -1 ;
      m_has_converged = false ;
      m_code = DichotomySolver::UNDEFINED ;
    }
    Real m_norme ;
    Real m_dx ;
    Integer m_num_iter ;
    bool m_has_converged ;
    DichotomySolver::ErrorCodeType m_code ;
  };
  template<class F>
  Real solve(F& function,
             Real x_init, 
             Real x_min,
             Real x_max,
             Info* info)
  {
    //check that x_init is in [x_min,x_max]
    Real x = Arcane::math::max(x_min,Arcane::math::min(x_init,x_max)) ;
    Real y = function.eval(x) ;
    Real eps = Arcane::math::abs(y) ;
    if(eps<m_eps)
    {
      info->m_num_iter = 0 ;
      info->m_norme = eps ;
      info->m_dx = x_max-x_min ;
      info->m_has_converged = true ;
      info->m_code = DichotomySolver::OK ;
      return x ;
    }
    Real y_min = -1. ;
    Real y_max = 1. ;
    if(y<0)
    {
      y_min = y ;
      x_min = x ;
    }
    else
    {
      y_max = y ;
      x_max = x ;
    }
    x = (x_max+x_min)/2 ;
    for(Integer i=1;i<m_max_iter;i++)
    {
      Real y = function.eval(x) ;
      if(y<0)
      {
        y_min = y ;
        x_min = x ;
      }
      else
      {
        y_max = y ;
        x_max = x ;
      }
      x = (x_max+x_min)/2 ;
      eps = Arcane::math::abs(y) ;
      if(eps<m_eps)
      {
        info->m_num_iter = i ;
        info->m_norme = eps ;
        info->m_dx = x_max-x_min ;
        info->m_has_converged = true ;
        info->m_code = DichotomySolver::OK ;
        return x ;
      }
      if(x>x_max-m_xeps)
      {
        info->m_num_iter = i ;
        info->m_norme = eps ;
        info->m_dx = x_max-x_min ;
        info->m_has_converged = false ;
        info->m_code = XMAX_REACHED ;
        return x ;
      }
      if(x<x_min+m_xeps)
      {
        info->m_num_iter = i ;
        info->m_norme = eps ;
        info->m_dx = x_max-x_min ;
        info->m_has_converged = false ;
        info->m_code = DichotomySolver::XMIN_REACHED ;
        return x ;
      }
    }
     
    info->m_num_iter = m_max_iter ;
    info->m_norme = y ;
    info->m_dx = x_max-x_min ;
    info->m_has_converged = false ;
    info->m_code = ITERMAX_REACHED ;
    return x ;
  }
private :
  Integer m_max_iter ;
  Real m_eps ;
  Real m_xeps ;
};

#endif /*DICHOTOMIESOLVER_H_*/
