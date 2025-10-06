// -*- C++ -*-
/* Author : gaynor
 *
 * Unitary test for Neural Network Law Evaluation using ONNX Model
 *
 * This test consist in:
 *      - compute law throw a neural network at onnx format
 *      - Check inferred this neural network according the way that it created
 *      cf _compute_onnx_expected_results, analytical formula to compute weight and biais
 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"

#include "NNInferenceScalar_law.h"
#include "NNInferenceMultiScalar_law.h"

#include "ArcGeoSim/Physics/Law2/VariableManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionEvaluator.h"

#include <arcane/random/Uniform01.h>
#include <arcane/random/LinearCongruential.h>

struct NNSignatureTypes
{
  enum eSignatureType
  {
    OnlyScalar,
	OnlyMultiScalar,
	Mixed
  };
};

#include "LawNNInferenceTester_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class LawNNInferenceTesterService
  : public ArcaneLawNNInferenceTesterObject
{
private:

   // this formula match the neural network was created

  static float weight(const int couche, const int in_out, const int hidden_neural){
    return float((couche + 1)*(hidden_neural + 1))/(10 + in_out);
  }

  static float biais(const int couche, const int hidden_neural){
    return float((couche + 1))/(10 + hidden_neural);
  }

  // test_model_3_2_2.onnx
  // 3 inputs, 1 hidden layer of 2 neurons , 2 outputs
  static void _compute_onnx_expected_results(
      const float in1,
      const float in2,
      const float in3,
      float& out1,
      float& out2
      )
  {
    float neurone_cache_0 = weight(0,0,0) * in1 + weight(0,1,0) * in2 + weight(0,2,0) * in3 + biais(0,0);
    neurone_cache_0 = neurone_cache_0>0 ? neurone_cache_0:0; // fonction d'activation ReLu
    float neurone_cache_1 = weight(0,0,1) * in1 + weight(0,1,1) * in2 + weight(0,2,1) * in3 + biais(0,1);
    neurone_cache_1 = neurone_cache_1>0 ? neurone_cache_1:0; // fonction d'activation ReLu
    out1 = weight(1,0,0) * neurone_cache_0 + weight(1,1,0) * neurone_cache_1 + biais(1,0);
    out2 = weight(1,0,1) * neurone_cache_0 + weight(1,1,1) * neurone_cache_1 + biais(1,1);
  }

public:
  
  LawNNInferenceTesterService(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneLawNNInferenceTesterObject(sbi) {}
  
  ~LawNNInferenceTesterService() {}
  
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
LawNNInferenceTesterService::
init()
{

}

/*---------------------------------------------------------------------------*/

bool
LawNNInferenceTesterService::
_unitaryTestNNInference()
{
  Law::ScalarRealProperty in1(0,"I1");
  Law::ScalarRealProperty in2(1,"I2");
  Law::ScalarRealProperty in3(2,"I3");
  Law::ScalarRealProperty out1(4,"O1");
  Law::ScalarRealProperty out2(5,"O2");

  Law::VariableManager vars(mesh());

  // add properties to variables manager
  vars << Law::variable<Arcane::Cell>(in1);
  vars << Law::variable<Arcane::Cell>(in2);
  vars << Law::variable<Arcane::Cell>(in3);
  vars << Law::variable<Arcane::Cell>(out1);
  vars << Law::variable<Arcane::Cell>(out2);

  auto accessor = vars.variables<Arcane::Cell>();

  // initialize input properties with random values
  auto vin1 = accessor.values(in1);
  auto vin2 = accessor.values(in2);
  auto vin3 = accessor.values(in3);
  
  Arcane::random::MinstdRand generator;
  Arcane::random::Uniform01<Arcane::random::MinstdRand> uniform(generator);
  ENUMERATE_CELL(icell, allCells())
  {
    vin1[icell] = uniform();
    vin2[icell] = uniform();
    vin3[icell] = uniform();
  }

  // neural neuron network inference law evaluation using onnx
  // available because config.xml used language="law[inference=onnx]"
  Law::FunctionManager funcs;
  if(options()->withMultiScalar)
  {
  	  NNInferenceMultiScalar::Signature s;
  	  s.input.add(in1);
  	  s.input.add(in2);
  	  s.input.add(in3);
  	  s.output.add(out1);
  	  s.output.add(out2);
  	  // passing onnx file instead of evaluation function
  	  std::string onnx_file_name("./test_model_3_2_2.onnx");
  	  auto f = std::make_shared<NNInferenceMultiScalar::Function>(s, onnx_file_name);
  	  funcs << f;
  }
  else
  {
	  NNInferenceScalar::Signature s;
	  s.input_1_1 = in1;
	  s.input_1_2 = in2;
	  s.input_1_3 = in3;
	  s.output_1_1 = out1;
	  s.output_1_2 = out2;
	  // passing onnx file instead of evaluation function
	  std::string onnx_file_name("./test_model_3_2_2.onnx");
	  auto f = std::make_shared<NNInferenceScalar::Function>(s, onnx_file_name);
	  funcs << f;
  }
  // create evaluator
  Law::FunctionEvaluator e(funcs);
  // evaluate laws
  e.evaluate(accessor, allCells());

  // get law output properties
  auto vout1 = accessor.values(out1);
  auto vout2 = accessor.values(out2);

  // check results
  ENUMERATE_CELL(icell, allCells()) {
    // compute reference values
    float result_1, result_2;
    _compute_onnx_expected_results(
        vin1[icell],vin2[icell],vin3[icell],
        result_1,result_2);
    // check results with reference values
    const float eps = std::numeric_limits<float>::epsilon();
    if(std::fabs(result_1 - vout1[icell]) > eps) return true;
    if(std::fabs(result_2 - vout2[icell]) > eps) return true;
  }
  return false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_LAWNNINFERENCETESTER(LawNNInferenceTester,LawNNInferenceTesterService);
