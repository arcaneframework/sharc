/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
//
// Include pour pouvoir instancier les strong options et service
//
#include "RootTesterService.h"
#include "RootTester_StrongOptions.h"
#include "IY.h"
#include "Y1Service.h"
#include "IX.h"
#include "X1_StrongOptions.h"
#include "X1Service.h"
#include "X2_StrongOptions.h"
#include "X2Service.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// build root test options
// test of simple, enum and service options
void
RootTesterService::
init()
{

  info() << "Strong Options Tester Module Init";

  using namespace X2OptionsNames;
  IX* case_service_x3 = options()->serviceX3();
  TypesStrongOptionsTester::eSimpleEnum en1 = TypesStrongOptionsTester::eSimpleEnum::StrongEnum2;
  TypesStrongOptionsTester::eSimpleEnum en2 = TypesStrongOptionsTester::eSimpleEnum::StrongEnum3;
  info() << "Building Manual Strong Options";
  // first IX* service
  // options
  auto strong_x2_alpha = std::make_shared<StrongOptionsX2>(
    _simpleInt1 = 1
  );
  // service
  auto strong_x2_service_alpha = std::make_shared<X2Service>(strong_x2_alpha, traceMng(), subDomain(), nullptr, nullptr);
  //
  // second IX* service
  // options
  auto strong_x2_beta = std::make_shared<StrongOptionsX2>(
      _simpleInt1 = 2
  );
  // service
  auto strong_x2_service_beta = std::make_shared<X2Service>(strong_x2_beta, traceMng(), subDomain(), nullptr, nullptr);
  //
  // third IX* service more complex from X1Service
  auto strong_x1_service = _buildX1();
  //
  // root options
  using namespace RootTesterOptionsNames;
  std::unique_ptr<StrongOptionsRootTester> strongoptions(new StrongOptionsRootTester
  {
    _simpleInt3 = 1,
    _simpleReal3 = 1.5,
    _simpleBool3 = true,
    _simpleString3 = "HYPRE",
    _simpleEnum3 = en1,
    _simpleInt4 = 1,
    _simpleInt4 = 2,
    _simpleInt4 = 3,
    _simpleReal4 = 1.,
    _simpleReal4 = 2.,
    _simpleString4 = "Test String 1",
    _simpleBool4 = false,
    _simpleEnum4 = en2,
    _simpleBool5 = false,
    _simpleEnum5 = en2,
    _serviceX1 = strong_x2_service_alpha,
    _serviceX3 = std::shared_ptr<IX>(case_service_x3),
    _serviceX4 = strong_x2_service_beta,
    _serviceX4 = strong_x1_service
  }
  );
  //
  // setter
  //
  // single
  strongoptions->simpleInt2(30);
  strongoptions->simpleReal2(12.3);
  strongoptions->simpleBool2(true);
  strongoptions->simpleEnum2(TypesStrongOptionsTester::eSimpleEnum::StrongEnum1);
  strongoptions->simpleString2("HYPRE_SET");
  // multi
  strongoptions->simpleInt5({{2,3}});
  strongoptions->simpleReal5({{1.5,2.5,3.5,4.5,5.5}});
  strongoptions->simpleString5({{"HYPRE_SET_1","HYPRE_SET_2","HYPRE_SET_3"}});
  strongoptions->simpleBool5({{true,false,true,false}});
  strongoptions->simpleEnum5({{TypesStrongOptionsTester::eSimpleEnum::StrongEnum1}});
  // service
  auto strong_x2_gamma = std::make_shared<StrongOptionsX2>(
    X2OptionsNames::_simpleInt1 = 4
  );
  auto strong_x2_service_gamma = std::make_shared<X2Service>(strong_x2_gamma, traceMng(), subDomain(), nullptr, nullptr);
  strongoptions->serviceX5(strong_x2_service_gamma);
  strongoptions->serviceX5Multi({{strong_x2_service_beta, strong_x2_service_gamma}});
  // end setter
  //
  m_strongoptions = std::move(strongoptions);
}


