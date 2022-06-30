// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_TESTS_SERVICETESTERS_LAWGRAPHTESTER_LAWTESTERSERVICE_H
#define ARCGEOSIM_TESTS_SERVICETESTERS_LAWGRAPHTESTER_LAWTESTERSERVICE_H

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"

#include "LawGraphTester_axl.h"

#include "ThreeInTwoOut_law.h"
#include "TwoInOneOut_law.h"

#include "ArcGeoSim/Physics/Law2/VariableManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionManager.h"
#include "ArcGeoSim/Physics/Law2/FunctionEvaluator.h"

#include <arcane/random/Uniform01.h>
#include <arcane/random/UniformSmallInt.h>
#include <arcane/random/LinearCongruential.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class LawGraphTesterService
    : public ArcaneLawGraphTesterObject
{

private:
  /*---------------------------------------------------------------------------*/
  enum eFunctionKind {Two_In_One_Out, Three_In_Two_Out};
  //! light function definition view
  class FuctionDefininition
  {
  public :
    FuctionDefininition(){};
    FuctionDefininition(Integer* _property_id)
    {
      if(_property_id[5]==-1){
        m_functions_kind = Two_In_One_Out;
        m_in_property_id.resize(2);
        m_in_property_id[0] = _property_id[0];
        m_in_property_id[1] = _property_id[1];
        m_out_property_id.resize(1);
        m_out_property_id[0] = _property_id[2];
        m_param_prop_id = _property_id[3];
      }
      else{
        m_functions_kind=Three_In_Two_Out;
        m_in_property_id.resize(3);
        m_in_property_id[0] = _property_id[0];
        m_in_property_id[1] = _property_id[1];
        m_in_property_id[2] = _property_id[2];
        m_out_property_id.resize(2);
        m_out_property_id[0] = _property_id[3];
        m_out_property_id[1] = _property_id[4];
        m_param_prop_id = _property_id[5];
      }
    };

    virtual ~FuctionDefininition(){}

    eFunctionKind m_functions_kind;
    UniqueArray<Integer> m_in_property_id;
    UniqueArray<Integer> m_out_property_id;
    Integer m_param_prop_id;
  };

  /*---------------------------------------------------------------------------*/
  //! class to compute function with two inputs and one output specialized by parameters

  struct TwoInOneOutAlgo
  {
    void compute(const Real i_1, const Real i_2, Real& o_1, Real& do_1_1, Real& do_1_2,
        const Real p_1) const
    {
      o_1=p_1*i_1+2*p_1*i_2;
      do_1_1 = i_1*p_1;
      do_1_2 = i_2*p_1;
    }
  };

  /*---------------------------------------------------------------------------*/
  //! class to compute function with three inputs and two outputs specialized by parameters

  struct ThreeInTwoOutAlgo
  {
    void compute(const Real i_1, const Real i_2, const Real i_3,
        Real& o_1, Real& do_1_1, Real& do_1_2, Real& do_1_3,
        Real& o_2, Real& do_2_1, Real& do_2_2, Real& do_2_3,
        const Real p_1) const
    {
      o_1=p_1*i_1+2*p_1*i_2+3*p_1*i_3;
      o_2=2*p_1*i_1+3*p_1*i_2+4*p_1*i_3;
      do_1_1 = i_1*p_1;
      do_1_2 = i_1*i_2*p_1;
      do_1_3 = i_2*p_1;
      do_2_1 = i_3;
      do_2_2 = i_1;
      do_2_3 = i_3*i_1;
    }
  };

public:

  //! Constructor
  LawGraphTesterService(const Arcane::ServiceBuildInfo & sbi)
: ArcaneLawGraphTesterObject(sbi)
, m_num_of_function(6)
, m_nb_root_prop(3)
, m_nb_leafs_prop(2)
, m_num_of_properties(12)
, m_num_param(2)
, m_param_ref(m_num_param)
, m_properties(m_num_of_properties)
, m_params(m_num_param)
, m_functions_def(m_num_of_function)
, m_root_begin_test1(0)
, m_root_end_test1(2)
, m_leaf_begin_test1(10)
, m_leaf_end_test1(11)
, m_root_begin_test2(0)
, m_root_end_test2(2)
, m_leaf_begin_test2(7)
, m_leaf_end_test2(9)
, m_root_begin_test3(0)
, m_root_end_test3(2)
, m_leaf_begin_test3(10)
, m_leaf_end_test3(11)
, m_variable_mng(mesh())
// activate derivatives composition
// don t keep intermediate composition derivatives
, m_function_mng(false, true, Law::eComposition)
, m_uniform(m_generator)
{
    // definition graph (must be sort)
    Integer function_prop_id[][6] =
    {{0,1,3,0,-1,-1},{1,0,4,1,-1,-1},{0,1,2,5,6,0},
        {3,4,7,1,-1,-1},{5,6,2,8,9,0},{7,8,9,10,11,0}};
    const Integer function_insertion_order[] = {4,0,5,2,1,3};
    m_function_insertion_order.resize(m_num_of_function);
    for(int i =0;i<m_num_of_function;++i){
      m_functions_def[i] = FuctionDefininition(function_prop_id[i]);
      m_function_insertion_order[i]=function_insertion_order[i];
    }
}

  //! Destructor
  ~LawGraphTesterService() {};

public:

  //! Initialization
  void init();

  //! Run the tests
  Integer test();

private:
  //! add function in graph
  void _addFunction(FuctionDefininition& function_def);
  //! first test validate the full graph evaluation
  Integer _test_1() const;
  //! second test validate a partial graph evaluation
  Integer _test_2() const;
  //! third test validate a partial graph evaluation with derivative evaluation and correction for composition
  Integer _test_3() const;
  //! affectation of "random" sub graph
  void _graphAffectation(Integer root_begin, Integer root_end,Integer leaf_begin) const;
  //! check if complete or partial graph evaluation is correct
  Integer _checkResults(Integer root_begin, Integer root_end, Integer leaf_begin, Integer leaf_end) const;
  //! check if full graph derivative evaluation is correct (including correct correction)
  Integer _checkResultsWithDerivatives() const;
  //! compute reference direct derivatives for Two_In_One_Out laws
  void _computeLawDerivativesRef(const Integer function_id, UniqueArray<Real>& in_out_ref, Real& deriv_1_1, Real& deriv_1_2) const;
  //! compute reference direct derivatives for Three_In_Two_Out laws
  void _computeLawDerivativesRef(const Integer function_id, UniqueArray<Real>& in_out_ref,
      Real& deriv_1_1, Real& deriv_1_2, Real& deriv_1_3,
      Real& deriv_2_1, Real& deriv_2_2, Real& deriv_2_3) const;
private:

  // management of the full graph
  const Integer m_num_of_function;
  const Integer m_nb_root_prop;
  const Integer m_nb_leafs_prop;
  const Integer m_num_of_properties;
  const Integer m_num_param;

  UniqueArray<Real> m_param_ref;
  UniqueArray<Real> m_root_prop_ref;

  // properties of the graph
  UniqueArray<Law::ScalarRealProperty> m_properties;

  // parameters to specialized functions
  UniqueArray<Law::ScalarRealProperty> m_params;

  UniqueArray<FuctionDefininition> m_functions_def;

  // management of partial graph for the three tests
  Integer m_root_begin_test1;
  Integer m_root_end_test1;
  Integer m_leaf_begin_test1;
  Integer m_leaf_end_test1;
  Integer m_root_begin_test2;
  Integer m_root_end_test2;
  Integer m_leaf_begin_test2;
  Integer m_leaf_end_test2;
  Integer m_root_begin_test3;
  Integer m_root_end_test3;
  Integer m_leaf_begin_test3;
  Integer m_leaf_end_test3;

  // variables and function managers
  Law::VariableManager m_variable_mng;
  Law::FunctionManager m_function_mng;

  // random number generator
  Arcane::random::MinstdRand m_generator;
  mutable Arcane::random::Uniform01<Arcane::random::MinstdRand> m_uniform;
  
  // functions insertion in graph order
  UniqueArray<Integer> m_function_insertion_order;
};

#endif /* ARCGEOSIM_TESTS_SERVICETESTERS_LAWGRAPHTESTER2_LAWTESTERSERVICE_H */
