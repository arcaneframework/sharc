// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef CUBETRANSFORMATIONSERVICE_H_
#define CUBETRANSFORMATIONSERVICE_H_

#include "IMeshOperator.h"
#include "CubeTransformationMesh_axl.h"


using namespace Arcane;

class CubeTransformationMeshService : public  ArcaneCubeTransformationMeshObject {
public:
	//! Constructor
    CubeTransformationMeshService(const ServiceBuildInfo& sbi) :
    ArcaneCubeTransformationMeshObject(sbi) {}
    
    void ApplyOperator() ;
    
private:
	static double _myrandom();
};
#endif /*CUBETRANSFORMATIONSERVICE_H_*/