// building strong X1Service instance
// test of complex options
std::shared_ptr<IX>
RootTesterService::
_buildX1()
{

  // 1 - With Strong options
  USING_NAMESPACES_X1();
  //
  auto complex_11 = std::make_shared<StrongOptionsComplex11>(
    Complex11OptionsNames::_simpleBool1 = false,
    _multiString1 = "Albert",
    _multiString1 = "Bernard"
  );
  //
  auto complex_121 = std::make_shared<StrongOptionsComplex121>(
    _simpleReal4 = -56.5,
    Complex121OptionsNames::_simpleBool1 = false
  );
  // full default complex
  auto complex_12_alpha = std::make_shared<StrongOptionsComplex12>();
  //
  auto complex_12_beta = std::make_shared<StrongOptionsComplex12>(
     _complex121 = complex_121
  );
  // default for setter
  auto complex_12_gamma = std::make_shared<StrongOptionsComplex12>();

  std::cout << "building Complex1 Options" << std::endl;
  auto complex_1 = std::make_shared<StrongOptionsComplex1>(
  _simpleReal1 = -2.5,
  _simpleReal2 = -3.5,
  _complex11 = complex_11,
  _complex12 = complex_12_alpha,
  _complex12 = complex_12_beta,
  _complex12 = complex_12_gamma
  );
  std::cout << "building X1 Options" << std::endl;
  auto strong_x1_manual = std::make_shared<StrongOptionsX1>(
  _complex1 = complex_1
  );
  //
  // setter
  //
  // single
  auto complex_121_gamma = std::make_shared<StrongOptionsComplex121>(
      _simpleReal4 = -52,
      Complex121OptionsNames::_simpleBool1 = true
  );
  complex_12_gamma->complex121(complex_121_gamma);
  // multi
  auto complex_2_0 =  std::make_shared<StrongOptionsComplex2>(
  _simpleInt1 = 1
  );
  auto complex_2_1 =  std::make_shared<StrongOptionsComplex2>(
  _simpleInt1 = 2
  );
  strong_x1_manual->complex2({{complex_2_0, complex_2_1, complex_2_1}});
  // return my strong X1Service instance
  return std::make_shared<X1Service>(strong_x1_manual, traceMng(), subDomain(), nullptr, nullptr);

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int RootTesterService::
test()
{
  bool is_ok = true;
  // axl default
  is_ok = is_ok && m_strongoptions->simpleInt1() == 3;
  is_ok = is_ok && m_strongoptions->simpleReal1() == 1.5;
  is_ok = is_ok && m_strongoptions->simpleBool1() == true;
  is_ok = is_ok && m_strongoptions->simpleEnum1() == TypesStrongOptionsTester::eSimpleEnum::StrongEnum2;
  is_ok = is_ok && m_strongoptions->simpleString1() == "PETSC";
  // by setter
  is_ok = is_ok && m_strongoptions->simpleInt2() == 30;
  is_ok = is_ok && m_strongoptions->simpleReal2() == 12.3;
  is_ok = is_ok && m_strongoptions->simpleBool2() == true;
  is_ok = is_ok && m_strongoptions->simpleEnum2() == TypesStrongOptionsTester::eSimpleEnum::StrongEnum1;
  is_ok = is_ok && m_strongoptions->simpleString2() == "HYPRE_SET";
  // from init
  // single
  is_ok = is_ok && m_strongoptions->simpleInt3() == 1;
  is_ok = is_ok && m_strongoptions->simpleReal3() == 1.5;
  is_ok = is_ok && m_strongoptions->simpleBool3() == true;
  is_ok = is_ok && m_strongoptions->simpleEnum3() == TypesStrongOptionsTester::eSimpleEnum::StrongEnum2;
  is_ok = is_ok && m_strongoptions->simpleString3() == "HYPRE";
  // multi
  is_ok = is_ok && m_strongoptions->simpleInt4().size() == 3;
  is_ok = is_ok && m_strongoptions->simpleInt4()[0] == 1;
  is_ok = is_ok && m_strongoptions->simpleInt4()[1] == 2;
  is_ok = is_ok && m_strongoptions->simpleInt4()[2] == 3;
  is_ok = is_ok && m_strongoptions->simpleReal4().size() == 2;
  is_ok = is_ok && m_strongoptions->simpleReal4()[0] == 1.;
  is_ok = is_ok && m_strongoptions->simpleReal4()[1] == 2.;
  is_ok = is_ok && m_strongoptions->simpleString4().size() == 1;
  is_ok = is_ok && m_strongoptions->simpleString4()[0] == "Test String 1";
  is_ok = is_ok && m_strongoptions->simpleBool4().size() == 1;
  is_ok = is_ok && m_strongoptions->simpleBool4()[0] == false;
  is_ok = is_ok && m_strongoptions->simpleEnum4().size() == 1;
  is_ok = is_ok && m_strongoptions->simpleEnum4()[0] == TypesStrongOptionsTester::eSimpleEnum::StrongEnum3;
  // multi setter
  is_ok = is_ok && m_strongoptions->simpleInt5().size() == 2;
  is_ok = is_ok && m_strongoptions->simpleInt5()[0] == 2;
  is_ok = is_ok && m_strongoptions->simpleInt5()[1] == 3;
  is_ok = is_ok && m_strongoptions->simpleReal5().size() == 5;
  is_ok = is_ok && m_strongoptions->simpleReal5()[0] == 1.5;
  is_ok = is_ok && m_strongoptions->simpleReal5()[1] == 2.5;
  is_ok = is_ok && m_strongoptions->simpleReal5()[2] == 3.5;
  is_ok = is_ok && m_strongoptions->simpleReal5()[3] == 4.5;
  is_ok = is_ok && m_strongoptions->simpleReal5()[4] == 5.5;
  is_ok = is_ok && m_strongoptions->simpleString5().size() == 3;
  is_ok = is_ok && m_strongoptions->simpleString5()[0] == "HYPRE_SET_1";
  is_ok = is_ok && m_strongoptions->simpleString5()[1] == "HYPRE_SET_2";
  is_ok = is_ok && m_strongoptions->simpleString5()[2] == "HYPRE_SET_3";
  is_ok = is_ok && m_strongoptions->simpleBool5().size() == 4;
  is_ok = is_ok && m_strongoptions->simpleBool5()[0] == true;
  is_ok = is_ok && m_strongoptions->simpleBool5()[1] == false;
  is_ok = is_ok && m_strongoptions->simpleBool5()[2] == true;
  is_ok = is_ok && m_strongoptions->simpleBool5()[3] == false;
  is_ok = is_ok && m_strongoptions->simpleEnum5().size() == 1;
  is_ok = is_ok && m_strongoptions->simpleEnum5()[0] == TypesStrongOptionsTester::eSimpleEnum::StrongEnum1;
  // service
  // from init
  X2Service* service_x1 = dynamic_cast<X2Service*>(m_strongoptions->serviceX1());
  is_ok = is_ok && service_x1 && service_x1->options()->simpleInt1()==1;
  // from default
  X2Service* service_x2 = dynamic_cast<X2Service*>(m_strongoptions->serviceX2());
  is_ok = is_ok && service_x2 && service_x2->options()->simpleInt1()==30;
  // from arc
  X2Service* service_x3 = dynamic_cast<X2Service*>(m_strongoptions->serviceX3());
  is_ok = is_ok && service_x3;
  is_ok = is_ok && dynamic_cast<CaseOptionsTX2<CaseOptionsX2>*>(service_x3->options());
  is_ok = is_ok && service_x3->options()->simpleInt1()==5;
  // array from init
  /* bug arcane3 multi service to patch or not
  is_ok = is_ok && m_strongoptions->serviceX4().size() == 2;
  X2Service* service_x4_1 = dynamic_cast<X2Service*>(m_strongoptions->serviceX4()[0]);
  is_ok = is_ok && service_x1 && service_x4_1->options()->simpleInt1()==2;
  X1Service* service_x4_2 = dynamic_cast<X1Service*>(m_strongoptions->serviceX4()[1]);
  // Test on complex X1Service Instance
  is_ok = is_ok && _testX1(service_x4_2->options());*/
  // setter
  X2Service* service_x5 = dynamic_cast<X2Service*>(m_strongoptions->serviceX5());
  is_ok = is_ok && service_x5 && service_x5->options()->simpleInt1()==4;
  /* bug arcane3 multi service to patch or not
  is_ok = is_ok && m_strongoptions->serviceX5Multi().size() == 2;
  X2Service* service_x5_1 = dynamic_cast<X2Service*>(m_strongoptions->serviceX5Multi()[0]);
  is_ok = is_ok && service_x5_1 && service_x5_1->options()->simpleInt1()==2;
  X2Service* service_x5_2 = dynamic_cast<X2Service*>(m_strongoptions->serviceX5Multi()[1]);
  is_ok = is_ok && service_x5_2 && service_x5_2->options()->simpleInt1()==4;
  */
  return (is_ok ? 1:0);
}

bool RootTesterService::
_testX1(const IOptionsX1* options_x1)
{
  bool is_ok = true;
  //
  is_ok = is_ok && options_x1->complex1().simpleReal1() == -2.5;
  is_ok = is_ok && options_x1->complex1().simpleReal2() == -3.5;
  // array complex
  is_ok = is_ok && options_x1->complex1().complex12().size() == 3;
  auto current_complex12 = options_x1->complex1().complex12()[0];
  is_ok = is_ok && current_complex12->simpleReal3() == 15.5;
  // default
  is_ok = is_ok && current_complex12->complex121().simpleReal4() == 1.2;
  is_ok = is_ok && current_complex12->complex121().simpleBool1() == true;
  is_ok = is_ok && current_complex12->complex121().simpleReal3() == 18.5;
  is_ok = is_ok && current_complex12->complex121().simpleRealMulti().size() == 0;
  is_ok = is_ok && current_complex12->complex121().serviceMultiY().size() == 0;
  is_ok = is_ok && dynamic_cast<Y1Service*>(current_complex12->complex121().serviceY());
  //
  current_complex12 = options_x1->complex1().complex12()[1];
  is_ok = is_ok && current_complex12->simpleReal3() == 15.5;
  is_ok = is_ok && current_complex12->complex121().simpleReal4() == -56.5;
  is_ok = is_ok && current_complex12->complex121().simpleBool1() == false;
  //default
  is_ok = is_ok && current_complex12->complex121().simpleReal3() == 18.5;
  is_ok = is_ok && current_complex12->complex121().simpleRealMulti().size() == 0;
  is_ok = is_ok && current_complex12->complex121().serviceMultiY().size() == 0;
  // setter
  current_complex12 = options_x1->complex1().complex12()[2];
  is_ok = is_ok && current_complex12->simpleReal3() == 15.5;
  is_ok = is_ok && current_complex12->complex121().simpleReal4() == -52.;
  is_ok = is_ok && current_complex12->complex121().simpleBool1() == true;
  // default
  is_ok = is_ok && current_complex12->complex121().simpleReal3() == 18.5;
  //
  is_ok = is_ok && dynamic_cast<Y1Service*>(current_complex12->complex121().serviceY());
  is_ok = is_ok && options_x1->complex1().complex11().simpleBool1() == false;
  is_ok = is_ok && options_x1->complex1().complex11().multiString1().size() == 2;
  is_ok = is_ok && options_x1->complex1().complex11().multiString1()[0] == "Albert";
  is_ok = is_ok && options_x1->complex1().complex11().multiString1()[1] == "Bernard";
  // setter
  is_ok = is_ok && options_x1->complex2().size() == 3;
  is_ok = is_ok && options_x1->complex2()[0]->simpleInt1() == 1;
  is_ok = is_ok && options_x1->complex2()[1]->simpleInt1() == 2;
  is_ok = is_ok && options_x1->complex2()[2]->simpleInt1() == 2;
  return is_ok;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_ROOTTESTER(RootTester,RootTesterService);
// Pour les parametres nommes
REGISTER_STRONG_OPTIONS_ROOTTESTER();
