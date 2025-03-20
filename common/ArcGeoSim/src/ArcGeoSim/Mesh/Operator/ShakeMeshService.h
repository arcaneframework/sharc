#ifndef SHAKEMESHSERVICE_H_
#define SHAKEMESHSERVICE_H_

#include "IMeshOperator.h"
#include "ShakeMesh_axl.h"


using namespace Arcane;

class ShakeMeshService : public  ArcaneShakeMeshObject {
public:
	//! Constructor
    ShakeMeshService(const ServiceBuildInfo& sbi) :
    ArcaneShakeMeshObject(sbi) {}
    
    void ApplyOperator() ;
    
private:
	static double _myrandom();
};
#endif /*SHAKEMESHSERVICE_H_*/
