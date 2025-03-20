#ifndef NEWTONSOLVER_H_
#define NEWTONSOLVER_H_
#define DEFNEWTONMAXITER 500
#define DEFNEWTONEPSILON 1e-6

class NewtonSolver
{
public:
  NewtonSolver()
  {
    m_max_iter = DEFNEWTONMAXITER ;
    m_eps = DEFNEWTONEPSILON ;
  }
  virtual ~NewtonSolver() {}
  void setParameter(Integer max_iter, Real precision)
  {
    m_max_iter = max_iter ;
    m_eps = precision ;
  }
  class Info
    {
    public :
      Info()
      {
        m_norme = -1 ;
        m_num_iter = -1 ;
        m_has_converged = false ;
        m_code = -1 ;
      }
      Real m_norme ;
      Integer m_num_iter ;
      bool m_has_converged ;
      Integer m_code ;
    };
  template<class F>
  Real solve(F& function,
             Real x_init, 
             Real x_min,
             Real x_max,
             Info* info)
  {
    F& f = function ;
    //check that x_init is in [x_min,x_max]
    Real x = math::max(x_min,math::min(x_init,x_max)) ;
    Real y = 0. ;
    Real dy = 0. ;
    for(Integer i=0;i<m_max_iter;i++)
    {
      f.eval(&y,&dy,x) ;
      Real dx = -y/dy ;
      Real new_x = x+dx ;
      if(new_x>x_max)
      {
        new_x = (x_max+x)/2 ;
        dx = new_x -x ;
      }
      else if(new_x<x_min)
      {
        new_x = (x_min+x)/2 ;
        dx = new_x -x ;
      }
      x = new_x ;
      if(math::abs(dx)<m_eps)
      {
        info->m_num_iter = i ;
        info->m_norme = math::abs(dx) ;
        info->m_has_converged = true ;
        info->m_code = 0 ;
        return x ;
      }
    }
    info->m_num_iter = m_max_iter ;
    info->m_norme = -1 ;
    info->m_has_converged = false ;
    info->m_code = 1 ;
    return x ;
  }
private :
  Integer m_max_iter ;
  Real m_eps ;
};

#endif /*NEWTONSOLVER_H_*/
