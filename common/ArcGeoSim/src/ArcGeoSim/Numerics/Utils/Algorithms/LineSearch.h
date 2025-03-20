#ifndef LINESEARCH_H_
#define LINESEARCH_H_

template<class Problem, class System, typename FuncType>
class LineSearchAlgo
{
public :
  LineSearchAlgo(Integer max_iter=100,
              Real beta=1e-4,
              Real alpha_min=0,
              Real alpha_max=1e100)
  : m_max_iter(max_iter)
  , m_beta(beta)
  , m_alpha_min(alpha_min)
  , m_alpha_max(alpha_max)
  , m_msg_debug(false)
  , m_opt_florian(true)
  {}

 public :
  bool lineSearch(Problem* problem,
                  typename System::vector_type& x,
                  typename System::vector_type& vk,
                  typename System::vector_type& rk,
                  typename System::vector_type& xt,
                  typename System::vector_type& xt1,
                  typename System::vector_type& rkt,
                  typename System::vector_type& rkt1,
                  FuncType norm2)
  {
    //cout<<"Start LineSeach"<<endl;
    Integer n = problem->size() ;
    Real lambda = 1.;
    bool ls_succes = false;
    
    for(int j=1; j<m_max_iter; ++j)
    {
      if (ls_succes==true) break;

      if (m_msg_debug) std::cout <<  "lambda =" << lambda << std::endl;
      xt = x + lambda * vk; // Set new possible point
      problem->evalf(xt,rkt); // Evaluate new possible point
      const Real tol = (1. - m_beta * lambda) * (*norm2)(rk,n) ;
      if (m_msg_debug) std::cout << "norm, tol " << (*norm2)(rkt,n) << " < " <<  tol << std::endl;
      if((*norm2)(rkt,n) < tol )
      {
        ls_succes = true; // Accept the step       
      }
      else
        lambda = lambda/2.; // Reject the step    
     }

     // Now let's have a look in the other direction
     Real lambda1 = 1.;
     bool ils_succes = false;
     if (m_msg_debug) std::cout <<  "lambda1 =" << lambda1 << std::endl;
     //xt1 = x + vk;
     //problem->evalf(xt1,rkt) ;
     Real prev_residual_norm = (*norm2)(rkt,n);
     for(int j=1 ; j<m_max_iter ; ++j)
     {
       if (m_opt_florian) 
	 { if ( ! ( ils_succes==false  && (*norm2)(rk,n) > 1.e5 ) ) break; }
       else
	 if ( ils_succes==true )  break;

        //cout<<"j="<<j<<endl;
       lambda1 = lambda1*1.2;
        if (m_msg_debug) std::cout <<  "lambda1 =" << lambda1 << std::endl;
	xt1 = x + lambda1 * vk; // Set new possible point
        problem->evalf(xt1,rkt); // Evaluate new possible point
        Real new_residual_norm = (*norm2)(rkt,n);
        if (m_msg_debug) std::cout << "norm, tol " << new_residual_norm << " > " <<  m_alpha_max << std::endl;
	if(new_residual_norm > m_alpha_max)
          break ;

        if (m_msg_debug) std::cout << "norm, tol " << new_residual_norm << " > " <<  prev_residual_norm << std::endl;
	if(new_residual_norm > prev_residual_norm)
        {
          ils_succes = true; // Inverse Linesearch don't work
	}
        else
          prev_residual_norm = new_residual_norm; // Inverse Linesearch found a better point
        
        
       }
    //cout<<"output j="<<j<<endl;

     lambda1 = lambda1/1.2;
     if (m_opt_florian) { if(lambda1<1.) lambda1=1.; }
     if (m_msg_debug) std::cout <<  "lambda1 =" << lambda1 << std::endl;
     xt1 = x + lambda1 * vk;
     problem->evalf(xt1,rkt1);
     problem->evalf(xt,rkt);

     // Increment actual point
     //      x = x + lambda * vk;
     if (m_msg_debug) std::cout << "norm, tol " << (*norm2)(rkt1,n) << " < " <<  (*norm2)(rkt,n) << std::endl;
     if((*norm2)(rkt1,n)<(*norm2)(rkt,n))
     {
       x = xt1 ;
       rk = rkt1 ;
     }
     else
     {
        x = xt ;
        rk = rkt ;
     };
     return ls_succes||ils_succes ;
   }

private :
  Integer m_max_iter ;
  Real m_beta ;
  Real m_alpha_min ;
  Real m_alpha_max ;

private : 
  bool m_msg_debug   ;
  bool m_opt_florian ; 

} ;
#endif /*LINESEARCH_H_*/
