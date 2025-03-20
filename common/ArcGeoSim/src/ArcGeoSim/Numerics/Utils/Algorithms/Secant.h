#ifndef ARCGEOSIM_NUMERICS_UTILS_ALGORITHMS_SECANT_H
#define ARCGEOSIM_NUMERICS_UTILS_ALGORITHMS_SECANT_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcGeoSim {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
  PURPOSE : Approximation of the root of a function using the secant method

  INPUT: Function f, endpoint values a, b, tolerance TOL, maximum iterations NMAX

  CONDITIONS: a < b, either f(a) < 0 and f(b) > 0 or f(a) > 0 and f(b) < 0

  OUTPUT: value which differs from a root of f(x)=0 by less than TOL
*/
struct Secant
{
  enum eCode
  {
    Converged,
    MaxIteration,
    UnreachableRoot,
    Undefined
  };

  struct Info
  {
    Arcane::Real norm;
    Arcane::Real dx;
    Arcane::Integer iteration;
    bool converged;
    eCode code;
  };

  template<class F>
  static Arcane::Real solve(F& function,
			    Arcane::Real a,
			    Arcane::Real b,
			    Info& info,
			    Arcane::Integer max_iteration = 500,
			    Arcane::Real epsilon = 1.e-6)
  {
    if(a > b) std::swap(a,b);

    auto fa = function(a);
    auto fb = function(b);

    if(fa*fb > 0) {
      info = Info { -1., -1., -1, false, UnreachableRoot };
      return 0;
    }
    
    auto xkm1 = 0.;
    auto xk   = 0.;
    auto fkm1 = 0.;
    auto fk   = 0.;
    auto dx   = 0.;
    auto err  = 0.;

    //    if(Arcane::math::abs(fa)> Arcane::math::abs(fb)){
      xkm1 = a;
      xk   = b;
      fkm1 = fa;
      fk   = fb;
      //    }
      //    else{
      //      xkm1 = b;
      //      xk   = a;
      //      fkm1 = fb;
      //      fk   = fa;
      //    }
    
    for(auto i = 0 ; i < max_iteration; ++i) {
      if(fkm1 == fk){
	// The secant method does not converge. switch to the false position method 
	if(fkm1 == fa){
	  fkm1 = fb;
	  xkm1 = b;
	}
	else{
	  fkm1 = fa;
	  xkm1 = a;
	}
      }
      auto dx = (xk-xkm1)/(fk-fkm1)*fk;
      xkm1 = xk;
      fkm1 = fk;
      xk  -= dx;
      fk   = function(xk);
      
      err = Arcane::math::abs(dx);
      if(err < epsilon || fk == 0.) {
	info = Info {err, dx, i, true, Converged };
	return xk;
      }
    }
    info = Info { err, dx, max_iteration, false, MaxIteration };
    return xk;
  }

  template<class F>
  static std::tuple<Arcane::Real, Info> solve(F& function,
        Arcane::Real a,
        Arcane::Real b,
        Arcane::Integer max_iteration = 500,
        Arcane::Real epsilon = 1.e-6)
  {
    Info i;
    return std::tuple<Arcane::Real, Info> {
      solve(function, a, b, i, max_iteration, epsilon), i
    };
  }
};
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_NUMERICS_UTILS_ALGORITHMS_SECANT_H */
