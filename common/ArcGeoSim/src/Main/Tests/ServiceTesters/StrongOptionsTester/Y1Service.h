#ifndef ARCANE_STRONGOPTIONS_Y1_SERVICE_H
#define ARCANE_STRONGOPTIONS_Y1_SERVICE_H

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "IY.h"
#include "Y1_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

class Y1Service
  : public ArcaneY1Object
{
public:
  
  /** Constructeur de la classe */
  /** Constructeur classique pour case options */
  Y1Service(const Arcane::ServiceBuildInfo & sbi)
    : ArcaneY1Object(sbi) {}
  /** Constructeur complet pour strong options */
  Y1Service(std::shared_ptr<IOptionsY1> options, Arcane::ITraceMng* _trace_mng, Arcane::ISubDomain* _sub_domain,
    Arcane::IServiceInfo* _service_info, Arcane::IBase* _parent)
    : ArcaneY1Object(options, _trace_mng, _sub_domain, _service_info, _parent) {}
  /** Constructeur reduit pour strong options et register instance */
  Y1Service(std::shared_ptr<IOptionsY1> options)
    : ArcaneY1Object(options) {}
  /** Destructeur de la classe */
  ~Y1Service() {}
  
public:

  //! Initialisation
  void init();

  //! Apply service function
  void apply();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#endif
