// -*- C++ -*-
/* Author : gaynor
 *
 * Unitary test for ONNX load Neural Network and inferred it
 *
 * This test consist in:
 *      - Load neural network onnx format
 *      - Check input output and shape dimension
 *      - Check inferred this neural network according the way that it created
 *      cf _compute_onnx_expected_results, analytical formula to compute weight and biais
 */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"

#include "ONNXInferenceTester_axl.h"

#include <arcane/random/Uniform01.h>
#include <arcane/random/LinearCongruential.h>

#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ONNXInferenceTesterService
  : public ArcaneONNXInferenceTesterObject
{

public:
  
  ONNXInferenceTesterService(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneONNXInferenceTesterObject(sbi)
    , m_environment(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING) {}
  
  ~ONNXInferenceTesterService() {}
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

  // end this formula match the neural network was created

  // tests
  bool _unitaryTestSignature();
  bool _unitaryTestInference();

public:

  void init();

  Arcane::Integer test()
  {
    if(_unitaryTestSignature()) return 0;
    if(_unitaryTestInference()) return 0;
    return 1;
  }

private:
  Ort::Env m_environment;
  std::unique_ptr<Ort::Session> m_session;
};

/*---------------------------------------------------------------------------*/

void
ONNXInferenceTesterService::
init()
{
  // onnxruntime setup
  Ort::SessionOptions session_options;
  session_options.SetIntraOpNumThreads(1);
  session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
  // onnx session:
  std::string onnx_file_name("./test_model_3_2_2.onnx");
  m_session = std::make_unique<Ort::Session>(m_environment, onnx_file_name.c_str(), session_options);
  std::cout<<"ONNX Session OK"<<std::endl;
}

/*---------------------------------------------------------------------------*/

bool
ONNXInferenceTesterService::
_unitaryTestSignature()
{
  // input
  {
    // input size 1
    if(m_session->GetInputCount()!=1) return true;
    // input name: {input_1_1, input_1_2, input_1_3} law convention
    std::string  input_name = m_session->GetInputNameAllocated(0, Ort::AllocatorWithDefaultOptions()).get();
    if(input_name.compare("{input_1_1, input_1_2, input_1_3}") != 0) return true;
    // shape and type input
    Ort::TypeInfo input_type_info = m_session->GetInputTypeInfo(0);
    auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
    // tensor 3 axis
    if(input_tensor_info.GetDimensionsCount()!=3) return true;
    // -1 axis 1 batch size variable, 3 on axis 3: 3 inputs
    if(input_tensor_info.GetShape() != std::vector<int64_t>{ -1, 1, 3 } ) return true;
    // float precision model
    if(input_tensor_info.GetElementType()!=ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) return true;

  }
  std::cout<<"onnx input type and shape ok"<<std::endl;
  // output
  {
    // input size 1
    if(m_session->GetOutputCount()!=1) return true;
    // output name: output1
    std::string  output_name = m_session->GetOutputNameAllocated(0, Ort::AllocatorWithDefaultOptions()).get();
    if(output_name.compare("{output_1_1, output_1_2}") != 0) return true;
    // shape and type output
    Ort::TypeInfo output_type_info = m_session->GetOutputTypeInfo(0);
    auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
    if(output_tensor_info.GetDimensionsCount()!=3) return true;
    // float precision model
    if(output_tensor_info.GetElementType()!=ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT) return true;
    // 2 outputs
    if(output_tensor_info.GetShape() != std::vector<int64_t>{ -1, 1 , 2} ) return true;
  }
  std::cout<<"onnx output type and shape ok"<<std::endl;

  return false;
}

/*---------------------------------------------------------------------------*/

bool
ONNXInferenceTesterService::
_unitaryTestInference()
{
  Arcane::Integer local_data_size = 1000;
  Arcane::Integer input_tensor_size = 3 * local_data_size;

  std::vector<float> input_local_data(input_tensor_size);
  // random values generator for properties initialization
  Arcane::random::MinstdRand generator;
  Arcane::random::Uniform01<Arcane::random::MinstdRand> uniform(generator);
  for(auto& in_val : input_local_data)
    in_val = uniform();

  Arcane::Integer output_tensor_size = 2 * local_data_size;
  std::vector<float> output_local_data(output_tensor_size);

  // onnx tensors:
  Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

  // input:
  std::vector<Ort::Value> input_tensors;
  Ort::TypeInfo input_type_info = m_session->GetInputTypeInfo(0);
  auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();
  std::vector<int64_t> input_dims = input_tensor_info.GetShape();
  // batch size
  input_dims[0]=local_data_size;
  input_tensors.push_back(Ort::Value::CreateTensor<float>(memory_info, input_local_data.data(),
      input_tensor_size , input_dims.data(), input_dims.size()));

  // output:
  std::vector<Ort::Value> output_tensors;
  Ort::TypeInfo output_type_info = m_session->GetOutputTypeInfo(0);
  auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
  std::vector<int64_t> output_dims = output_tensor_info.GetShape();
  // batch size
  output_dims[0]=local_data_size;
  output_tensors.push_back(Ort::Value::CreateTensor<float>(memory_info, output_local_data.data(),
      output_tensor_size, output_dims.data(), output_dims.size()));

  // serving names:
  std::string input_name = m_session->GetInputNameAllocated(0, Ort::AllocatorWithDefaultOptions()).get();
  std::vector<const char*>  input_names{input_name.c_str()};
  std::string output_name = m_session->GetOutputNameAllocated(0, Ort::AllocatorWithDefaultOptions()).get();
  std::vector<const char*> output_names{output_name.c_str()};

  // model inference
  m_session->Run(Ort::RunOptions{nullptr}, input_names.data(),
                     input_tensors.data(), 1, output_names.data(),
                     output_tensors.data(), 1);

  // check results
  int index_in, index_out = 0;
  for(int i_sample=0; i_sample<local_data_size; i_sample++)
  {
    // compute reference values
    float result_1, result_2;
    _compute_onnx_expected_results(
        input_local_data[index_in],input_local_data[index_in+1],input_local_data[index_in+2],
        result_1,result_2);
    // check results with reference values
    const float eps = std::numeric_limits<float>::epsilon();
    if(std::fabs(result_1 - output_local_data[index_out]) > eps) return true;
    if(std::fabs(result_2 - output_local_data[index_out+1])> eps) return true;
    index_in+=3;
    index_out+=2;
  }

  std::cout<<"onnx inference ok"<<std::endl;

  return false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_ONNXINFERENCETESTER(ONNXInferenceTester,ONNXInferenceTesterService);
