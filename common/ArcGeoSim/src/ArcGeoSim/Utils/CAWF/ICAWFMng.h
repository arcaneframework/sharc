// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * ICAWFMng.h
 *
 *  Created on: 3 mars 2022
 *      Author: gratienj
 */

#pragma once

#include <vector>

BEGIN_ARCGEOSIM_NAMESPACE

class ICAWFMng
{
public:
    class ICouplingMesh
    {
    public :
        typedef Integer lid_type ;
        typedef Int64   uid_type ;
        ICouplingMesh() {}
        virtual ~ICouplingMesh() {}

        virtual void initializeCAWF(std::vector<double>& vertices,
                                    std::vector<uid_type>& vertex_uids) = 0 ;
    };

    class ITimeIterStateOp
    {
    public:
      ITimeIterStateOp() {}
      virtual ~ITimeIterStateOp() {}
      virtual void saveOldState() = 0 ;
      virtual void reloadOldState() = 0 ;
    };

    ICAWFMng() {}
    virtual ~ICAWFMng() {}

    virtual void init() = 0 ;
    virtual void init(ICouplingMesh* mesh) = 0 ;

    virtual void initMesh() = 0 ;
    virtual void initMesh(Arcane::ConstArrayView<Int64> uids) = 0 ;

    virtual void initData() = 0 ;

    virtual void start() = 0 ;

    virtual bool isAppLeader() = 0 ;

#ifdef USE_PRECICE_V3
    virtual void updateMesh() = 0 ;
    virtual void updateMesh(ICouplingMesh* mesh) = 0 ;
#endif
    virtual void setAppMeshFilter(Arcane::ConstArrayView<Int64> uids) = 0;
#ifndef USE_PRECICE_V3
    virtual void activateMeshFilter(Arcane::String const& mesh_name,bool value) = 0;
#endif

    virtual void update() = 0 ;

    virtual void finalize() = 0 ;

    virtual void startTimeStep() = 0 ;

    virtual void endTimeStep() = 0 ;

    virtual Real initialTimeStep() = 0 ;

    virtual Real newTimeStep(Real current_dt) = 0 ;

    virtual bool isCouplingOngoing() = 0 ;

    virtual bool validateCurrentTimeStep() = 0 ;

    virtual void setTimeIterStateOp(ITimeIterStateOp* op) = 0 ;

    virtual void setConvergence(bool values) = 0 ;
};

END_ARCGEOSIM_NAMESPACE

