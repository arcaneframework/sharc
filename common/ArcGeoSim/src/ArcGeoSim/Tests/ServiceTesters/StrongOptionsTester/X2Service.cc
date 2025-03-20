/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "X2Service.h"
// include pour les macros REGISTER_STRONG_OPTIONS_X2 et REGISTER_INSTANCE_X2
#include "X2_StrongOptions.h"


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
X2Service::
build(){}

/*---------------------------------------------------------------------------*/

void 
X2Service::
init(){}

/*---------------------------------------------------------------------------*/

void 
X2Service::
apply(){}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_X2(X2,X2Service);
// Pour les parametres nommes
REGISTER_STRONG_OPTIONS_X2();
// Pour pouvoir utiliser l instance X2 de type X2Service comme defaut dans Root.axl
// Possible car toutes les options de X2.axl ont un defaut ou sont optionnelles
REGISTER_INSTANCE_X2(X2, X2Service, IX);


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
