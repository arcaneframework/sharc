#ifndef ARCGEOSIM_NUMERICS_UTILS_ALGORITHMS_BISECTION_H
#define ARCGEOSIM_NUMERICS_UTILS_ALGORITHMS_BISECTION_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcGeoSim {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*

  INPUT: Function f, endpoint values a, b, tolerance TOL, maximum iterations NMAX
  CONDITIONS: a < b, either f(a) < 0 and f(b) > 0 or f(a) > 0 and f(b) < 0
  OUTPUT: value which differs from a root of f(x)=0 by less than TOL

  N = 1
  While N <= NMAX # limit iterations to prevent infinite loop
    c = (a + b)/2 # new midpoint
    If f(c) = 0 or (b - a)/2 < TOL then # solution found
      Output(c)
      Stop
    EndIf
    N = N + 1 # increment step counter
    If sign(f(c)) = sign(f(a)) then a = c else b = c # new interval
  EndWhile
  Output("Method failed.") # max number of steps exceeded

 */

struct Bisection
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

    if(function(a)*function(b) > 0) {
      info = Info { -1., -1., -1, false, UnreachableRoot };
      return 0;
    }

    auto c = 0., err = 0.;

    for(auto i = 0 ; i < max_iteration; ++i) {

      c = (a+b)/2;

      auto fc = function(c);

      err = Arcane::math::abs(fc);

      if(err < epsilon || (b-a)/2 < epsilon) {
        info = Info { err, b-a, i, true, Converged };
        return c;
      }

      auto fa = function(a);

      auto product = fa * fc;

      if (product < 0) {
        b = c;
      } else {
        a = c;
      }
    }
    info = Info { err, b-a, max_iteration, true, Converged };
    return c;
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

#endif /* ARCGEOSIM_NUMERICS_UTILS_ALGORITHMS_BISECTION_H */
