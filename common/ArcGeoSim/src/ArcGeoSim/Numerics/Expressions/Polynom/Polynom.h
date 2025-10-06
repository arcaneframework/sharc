#ifndef POLYNOM_H
#define POLYNOM_H

#include <iostream>
#include <arcane/ArcaneVersion.h>
#include "ArcGeoSim/Numerics/Expressions/FunctionVars.h"
#include "ArcGeoSim/Numerics/Expressions/IDFunctionR1vR1.h"

BEGIN_ARCGEOSIM_NAMESPACE BEGIN_NUMERICS_NAMESPACE
BEGIN_EXPRESSIONS_NAMESPACE

class BasePolynom
  {
protected:
  template<Integer n>
    Real
    compute(Real y, const Real& x);

  template<Integer n>
    DVar<Real>
    compute(DVar<Real> y, const Real& x);
  Real* m_coef;
  };
template<Integer N>
  class Polynom : public BasePolynom, public IDFunctionR1vR1
    {
  public:
    Polynom(Real* coef)
      {
        m_coef = new Real[N + 1];
        for (Integer i = 0; i < N + 1; ++i)
          m_coef[i] = coef[i];
      }

    void
    init()
      {
      }

    void
    setParameter(const String & name, const Real & parameter)
      {
        throw FatalErrorException(A_FUNCINFO, "not implemented");
      }

    void
    setParameter(Integer index, const Real & parameter)
      {
        throw FatalErrorException(A_FUNCINFO, "not implemented");
      }

    //! Getting number of parameter
    Integer
    nbParameter() const
      {
        return 0;
      }

    //! Getting name of parameter
    String
    parameterName(Integer index) const
      {
        return "Undefined";
      }

    //! Setting vectorized variable
    void
    setVariable(const String & name, const Array<Real> & variable)
    {
    	throw FatalErrorException(A_FUNCINFO, "not implemented");
    }

    void
    setVariable(Integer index, const Array<Real> & variable)
    {
    	throw FatalErrorException(A_FUNCINFO, "not implemented");
    }

    //! Getting name of variable
    String
    variableName(Integer index) const
    {
    	return "Undefined";
    }

    //! Setting evaluation vectorized result
    void
    setEvaluationResult(Array<Real> & result)
    {
    	throw FatalErrorException(A_FUNCINFO, "not implemented");
    }

    //! Setting derivation vectorized result
    /*! Derivation following @name di variable */
    void
    setDerivationResult(Integer di, Array<Real> & result)
    {
    	throw FatalErrorException(A_FUNCINFO, "not implemented");
    }

    //! Setting derivation vectorized result
    /*! Derivation following @name di variable */
    virtual void
    setDerivationResult(const String & di, Array<Real> & result)
    {
    	throw FatalErrorException(A_FUNCINFO, "not implemented");
    }

    void
    cleanup()
      {
        throw FatalErrorException(A_FUNCINFO, "not implemented");
      }

    void
    eval()
      {
        throw FatalErrorException(A_FUNCINFO, "not implemented");
      }
    //! Point-wise evaluation of P(x)
    //!@{
    inline Real
    eval(const Real& x);
    inline void
    eval(const Real & var0, Real & res0);
    //!@}

    //! Vector evaluationo f P(x)
    inline void
    eval(const Array<Real> & var0, Array<Real> & res0);

    //! Vector evaluation
    void
    eval(const ConstArrayView<Real> var0, ArrayView<Real> res0);

    //! Point-wise evaluation of P(x) and P'(x)
    inline DVar<Real>
    evalD(const Real& x);

    //! Vector evaluation of P(x) and P'(x)
    inline void
    evalD(const Array<Real> & var0, Array<DVar<Real> >& res0);

    //! Point-wise derivation temporary (derivation handling in IFunction not yet defined)
    inline Real
    diffEval(const Real& x)
      {
        throw FatalErrorException(A_FUNCINFO, "not implemented");
      }

    //! Coupled function and derivative scalar evaluation (temporary: derivation handling in IFunction still to be defined)
    void
    eval(const Real & var0, Real & res0, Real & diffres0)
      {
        throw FatalErrorException(A_FUNCINFO, "not implemented");
      }
    };

template<Integer n>
  Real
  BasePolynom::compute(Real y, const Real& x)
    {
      return compute<n - 1> (m_coef[n] + x * y, x);
    }

template<Integer n>
  DVar<Real>
  BasePolynom::compute(DVar<Real> y, const Real& x)
    {
      return compute<n - 1> (DVar<Real> (m_coef[n] + x * y.m_y, n * m_coef[n]
          + x * y.m_dy), x);
    }

template<>
  Real
  BasePolynom::compute<0>(Real y, const Real& x)
    {
      return m_coef[0] + x * y;
    }
template<>
  DVar<Real>
  BasePolynom::compute<0>(DVar<Real> y, const Real& x)
    {
      return DVar<Real> (m_coef[0] + x * y.m_y, y.m_dy);
    }

template<Integer N>
  Real
  Polynom<N>::eval(const Real& x)
    {
      return compute<N - 1> (m_coef[N], x);
    }
template<Integer N>
  void
  Polynom<N>::eval(const Real & var0, Real & res0)
    {
	res0 = compute<N - 1> (m_coef[N], var0);
    }

template<Integer N>
void
Polynom<N>::eval(const Array<Real> & var0, Array<Real> & res0)
{
	for (Integer i = 0; i < var0.size(); ++i)
		res0[i] = compute<N - 1> (m_coef[N], var0[i]);
}

template<Integer N>
  void
  Polynom<N>::eval(const ConstArrayView<Real> var0, ArrayView<Real> res0)
    {
      for (Integer i = 0; i < var0.size(); ++i)
        res0[i] = compute<N - 1> (m_coef[N], var0[i]);
    }
template<Integer N>
  DVar<Real>
  Polynom<N>::evalD(const Real& x)
    {
      return compute<N - 1> (DVar<Real> (m_coef[N], N * m_coef[N]), x);
    }

template<Integer N>
void
Polynom<N>::evalD(const Array<Real> & var0, Array<DVar<Real> >& res0)
{
	for (Integer i = 0; i < var0.size(); ++i)
		res0[i] = compute<N - 1> (DVar<Real> (m_coef[N], N * m_coef[N]),
				var0[i]);
}

class PolynomeTester
  {
public:
  class P4
    {
  public:
    Real
    eval(Real x)
      {
        return 1 + x + x * x + x * x * x + x * x * x * x;
      }
    };

  static Integer
  main(int argv, char** argc)
    {

      Real* coef = new Real[5];
      coef[0] = 1;
      coef[1] = 1;
      coef[2] = 1;
      coef[3] = 1;
      coef[4] = 1;
      Polynom<4> p(coef);
      P4 p4;
      int n = 10000000;
      for (int i = 0; i < n; ++i)
        {
          //p.eval(1./(n+1)) ;
          p4.eval(1. / (n + 1));
        }
      std::cout << "P=" << p.eval(2) << std::endl;
      std::cout << "P=" << p4.eval(2) << std::endl;
      return 0;
    }
  };

END_EXPRESSIONS_NAMESPACE END_NUMERICS_NAMESPACE
END_ARCGEOSIM_NAMESPACE

#endif
