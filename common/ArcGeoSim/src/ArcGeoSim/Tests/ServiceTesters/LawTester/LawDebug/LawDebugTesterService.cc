// -*- C++ -*-
/* Author : gaynor
 *
 * Unary test for Debug Law Evaluation
 *
 * Debug law evaluation consist in:
 *      - Compute derivatives by finites differences
 *      - output in trace manager:
 *              - classical derivatives values computed by the law
 *              - finite differences derivatives values
 *              - error between this two derivatives
 *      - Replace derivatives values computed by the law by finites differences ones
 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"

#include "LawDebugTester_axl.h"

#include "Debug1_law.h"
#include "Debug2_law.h"

#include "ArcGeoSim/Physics/Law2/VariableManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionEvaluator.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class LawDebugTesterService
  : public ArcaneLawDebugTesterObject
{
private:
  struct AlgoDebug1
  {
    void compute(
        Arcane::Real in1,
        Arcane::Real in2,
        Arcane::Real in3,
        Arcane::Real in4,
        Arcane::Real& out1,
        Arcane::Real& dout1_in1,
        Arcane::Real& dout1_in2,
        Arcane::Real& dout1_in3,
        Arcane::Real& dout1_in4,
        Arcane::Real& out2,
        Arcane::Real& dout2_in1,
        Arcane::Real& dout2_in2,
        Arcane::Real& dout2_in3,
        Arcane::Real& dout2_in4,
        Arcane::Real param1,
        Arcane::Real param2) const
    {
      out1 = param1*(in1+2*in2+3*in3+4*in4);
      dout1_in1 = param1;
      dout1_in2 = 2*param1;
      dout1_in3 = 3*param1;
      dout1_in4 = 4*param1;
      out2 = param2*(2*in1*in1+3*in2*in2+4*in3*in3+5*in4*in4);
      dout2_in1 = 4*param2*in1;
      dout2_in2 = 6*param2*in2;
      dout2_in3 = 8*param2*in3;
      dout2_in4 = 10*param2*in4;
    }
  };

  struct AlgoDebug2
  {
    void compute(
        Arcane::Real in1,
        Arcane::RealConstArrayView in2,
        Arcane::Real& out1,
        Arcane::Real& dout1_in1,
        Arcane::RealArrayView dout1_in2,
        Arcane::RealArrayView out2,
        Arcane::RealArrayView dout2_in1,
        Arcane::RealArray2View dout2_in2,
        Arcane::Real param1) const
    {
      auto sizei2 = in2.size();
      auto sizeo2 = out2.size();
      out1 = param1*in1;
      dout1_in1 = param1;
      for(auto k = 0; k < sizei2; ++k) {
        out1 += (k+1)*param1*in2[k];
        dout1_in2[k] = (k+1)*param1;
      }

      for(auto k = 0; k < sizeo2; ++k) {
        out2[k] = param1*in1;
        dout2_in1[k] = param1;
        for(auto i = 0; i < sizei2; ++i) {
           out2[k] += (i+1)*param1*in2[i];
           dout2_in2[k][i] = (i+1)*param1;
        }
      }
    }
  };


public:
  
  LawDebugTesterService(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneLawDebugTesterObject(sbi) {}
  
  ~LawDebugTesterService() {}
  
private:

  inline bool _checkDerivate(const Arcane::Real& deriv_ref, const Arcane::Real& deriv_debug)
  {
    // error between law derivatives and those computed by finites differences
    // must be good enough but not so...
    auto error = fabs((deriv_ref-deriv_debug)/(fabs(deriv_ref) + 1.e-20));
    if(error > 1.e-4)
      return false;
    if(error < 1.e-20)
      return false;
    return true;
  }

  bool _unitaryTestDebug1();
  bool _unitaryTestDebug2();

public:

  void init();

  Arcane::Integer test()
  {
    if(_unitaryTestDebug1()) return 0;
    if(_unitaryTestDebug2()) return 0;
    return 1;
  }

  AlgoDebug1 m_algo1;
  AlgoDebug2 m_algo2;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
LawDebugTesterService::
init()
{

}

/*---------------------------------------------------------------------------*/

