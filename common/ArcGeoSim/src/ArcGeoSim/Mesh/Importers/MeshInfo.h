// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef MESHINFO_H_
#define MESHINFO_H_

#include <string>
#include <arcane/utils/Array.h>
#include <arcane/ArcaneTypes.h>

using namespace Arcane;

class Singleton
{
public:
	int nx;
	int ny;
	int nz;
	
	IntegerSharedArray m_nx ;
	IntegerSharedArray m_ny ;
	IntegerSharedArray m_nz ;
	
	//BOTTOM or TOP
	char layer_type[7];
	//UP or DOWN
	char z_axis[5];

enum eMeshType
	{
		VTK_MT_Unknown,
		VTK_MT_StructuredGrid,
		VTK_MT_DistStructuredGrid,
		VTK_MT_StructuredGraph,
		VTK_MT_StructuredDualGrid,
		VTK_MT_UnstructuredGrid
	};
	eMeshType mesh_type;
};
extern Singleton singleton;
#endif 
