// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef AGS_UTILS_UTILS_H
#define AGS_UTILS_UTILS_H

#include <arcane/ArcaneVersion.h>
#include <arcane/utils/ArcanePrecomp.h>

#include <arcane/utils/ArcaneGlobal.h>
#include <arcane/utils/UtilsTypes.h>
#include <arcane/utils/Limits.h>

#include <arcane/utils/Iostream.h>
#include <arcane/utils/StdHeader.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/String.h>
#include <arcane/utils/IOException.h>
#include <arcane/utils/TraceAccessor.h>
#include <arcane/IIOMng.h>

#include <arcane/utils/FatalErrorException.h>

#include <arcane/utils/ITraceMng.h>
#include <arcane/IParallelMng.h>
#include <arcane/utils/Iostream.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/Array2.h>
#include <arcane/ItemVector.h>

#include <map>
#include <set>

#ifndef F2C
#if defined(arch_Linux) || defined(arch_SunOS) || defined(arch_IRIX)
/* Ajoute un caractere '_' a la fin du nom de la fonction C */
#define F2C(functionC) functionC##_
#elif defined(arch_AIX) || defined(WIN32) || defined(arch_Windows)
#define F2C(functionC) functionC
#else
#error "La macro F2C est a definir pour cette architecture."
/* #define F2C(functionC) functionC */
#endif
#endif

#define FORMAT(w,p) std::setiosflags(ios::fixed)<<std::setw(w)<<std::setprecision(p)
#define FORMATS(w,p) std::setiosflags(ios::scientific)<<std::setw(w)<<std::setprecision(p)
#define FORMATW(w) std::setw(w)
#define FORMATF(n,c) std::setfill(c)<<std::setw(n)<<(c)<<std::setfill(' ')

#include "ArcGeoSim/Utils/DefineUtils.h"

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include "ArcGeoSim/Utils/IVisitor.h"
#include "ArcGeoSim/Utils/IOBuffer.h"
#include "ArcGeoSim/Utils/Table.h"
#include "ArcGeoSim/Utils/FunctionUtils.h"
#include "ArcGeoSim/Utils/ArrayUtils.h"
#include "ArcGeoSim/Utils/MakeDelegatedPtr.h"

#endif /* AGS_UTILS_UTILS_H */