bool
LawDebugTesterService::
_unitaryTestDebug1()
{
  Law::ScalarRealProperty in1(0,"I1");
  Law::ScalarRealProperty in2(1,"I2");
  Law::ScalarRealProperty in3(2,"I3");
  Law::ScalarRealProperty in4(3,"I4");
  Law::ScalarRealProperty out1(4,"O1");
  Law::ScalarRealProperty out2(5,"O2");
  Law::ScalarRealProperty pa1(6,"P1");
  Law::ScalarRealProperty pa2(7,"P2");
  // output properties for debug evaluation results
  Law::ScalarRealProperty out1_debug(8,"O1_DEBUG");
  Law::ScalarRealProperty out2_debug(9,"O2_DEBUG");

  Law::VariableManager vars(mesh());

  // add properties to variables manager
  vars << Law::variable<Arcane::Cell>(in1);
  vars << Law::variable<Arcane::Cell>(in2);
  vars << Law::variable<Arcane::Cell>(in3);
  vars << Law::variable<Arcane::Cell>(in4);
  vars << Law::variableWithDerivative<Arcane::Cell>(out1);
  vars << Law::variableWithDerivative<Arcane::Cell>(out2);
  vars << Law::variable<Arcane::Cell>(pa1);
  vars << Law::variable<Arcane::Cell>(pa2);
  vars << Law::variableWithDerivative<Arcane::Cell>(out1_debug);
  vars << Law::variableWithDerivative<Arcane::Cell>(out2_debug);

  auto accessor = vars.variables<Arcane::Cell>();

  // initialize input properties and parameter
  accessor.values(in1).fill(1.);
  accessor.values(in2).fill(2.);
  accessor.values(in3).fill(3.);
  accessor.values(in4).fill(4.);
  accessor.values(pa1).fill(5.);
  accessor.values(pa2).fill(6.);

  // classical law evaluation
  {
    Law::FunctionManager funcs;

    Debug1::Signature s;
    s.in_1 = in1;
    s.in_2 = in2;
    s.in_3 = in3;
    s.in_4 = in4;
    s.out_1 = out1;
    s.out_2 = out2;
    s.param_1 = pa1;
    s.param_2 = pa2;

    auto f = std::make_shared<Debug1::Function>(s, m_algo1, &AlgoDebug1::compute);

    funcs << f;

    Law::FunctionEvaluator e(funcs);

    e.evaluate(accessor, allCells(), Law::eWithDerivative);
  }

  // debug law evaluation
  // available because config.xml used language="law[debug]" for Debug1
  {
    Law::FunctionManager funcs;

    Debug1::Signature s;
    s.in_1 = in1;
    s.in_2 = in2;
    s.in_3 = in3;
    s.in_4 = in4;
    s.out_1 = out1_debug;
    s.out_2 = out2_debug;
    s.param_1 = pa1;
    s.param_2 = pa2;

    // epsilon on input are necessary to compute derivatives by finites differences
    Debug1::EpsilonDerivatives eps;
    eps.in_1 = 1.e-4;
    eps.in_2 = 1.e-4;
    eps.in_3 = 1.e-4;
    eps.in_4 = 1.e-4;

    /*  differences with classical evaluation:
     *    - Debug1::DebugFunction instead Debug1::Function
     *    - pass epsilon argument
     *    - pass traceMng() argument (to print debug information) */
    auto f = std::make_shared<Debug1::DebugFunction>(s, m_algo1, &AlgoDebug1::compute, eps, traceMng());

    funcs << f;

    Law::FunctionEvaluator e(funcs);

    e.evaluate(accessor, allCells(), Law::eWithDerivative);
  }

  // get law output properties
  auto vout1 = accessor.values(out1);
  auto dout1 = accessor.derivatives(out1);
  auto vout2 = accessor.values(out2);
  auto dout2 = accessor.derivatives(out2);
  auto vout1_debug = accessor.values(out1_debug);
  auto dout1_debug = accessor.derivatives(out1_debug);
  auto vout2_debug = accessor.values(out2_debug);
  auto dout2_debug = accessor.derivatives(out2_debug);

  // values between classical and debug evaluation must be exactly the same
  // derivatives between classical evaluation and debug (compute by finite differences) must be close
  ENUMERATE_CELL(icell, allCells()) {
    if(vout1[icell]!=vout1_debug[icell])
      return true;
    if(vout2[icell]!=vout2_debug[icell])
      return true;
    for(Arcane::Integer index_in=0;index_in<4;++index_in){
      if(!_checkDerivate(dout1[icell][index_in], dout1_debug[icell][index_in]))
        return true;
      if(!_checkDerivate(dout2[icell][index_in], dout2_debug[icell][index_in]))
        return true;
    }
  }
  return false;
}

/*---------------------------------------------------------------------------*/

