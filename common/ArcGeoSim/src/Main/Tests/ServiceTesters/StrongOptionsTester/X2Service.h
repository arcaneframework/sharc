#ifndef ARCANE_STRONGOPTIONS_X2_SERVICE_H
#define ARCANE_STRONGOPTIONS_X2_SERVICE_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "IX.h"
#include "X2_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

class X2Service
  : public ArcaneX2Object
{
public:
  
  /** Constructeur de la classe */
  /** Constructeur classique pour case options */
  X2Service(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneX2Object(sbi) {}
  /** Constructeur complet pour strong options */
  X2Service(std::shared_ptr<IOptionsX2> options, Arcane::ITraceMng* _trace_mng, Arcane::ISubDomain* _sub_domain,
  Arcane::IServiceInfo* _service_info, Arcane::IBase* _parent)
    : ArcaneX2Object(options, _trace_mng, _sub_domain, _service_info, _parent) {}
  /** Constructeur reduit pour strong options et register instance */
  X2Service(std::shared_ptr<IOptionsX2> options)
    : ArcaneX2Object(options) {}
  /** Destructeur de la classe */
  ~X2Service() {}
  
public:

  void build();

  void init();

  void apply();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#endif
