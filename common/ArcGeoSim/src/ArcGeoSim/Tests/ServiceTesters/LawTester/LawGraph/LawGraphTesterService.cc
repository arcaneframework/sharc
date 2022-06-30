// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "LawGraphTesterService.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

// initialize the full graph and print it in different format
void
LawGraphTesterService::
init()
{
  // properties creation (full scalar)
  Integer count = 0;
  String prop_name = "Property_";
  for(int i =0;i<m_num_of_properties;++i){
    String name = prop_name+i;
    m_properties[i] = Law::ScalarRealProperty(count,name);
    count++;
  }
  // parameters creation
  prop_name = "RealParam_";
  for(int i =0;i<m_num_param;++i){
    String name = prop_name+i;
    m_params[i] = Law::ScalarRealProperty(count,name);
    count++;
  }
  // graph creation
  for(Integer i_funct=0; i_funct<m_num_of_function; i_funct++){
    // add function in graph
    _addFunction(m_functions_def[m_function_insertion_order[i_funct]]);
  }
  // parameters initialization
  for(Integer i =0;i<m_num_param;++i)
    m_param_ref[i]=i+1;
  // add properties and parameters to the variable manager
  for(Integer i =0;i<m_num_of_properties;++i)
    m_variable_mng << Law::variableWithDerivative<Arcane::Cell>(m_properties[i]);
  for(Integer i =0;i<m_num_param;++i)
    m_variable_mng << Law::variable<Arcane::Cell>(m_params[i]);
  //
  // print graph
  //
  // print variables in the console
  info() << m_variable_mng;
  // print function graph in the console
  info() << m_function_mng;
  // print the function graph in dot format file

  const String output_path = ArcGeoSim::getOuputRootPath();
  String output_file_name = output_path + "/function_memory_graph.dot";
  std::ofstream file(output_file_name.localstr());
  m_function_mng.printDotGraph(file,false);
  // print the function graph in dot format file with much more details
  String output_file_full_name = output_path + "/function_graph.dot";
  std::ofstream file_full(output_file_full_name.localstr());
  m_function_mng.printDotGraph(file_full);
}

/*---------------------------------------------------------------------------*/

// add function in graph
void
LawGraphTesterService::
_addFunction(FuctionDefininition& function_def) {
  switch(function_def.m_functions_kind){
    case Two_In_One_Out:
    {
      // create a function with two inputs and one output specialized by parameters
      TwoInOneOutAlgo object;
      TwoInOneOut::Signature signature;
      signature.in_1 = m_properties[function_def.m_in_property_id[0]];
      signature.in_2 = m_properties[function_def.m_in_property_id[1]];
      signature.out_1 = m_properties[function_def.m_out_property_id[0]];
      signature.param_1 = m_params[function_def.m_param_prop_id];
      // add function to the graph of function
      auto f = std::make_shared<TwoInOneOut::Function>(signature, object, &TwoInOneOutAlgo::compute);
      m_function_mng <<  f;
      m_function_mng.registerOrReplace(f);
    }
    break;
    case Three_In_Two_Out:
    {
      // create a function with three inputs and two outputs specialized by parameters
      ThreeInTwoOutAlgo object;
      ThreeInTwoOut::Signature signature;
      signature.in_1 =  m_properties[function_def.m_in_property_id[0]];
      signature.in_2 =  m_properties[function_def.m_in_property_id[1]];
      signature.in_3 =  m_properties[function_def.m_in_property_id[2]];
      signature.out_1 =  m_properties[function_def.m_out_property_id[0]];
      signature.out_2 =  m_properties[function_def.m_out_property_id[1]];
      signature.param_1 = m_params[function_def.m_param_prop_id];
      // add function to the graph of function
      auto f = std::make_shared<ThreeInTwoOut::Function>(signature, object, &ThreeInTwoOutAlgo::compute);
      m_function_mng << f;
      m_function_mng.registerOrReplace(f);
    }
    break;
    //add fault with msg
  }

}

/*---------------------------------------------------------------------------*/

// validate all the tests
Integer
LawGraphTesterService::
test()
{
  info() << "Test...";
  if(_test_1() == 0) { info() << "test 1 failed...";  return 0; }
  if(_test_2() == 0) { info() << "test 2 failed...";  return 0; }
  if(_test_3() == 0) { info() << "test 3 failed...";  return 0; }
  return 1;
}

