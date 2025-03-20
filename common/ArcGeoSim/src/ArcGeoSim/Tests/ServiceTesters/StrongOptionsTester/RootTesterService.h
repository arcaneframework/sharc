#ifndef ARCANE_STRONGOPTIONS_ROOT_TESTER_SERVICE_H
#define ARCANE_STRONGOPTIONS_ROOT_TESTER_SERVICE_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"
#include "TypesStrongOptionsTester.h"
#include "IX.h"
#include "RootTester_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
class IOptionsX1;

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class RootTesterService
: public ArcaneRootTesterObject
{
 public:
  /** Constructeur de la classe */
  RootTesterService(const Arcane::ServiceBuildInfo & sbi)
   : ArcaneRootTesterObject(sbi) {}
  /** Constructeur de la classe strong options */
  RootTesterService(std::shared_ptr<IOptionsRootTester> options,  Arcane::ITraceMng* _trace_mng,  Arcane::ISubDomain* _sub_domain,
      Arcane::IServiceInfo* _service_info, Arcane::IBase* _parent)
  : ArcaneRootTesterObject(options, _trace_mng, _sub_domain, _service_info, _parent) {}
  /** Destructeur de la classe */
  ~RootTesterService() {}
  /** build: strong root options (eq m_strongoptions)
      scope: simple, enum and service options     */
  void init();
  //! Run test on strong root options */
  int test();

 private:
  /** build: strong X1Service instance
      scope: complex and service options */
  std::shared_ptr<IX> _buildX1();
  /* test on strong X1Service instance */
  bool _testX1(const IOptionsX1* options_x1);

  /** root strong options */
  std::unique_ptr<IOptionsRootTester> m_strongoptions;
};

#endif
