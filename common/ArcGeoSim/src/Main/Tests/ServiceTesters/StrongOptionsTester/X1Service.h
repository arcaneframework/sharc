#ifndef ARCANE_STRONGOPTIONS_X1_SERVICE_H
#define ARCANE_STRONGOPTIONS_X1_SERVICE_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "IX.h"
#include "IY.h"
#include "X1_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

class X1Service
  : public ArcaneX1Object
{
public:
  
  /** Constructeur de la classe */
  X1Service(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneX1Object(sbi) {}
  X1Service(std::shared_ptr<IOptionsX1> options,  Arcane::ITraceMng* _trace_mng, Arcane::ISubDomain* _sub_domain,
    Arcane::IServiceInfo* _service_info, Arcane::IBase* _parent)
      : ArcaneX1Object(options, _trace_mng, _sub_domain, _service_info, _parent) {}
  /** Destructeur de la classe */
  ~X1Service() {}
  
public:

  void build();

  void init();

  void apply();

private:

};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#endif