/*---------------------------------------------------------------------------*/

// first test validate the full graph evaluation
Integer
LawGraphTesterService::
_test_1() const
{
  info() << "Evaluation complete graph";
  Law::VariableAccessorT<Cell> accessor = m_variable_mng.variables<Cell>();
  _graphAffectation(m_root_begin_test1,m_root_end_test1,m_leaf_begin_test1);
  Law::FunctionEvaluator(m_function_mng).evaluate(accessor, allCells());
  return _checkResults(m_root_begin_test1,m_root_end_test1,m_leaf_begin_test1,m_leaf_end_test1);
}

/*---------------------------------------------------------------------------*/

// second test validate a partial graph evaluation
Integer
LawGraphTesterService::
_test_2() const
{
  info() << "Evaluation partial graph cut in and out";
  Law::VariableAccessorT<Cell> accessor = m_variable_mng.variables<Cell>();
  _graphAffectation(m_root_begin_test2,m_root_end_test2,m_leaf_begin_test2);
  Integer nb_root = m_root_end_test2-m_root_begin_test2;
  Integer* root_prop_id = new Integer[nb_root];
  for(Integer i=0;i<=nb_root;++i)
    root_prop_id[i]=i+m_root_begin_test2;
  IntegerArrayView in(nb_root, root_prop_id);
  Integer nb_leaf = m_leaf_end_test2-m_leaf_begin_test2;
  Integer* leaf_prop_id = new Integer[nb_leaf];
  for(Integer i=0;i<=nb_leaf;++i)
    leaf_prop_id[i]=i+m_leaf_begin_test2;
  IntegerArrayView out(nb_leaf, leaf_prop_id);
  Law::FunctionEvaluator(m_function_mng,out).evaluate(accessor, allCells());
  return _checkResults(m_root_begin_test2,m_root_end_test2,m_leaf_begin_test2,m_leaf_end_test2);
}

/*---------------------------------------------------------------------------*/

// third test validate a graph evaluation with derivative evaluation and correction for composition
Integer
LawGraphTesterService::
_test_3() const
{
  info() << "Evaluation graph with derivatives";
  Law::VariableAccessorT<Cell> accessor = m_variable_mng.variables<Cell>();
  _graphAffectation(m_root_begin_test3,m_root_end_test3,m_leaf_begin_test3);
  Law::FunctionEvaluator(m_function_mng).evaluateWithDerivatives(accessor, allCells());
  return _checkResultsWithDerivatives();
}

/*---------------------------------------------------------------------------*/

// affectation of "random" sub graph
void
LawGraphTesterService::
_graphAffectation(Integer root_begin, Integer root_end, Integer leaf_begin) const
{
  Law::VariableAccessorT<Cell> accessor = m_variable_mng.variables<Cell>();
  for(Integer i =root_begin;i<=root_end;++i){
    VariableCellReal prop_i = accessor.values(m_properties[i]);
    ENUMERATE_CELL(icell, allCells())
    prop_i[icell]=m_uniform();
  }
  for(Integer i =0;i<m_num_param;++i){
    VariableCellReal param_i = accessor.values(m_params[i]);
    ENUMERATE_CELL(icell, allCells())
    param_i[icell]=m_param_ref[i];
  }
  for(Integer i=leaf_begin;i<m_num_of_properties;++i){
    VariableCellReal prop_i = accessor.values(m_properties[i]);
    ENUMERATE_CELL(icell, allCells())
    prop_i[icell]=0.;
  }
}

/*---------------------------------------------------------------------------*/

