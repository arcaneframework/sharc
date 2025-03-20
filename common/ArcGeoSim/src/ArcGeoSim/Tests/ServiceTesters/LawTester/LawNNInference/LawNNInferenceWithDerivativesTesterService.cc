// -*- C++ -*-
/* Author : gaynor
 *
 * Unitary test for Neural Network Law Evaluation using ONNX Model with derivatives
 *
 * This test consist in:
 *      - compute law through a neural network at onnx format
 *      - Check inferred this neural network creating with a law derivatives
  */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"

#include "NNInferenceMultiScalar_law.h"

#include "ArcGeoSim/Physics/Law2/VariableManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionEvaluator.h"

#include <arcane/random/Uniform01.h>
#include <arcane/random/LinearCongruential.h>

#include "LawNNInferenceWithDerivativesTester_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class LawNNInferenceWithDerivativesTesterService
  : public ArcaneLawNNInferenceWithDerivativesTesterObject
{
private:

  // this formula match the neural network was created
  // 2 inputs, 1 outputs with derivatives:
  static void _compute_onnx_expected_results(
      const float in1, const float in2,
      float& out1, float& dout1_din1, float& dout1_din2)
  {
    const float alpha1 = 1.32;
    const float alpha2 = 2.2;
    out1 = alpha1 * in1 + alpha2 * in2 + 6.4;
    dout1_din1 = alpha1;
    dout1_din2 = alpha2;
  }

public:
  
  LawNNInferenceWithDerivativesTesterService(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneLawNNInferenceWithDerivativesTesterObject(sbi) {}
  
  ~LawNNInferenceWithDerivativesTesterService() {}
  
private:

  bool _unitaryTestNNInference();

public:

  void init();

  Arcane::Integer test()
  {
    if(_unitaryTestNNInference()) return 0;
    return 1;
  }

};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
LawNNInferenceWithDerivativesTesterService::
init()
{

}

/*---------------------------------------------------------------------------*/

bool
LawNNInferenceWithDerivativesTesterService::
_unitaryTestNNInference()
{
  Law::ScalarRealProperty in1(0,"I1");
  Law::ScalarRealProperty in2(1,"I2");
  Law::ScalarRealProperty out1(2,"O1");

  Law::VariableManager vars(mesh());

  // add properties to variables manager
  vars << Law::variable<Arcane::Cell>(in1);
  vars << Law::variable<Arcane::Cell>(in2);
  vars << Law::variableWithDerivative<Arcane::Cell>(out1);

  auto accessor = vars.variables<Arcane::Cell>();

  auto vin1 = accessor.values(in1);
  auto vin2 = accessor.values(in2);
  
  // initialize input properties with random values
  Arcane::random::MinstdRand generator;
  Arcane::random::Uniform01<Arcane::random::MinstdRand> uniform(generator);
  ENUMERATE_CELL(icell, allCells())
  {
    vin1[icell] = uniform();
    vin2[icell] = uniform();
  }

  // neural neuron network inference law evaluation using onnx
  // available because config.xml used language="law[inference=onnx]"
  Law::FunctionManager funcs;
  NNInferenceMultiScalar::Signature s;
  s.input.add(in1);
  s.input.add(in2);
  s.output.add(out1);
  // passing onnx file instead of evaluation function
  std::string onnx_file_name("./model_linear_derivatives_2_1.onnx");
  auto f = std::make_shared<NNInferenceMultiScalar::Function>(s, onnx_file_name);
  funcs << f;

  // create evaluator
  Law::FunctionEvaluator e(funcs);

  // check evaluation without derivatives
  {
	  // evaluate laws
	  std::cout<<"evaluate law without derivatives"<<std::endl;
	  e.evaluate(accessor, allCells(), Law::eWithoutDerivative);

	  // get law output properties
	  auto vout1 = accessor.values(out1);

	  // check results
	  std::cout<<"check only values results"<<std::endl;
	  ENUMERATE_CELL(icell, allCells()) {
		  // compute reference values
		  float result1, dresult1_din1, dresult1_din2;
		  _compute_onnx_expected_results(
				  vin1[icell],vin2[icell],
				  result1, dresult1_din1,dresult1_din2);
		  // check results with reference values
		  const float eps = 1.e01*std::numeric_limits<float>::epsilon();
		  if(std::fabs(result1 - vout1[icell]) > eps) return true;
	  }
  }

  // check evaluation with derivatives
  {
	  // re-initialize input properties with random values
	  Arcane::random::MinstdRand generator;
	  Arcane::random::Uniform01<Arcane::random::MinstdRand> uniform(generator);
	  ENUMERATE_CELL(icell, allCells())
	  {
		  vin1[icell] = uniform();
		  vin2[icell] = uniform();
	  }
	  // evaluate laws
	  std::cout<<"evaluate law with derivatives"<<std::endl;
	  e.evaluate(accessor, allCells(), Law::eWithDerivative);

	  // get law output properties
	  auto vout1 = accessor.values(out1);
	  auto dvout1 = accessor.derivatives(out1);

	  // check results
	  std::cout<<"check values and derivatives results"<<std::endl;
	  ENUMERATE_CELL(icell, allCells()) {
		  // compute reference values
		  float result1, dresult1_din1, dresult1_din2;
		  _compute_onnx_expected_results(
				  vin1[icell],vin2[icell],
				  result1, dresult1_din1,dresult1_din2);
		  // check results with reference values
		  const float eps = 1.e01*std::numeric_limits<float>::epsilon();
		  if(std::fabs(result1 - vout1[icell]) > eps) return true;
		  if(std::fabs(dresult1_din1 - dvout1[icell][0]) > eps) return true;
		  if(std::fabs(dresult1_din2 - dvout1[icell][1]) > eps) return true;
	  }
  }

  return false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_LAWNNINFERENCEWITHDERIVATIVESTESTER(LawNNInferenceWithDerivativesTester,LawNNInferenceWithDerivativesTesterService);
