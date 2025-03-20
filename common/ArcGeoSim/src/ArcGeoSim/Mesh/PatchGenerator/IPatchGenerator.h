// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
//
// Created by yousefs on 5/8/20.
//


#ifndef ARCGEOSIM_MESH_PATCHGENERATOR_IPATCHGENERATOR_H
#define ARCGEOSIM_MESH_PATCHGENERATOR_IPATCHGENERATOR_H


#include "Patch.h"



using namespace Arcane;

class IPatchGenerator
{

public:

    //! constructeur de la classe
    IPatchGenerator() {};
    //! destructeur de la classe
    virtual ~IPatchGenerator() {};


public:

    virtual SharedArray<Patch*> PatchesList() = 0;

    //! initialisation
    virtual void init(ISubDomain* sd, IMesh * mesh, IntegerConstArrayView mesh_limiters, IntegerConstArrayView axis_cells_nb) = 0;

    virtual void settlePatch(Patch* patch) = 0;

    virtual bool checkPatch(Patch* patch, RealConstArrayView flagged_cells_ids) = 0;

    virtual Real getEfficiency(Patch* patch, RealConstArrayView flagged_cells_ids) = 0;

    virtual IntegerSharedArray computeSignature(Patch* patch, const Integer& axis_id, RealConstArrayView flagged_cells_ids) = 0;

    virtual void splitPatch(Patch* patch, const Integer& axis_id, const Integer& cut_index, SharedArray<Patch*>& tmp_patches_list) = 0;

    virtual bool findHole(Patch* patch, const Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids) = 0;

    virtual bool findInflection(Patch* patch, Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids) = 0;

    virtual bool testEfficiency(Patch* patch, const Integer& max_length,const Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids) = 0;

    virtual void createPatchesFromCriteria(RealConstArrayView flagged_cells_ids) = 0;

    virtual void computeGroups(IItemFamily* current_family,IItemFamily* new_family) = 0;

};

#endif //ARCANEDEMO_IPATCHGENERATOR_H