// check if complete or partial graph evaluation is correct
Integer
LawGraphTesterService::
_checkResults(Integer root_begin, Integer root_end, Integer leaf_begin, Integer leaf_end) const
{
  Law::VariableAccessorT<Cell> accessor = m_variable_mng.variables<Cell>();
  //
  TwoInOneOutAlgo algo_2_1;
  ThreeInTwoOutAlgo algo_3_2;
  UniqueArray<Real> in_out_ref(m_num_of_properties);
  UniqueArray<Real> trash_deriv(6);
  ENUMERATE_CELL(icell, allCells()) {
    for(Integer i =root_begin;i<=root_end;++i){
      VariableCellReal root_i = accessor.values(m_properties[i]);
      in_out_ref[i]=root_i[icell];
    }
    // reference computation
    for(Integer i_funct = 0;i_funct<m_num_of_function;++i_funct){
      FuctionDefininition current_function = m_functions_def[i_funct];
      if(current_function.m_functions_kind==Two_In_One_Out){
        const Real in_1 = in_out_ref[current_function.m_in_property_id[0]];
        const Real in_2 = in_out_ref[current_function.m_in_property_id[1]];
        Real& out_1 = in_out_ref[current_function.m_out_property_id[0]];
        const Real param_1 = m_param_ref[current_function.m_param_prop_id];
        algo_2_1.compute(in_1, in_2, out_1, trash_deriv[0], trash_deriv[1], param_1);
      }
      else if(current_function.m_functions_kind==Three_In_Two_Out){
        const Real in_1 = in_out_ref[current_function.m_in_property_id[0]];
        const Real in_2 = in_out_ref[current_function.m_in_property_id[1]];
        const Real in_3 = in_out_ref[current_function.m_in_property_id[2]];
        Real& out_1 = in_out_ref[current_function.m_out_property_id[0]];
        Real& out_2 = in_out_ref[current_function.m_out_property_id[1]];
        const Real param_1 = m_param_ref[current_function.m_param_prop_id];
        algo_3_2.compute(in_1, in_2, in_3,
            out_1, trash_deriv[0], trash_deriv[1], trash_deriv[2],
            out_2, trash_deriv[3], trash_deriv[4], trash_deriv[5], param_1);
      }
    }
    // test graph value evaluation
    for(Integer i = leaf_begin;i<=leaf_end;++i){
      VariableCellReal leaf_i = accessor.values(m_properties[i]);
      if(leaf_i[icell] != in_out_ref[i])
        return 0;
    }
    // partial graph evaluation
    for(Integer i =leaf_end+1;i<m_num_of_properties;++i){
      VariableCellReal leaf_i = accessor.values(m_properties[i]);
      if(leaf_i[icell]!=0)
        return 0;
    }
  }
  return 1;
}

/*---------------------------------------------------------------------------*/

