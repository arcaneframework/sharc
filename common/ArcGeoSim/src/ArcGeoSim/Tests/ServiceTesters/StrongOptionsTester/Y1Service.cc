/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "Y1Service.h"
// include pour les macros REGISTER_STRONG_OPTIONS_Y1 et REGISTER_INSTANCE_Y1
#include "Y1_StrongOptions.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
Y1Service::
init()
{
  ;
}

/*---------------------------------------------------------------------------*/

void 
Y1Service::
apply()
{
  std::cout << " Y1 apply Begin " << std::endl;
  std::cout << " Y1 apply End " << std::endl;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
ARCANE_REGISTER_SERVICE_Y1(Y1,Y1Service);
// Pour les parametres nommes
REGISTER_STRONG_OPTIONS_Y1();
// Pour pouvoir utiliser l instance Y1 de type Y1Service comme defaut dans X1.axl
// Possible car toutes les options de Y1.axl ont un defaut ou sont optionnelles
REGISTER_INSTANCE_Y1(Y1, Y1Service, IY);


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