bool
LawDebugTesterService::
_unitaryTestDebug2()
{
  Law::ScalarRealProperty in1(0,"I1");
  Law::ScalarRealProperty in2a(1,"I2a");
  Law::ScalarRealProperty in2b(2,"I2b");
  Law::ScalarRealProperty in2c(3,"I2c");
  Law::ScalarRealProperty out1(4,"O1");
  Law::ScalarRealProperty out2a(5,"O2a");
  Law::ScalarRealProperty out2b(6,"O2b");
  Law::ScalarRealProperty pa1(7,"P1");
  // output properties for debug evaluation results
  Law::ScalarRealProperty out1_debug(8,"O1_DEBUG");
  Law::ScalarRealProperty out2a_debug(9,"O2a_DEBUG");
  Law::ScalarRealProperty out2b_debug(10,"O2b_DEBUG");

  Law::VariableManager vars(mesh());

  // add properties to variables manager
  vars << Law::variable<Arcane::Cell>(in1);
  vars << Law::variable<Arcane::Cell>(in2a);
  vars << Law::variable<Arcane::Cell>(in2b);
  vars << Law::variable<Arcane::Cell>(in2c);
  vars << Law::variableWithDerivative<Arcane::Cell>(out1);
  vars << Law::variableWithDerivative<Arcane::Cell>(out2a);
  vars << Law::variableWithDerivative<Arcane::Cell>(out2b);
  vars << Law::variable<Arcane::Cell>(pa1);
  vars << Law::variableWithDerivative<Arcane::Cell>(out1_debug);
  vars << Law::variableWithDerivative<Arcane::Cell>(out2a_debug);
  vars << Law::variableWithDerivative<Arcane::Cell>(out2b_debug);

  auto accessor = vars.variables<Arcane::Cell>();

  // initialize input properties and parameter
  accessor.values(in1).fill(1.);
  accessor.values(in2a).fill(2.);
  accessor.values(in2b).fill(3.);
  accessor.values(in2c).fill(4.);
  accessor.values(pa1).fill(5.);

  // classical law evaluation
  {
    Law::FunctionManager funcs;

    Debug2::Signature s;
    s.in_1 = in1;
    s.in_2.add(in2a);
    s.in_2.add(in2b);
    s.in_2.add(in2c);
    s.out_1 = out1;
    s.out_2.add(out2a);
    s.out_2.add(out2b);
    s.param_1 = pa1;

    auto f = std::make_shared<Debug2::Function>(s, m_algo2, &AlgoDebug2::compute);

    funcs << f;

    Law::FunctionEvaluator e(funcs);

    e.evaluate(accessor, allCells(), Law::eWithDerivative);
  }

  // debug law evaluation
  // available because config.xml used language="law[debug]" for Debug2
  {
    Law::FunctionManager funcs;

    Debug2::Signature s;
    s.in_1 = in1;
    s.in_2.add(in2a);
    s.in_2.add(in2b);
    s.in_2.add(in2c);
    s.out_1 = out1_debug;
    s.out_2.add(out2a_debug);
    s.out_2.add(out2b_debug);
    s.param_1 = pa1;

    // epsilon on input are necessary to compute derivatives by finites differences
    Debug2::EpsilonDerivatives eps;
    eps.in_1 = 1.e-4;
    eps.in_2 = 1.e-4;


    /*  differences with classical evaluation:
     *    - Debug2::DebugFunction instead Debug2::Function
     *    - pass epsilon argument
     *    - pass traceMng() argument (to print debug information) */
    auto f = std::make_shared<Debug2::DebugFunction>(s, m_algo2, &AlgoDebug2::compute, eps, traceMng());

    funcs << f;

    Law::FunctionEvaluator e(funcs);

    e.evaluate(accessor, allCells(), Law::eWithDerivative);
  }

  // get law output properties
  auto vout1 = accessor.values(out1);
  auto dout1 = accessor.derivatives(out1);
  auto vout2a = accessor.values(out2a);
  auto dout2a = accessor.derivatives(out2a);
  auto vout2b = accessor.values(out2b);
  auto dout2b = accessor.derivatives(out2b);
  auto vout1_debug = accessor.values(out1_debug);
  auto dout1_debug = accessor.derivatives(out1_debug);
  auto vout2a_debug = accessor.values(out2a_debug);
  auto dout2a_debug = accessor.derivatives(out2a_debug);
  auto vout2b_debug = accessor.values(out2b_debug);
  auto dout2b_debug = accessor.derivatives(out2b_debug);

  // values between classical and debug evaluation must be exactly the same
  // derivatives between classical evaluation and debug (compute by finite differences) must be close
  ENUMERATE_CELL(icell, allCells()) {
    if(vout1[icell]!=vout1_debug[icell])
      return true;
    if(vout2a[icell]!=vout2a_debug[icell])
      return true;
    if(vout2b[icell]!=vout2b_debug[icell])
      return true;
    for(Arcane::Integer index_in=0;index_in<3;++index_in){
      if(!_checkDerivate(dout1[icell][index_in], dout1_debug[icell][index_in]))
        return true;
      if(!_checkDerivate(dout2a[icell][index_in], dout2a_debug[icell][index_in]))
        return true;
      if(!_checkDerivate(dout2b[icell][index_in], dout2b_debug[icell][index_in]))
        return true;
    }
  }
  return false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_LAWDEBUGTESTER(LawDebugTester,LawDebugTesterService);