// check if partial graph derivative evaluation is correct (including correct correction)
Integer
LawGraphTesterService::
_checkResultsWithDerivatives() const
{
  Law::VariableAccessorT<Cell> accessor = m_variable_mng.variables<Cell>();
  // level 0 properties 0 1 2
  for(Integer i=0;i<3;++i){
    VariableCellArrayReal root_i = accessor.derivatives(m_properties[i]);
    ENUMERATE_CELL(icell, allCells()) {
      if(root_i[icell].size()!=0)
        return 0;
    }
  }
  // To initialize reference root properties
  UniqueArray<Real> in_out_ref(m_num_of_properties);
  VariableCellReal root_property_0 = accessor.values(m_properties[0]);
  VariableCellReal root_property_1 = accessor.values(m_properties[1]);
  VariableCellReal root_property_2 = accessor.values(m_properties[2]);
  // level 1
  VariableCellArrayReal deriv_property_3 = accessor.derivatives(m_properties[3]);
  VariableCellArrayReal deriv_property_4 = accessor.derivatives(m_properties[4]);
  VariableCellArrayReal deriv_property_5 = accessor.derivatives(m_properties[5]);
  VariableCellArrayReal deriv_property_6 = accessor.derivatives(m_properties[6]);
  // level 2 composed
  VariableCellArrayReal deriv_property_7 = accessor.derivatives(m_properties[7]);
  VariableCellArrayReal deriv_property_8 = accessor.derivatives(m_properties[8]);
  VariableCellArrayReal deriv_property_9 = accessor.derivatives(m_properties[9]);
  // level 3 composed
  VariableCellArrayReal deriv_property_10 = accessor.derivatives(m_properties[10]);
  VariableCellArrayReal deriv_property_11 = accessor.derivatives(m_properties[11]);
  ENUMERATE_CELL(icell, allCells()) {
    //
    // reference root properties initialization
    in_out_ref[0]=root_property_0[icell];
    in_out_ref[1]=root_property_1[icell];
    in_out_ref[2]=root_property_2[icell];
    //
    // law 3:
    Real deriv_P3_P0, deriv_P3_P1;
    _computeLawDerivativesRef(0,in_out_ref,deriv_P3_P0,deriv_P3_P1);
    if(deriv_property_3[icell][0]!=deriv_P3_P0)
      return 0;
    if(deriv_property_3[icell][1]!=deriv_P3_P1)
      return 0;
    //
    // law 4:
    // ordre inverse cf P0 P1 order in law 4
    //
    Real deriv_P4_P1, deriv_P4_P0;
    //
    _computeLawDerivativesRef(1,in_out_ref,deriv_P4_P1,deriv_P4_P0);
    //
    if(deriv_property_4[icell][0]!=deriv_P4_P1)
      return 0;
    if(deriv_property_4[icell][1]!=deriv_P4_P0)
      return 0;
    //
    // law 5:
    //
    Real deriv_P5_P0, deriv_P5_P1, deriv_P5_P2, deriv_P6_P0, deriv_P6_P1, deriv_P6_P2;
    //
    _computeLawDerivativesRef(2,in_out_ref,deriv_P5_P0,deriv_P5_P1,deriv_P5_P2,deriv_P6_P0,deriv_P6_P1,deriv_P6_P2);
    //
    if(deriv_property_5[icell][0]!=deriv_P5_P0)
      return 0;
    if(deriv_property_5[icell][1]!=deriv_P5_P1)
      return 0;
    if(deriv_property_5[icell][2]!=deriv_P5_P2)
      return 0;
    if(deriv_property_6[icell][0]!=deriv_P6_P0)
      return 0;
    if(deriv_property_6[icell][1]!=deriv_P6_P1)
      return 0;
    if(deriv_property_6[icell][2]!=deriv_P6_P2)
      return 0;
    //
    // Composed Laws
    //
    // Composed law 7
    // depend de P0 P1
    if(deriv_property_7[icell].size()!=2)
      return 0;
    //
    Real deriv_P7_P3, deriv_P7_P4;
    //
    _computeLawDerivativesRef(3,in_out_ref,deriv_P7_P3,deriv_P7_P4);
    // correction
    const Real deriv_P7_P0 = deriv_P7_P3*deriv_P3_P0+deriv_P7_P4*deriv_P4_P0;
    if(deriv_property_7[icell][0]!=deriv_P7_P0)
      return 0;
    const Real deriv_P7_P1 = deriv_P7_P3*deriv_P3_P1+deriv_P7_P4*deriv_P4_P1;
    if(deriv_property_7[icell][1]!=deriv_P7_P1)
      return 0;
    //
    // Hybrid law
    //
    // Hybrid law 7: Param = 1
    // dependent de P0 P1 P2
    if(deriv_property_8[icell].size()!=3)
      return 0;
    if(deriv_property_9[icell].size()!=3)
      return 0;
    // evaluate
    Real deriv_P8_P5, deriv_P8_P6, direct_deriv_P8_P2, deriv_P9_P5, deriv_P9_P6, direct_deriv_P9_P2;
    //
    _computeLawDerivativesRef(4,in_out_ref,deriv_P8_P5,deriv_P8_P6,direct_deriv_P8_P2,deriv_P9_P5,deriv_P9_P6,direct_deriv_P9_P2);
    // correction
    const Real deriv_P8_P0=deriv_P8_P5*deriv_P5_P0+deriv_P8_P6*deriv_P6_P0;
    const Real deriv_P9_P0=deriv_P9_P5*deriv_P5_P0+deriv_P9_P6*deriv_P6_P0;
    if(deriv_property_8[icell][0]!=deriv_P8_P0)
      return 0;
    if(deriv_property_9[icell][0]!=deriv_P9_P0)
      return 0;
    const Real deriv_P8_P1=deriv_P8_P5*deriv_P5_P1+deriv_P8_P6*deriv_P6_P1;
    const Real deriv_P9_P1=deriv_P9_P5*deriv_P5_P1+deriv_P9_P6*deriv_P6_P1;
    if(deriv_property_8[icell][1]!=deriv_P8_P1)
      return 0;
    if(deriv_property_9[icell][1]!=deriv_P9_P1)
      return 0;
    const Real deriv_P8_P2 = direct_deriv_P8_P2+deriv_P8_P5*deriv_P5_P2+deriv_P8_P6*deriv_P6_P2;
    const Real deriv_P9_P2 = direct_deriv_P9_P2+deriv_P9_P5*deriv_P5_P2+deriv_P9_P6*deriv_P6_P2;
    if(deriv_property_8[icell][2]!=deriv_P8_P2)
      return 0;
    if(deriv_property_9[icell][2]!=deriv_P9_P2)
      return 0;
    // law 10
    // eval
    Real deriv_P10_P7, deriv_P10_P8, deriv_P10_P9, deriv_P11_P7, deriv_P11_P8, deriv_P11_P9;
    _computeLawDerivativesRef(5,in_out_ref,deriv_P10_P7,deriv_P10_P8,deriv_P10_P9,deriv_P11_P7,deriv_P11_P8,deriv_P11_P9);
    // dependent de P0 P1 P2
    if(deriv_property_10[icell].size()!=3)
      return 0;
    if(deriv_property_11[icell].size()!=3)
      return 0;
    // correction
    const Real deriv_P10_P0 = deriv_P10_P7*deriv_P7_P0 + deriv_P10_P8*deriv_P8_P0 + deriv_P10_P9*deriv_P9_P0;
    const Real deriv_P11_P0 = deriv_P11_P7*deriv_P7_P0 + deriv_P11_P8*deriv_P8_P0 + deriv_P11_P9*deriv_P9_P0;
    if(deriv_property_10[icell][0]!=deriv_P10_P0)
      return 0;
    if(deriv_property_11[icell][0]!=deriv_P11_P0)
      return 0;
    const Real deriv_P10_P1 = deriv_P10_P7*deriv_P7_P1 + deriv_P10_P8*deriv_P8_P1 + deriv_P10_P9*deriv_P9_P1;
    const Real deriv_P11_P1 = deriv_P11_P7*deriv_P7_P1 + deriv_P11_P8*deriv_P8_P1 + deriv_P11_P9*deriv_P9_P1;
    if(deriv_property_10[icell][1]!=deriv_P10_P1)
      return 0;
    if(deriv_property_11[icell][1]!=deriv_P11_P1)
      return 0;
    const Real deriv_P10_P2 = deriv_P10_P8*deriv_P8_P2 + deriv_P10_P9*deriv_P9_P2;
    const Real deriv_P11_P2 = deriv_P11_P8*deriv_P8_P2 + deriv_P11_P9*deriv_P9_P2;
    if(deriv_property_10[icell][2]!=deriv_P10_P2)
      return 0;
    if(deriv_property_11[icell][2]!=deriv_P11_P2)
      return 0;
  }
  return 1;
}

void
LawGraphTesterService::
_computeLawDerivativesRef(const Integer function_id, UniqueArray<Real>& in_out_ref, Real& deriv_1_1, Real& deriv_1_2) const
{
  TwoInOneOutAlgo algo_2_1;
  FuctionDefininition current_function = m_functions_def[function_id];
  const Real in_1 = in_out_ref[current_function.m_in_property_id[0]];
  const Real in_2 = in_out_ref[current_function.m_in_property_id[1]];
  Real& out_1 = in_out_ref[current_function.m_out_property_id[0]];
  const Real param_1 = m_param_ref[current_function.m_param_prop_id];
  algo_2_1.compute(in_1, in_2, out_1, deriv_1_1, deriv_1_2, param_1);
}

void
LawGraphTesterService::
_computeLawDerivativesRef(const Integer function_id, UniqueArray<Real>& in_out_ref,
    Real& deriv_1_1, Real& deriv_1_2, Real& deriv_1_3,
    Real& deriv_2_1, Real& deriv_2_2, Real& deriv_2_3) const
{
  ThreeInTwoOutAlgo algo_3_2;
  FuctionDefininition current_function = m_functions_def[function_id];
  const Real in_1 = in_out_ref[current_function.m_in_property_id[0]];
  const Real in_2 = in_out_ref[current_function.m_in_property_id[1]];
  const Real in_3 = in_out_ref[current_function.m_in_property_id[2]];
  Real& out_1 = in_out_ref[current_function.m_out_property_id[0]];
  Real& out_2 = in_out_ref[current_function.m_out_property_id[1]];
  const Real param_1 = m_param_ref[current_function.m_param_prop_id];
  algo_3_2.compute(in_1, in_2, in_3,
      out_1, deriv_1_1, deriv_1_2, deriv_1_3,
      out_2, deriv_2_1, deriv_2_2, deriv_2_3, param_1);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_LAWGRAPHTESTER(LawGraphTester,LawGraphTesterService);
