// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
//
// Created by yousefs on 4/29/20.
//

#ifndef ARCANEDEMO_PATCH_H
#define ARCANEDEMO_PATCH_H


#include <arcane/ArcaneVersion.h>
#include "arcane/VariableTypes.h"
#include <arcane/IMesh.h>
#include <arcane/IParallelMng.h>
#include <arcane/utils/ITraceMng.h>


#include "arcane/IItemFamily.h"
#include <arcane/mesh/NodeFamily.h>
#include <arcane/mesh/FaceFamily.h>



/*----------------------------------------------------------------------------*/
using namespace Arcane;

class Patch
{

public :

    class Modifier
    {
    public:

        Modifier(Patch& patch)
        : m_patch(patch) {}

        //void createPatch(Arcane::CellGroup coarse_cells);

    private:

      Patch& m_patch;

    };


   Patch(ITraceMng * trace_mng,
           Arcane::IMesh* coarse_mesh,
           const ItemInternalList& coarse_cellsInternal,
           IntegerConstArrayView limiters,
           IntegerConstArrayView axis_cells_nb)
     : m_trace_mng(trace_mng)
        , m_coarse_mesh(coarse_mesh)
        , m_mesh(nullptr)
        , m_coarse_cellsInternal(coarse_cellsInternal)
        , m_limiters(limiters)
    {
      if(m_coarse_mesh == nullptr)
           m_trace_mng->error()<<"Patch constructor : Coarse mesh not defined !";


      _extractCellIds(axis_cells_nb);
      m_global_id = 0; // global id is defined when patch is settled
    }

    virtual ~Patch() {}

 
    void setMesh(Arcane::IMesh* mesh)
    {
        m_mesh = mesh;
    }

    static Patch::Modifier modifier();

    Arcane::IMesh* mesh() const { return m_mesh; }

    const IntegerConstArrayView limiters(){
        return m_limiters;
    }

    const IntegerConstArrayView coarseCellsLocalIds(){
        return m_coarse_cells_local_ids;
    }

    const Int64UniqueArray coarseCellsUniqueIds(){ //because of the need of the function itemsUniqueIdToLocalId
        return m_coarse_cells_unique_ids;
    }

    const Cell coarseCell(const Integer& id){
        return m_coarse_cellsInternal[id];
    }

    void computeMaxLength(Integer & max_length, Integer & axis_id);

    void updateCurrentToNewId(IMesh* new_mesh);

    void updateCommunicationData();

    void updateChildToParentFaceIds();

    Integer getCellsNumber();

    void setGlobalId(const Integer& id){
        m_global_id = id;
    }

    const Integer globalId(){
        return m_global_id;
    }

    Int32UniqueArray facesChildToCurrentLId() const{
        return m_faces_child_to_parent_local_id;
    }

    Int32UniqueArray cellsNewToCurrentLId() const{
        return m_cells_new_to_current_local_id;
    }

    Int32UniqueArray cellsCurrentToNewLId() const {
        return m_cells_current_to_new_local_id;
    }

    void print();

protected:

    void _extractCellIds(IntegerConstArrayView axis_cells_nb);


public:

    ITraceMng*             m_trace_mng;

    Arcane::IMesh*         m_coarse_mesh;

    Arcane::IMesh*         m_mesh;

    ItemInternalList       m_coarse_cellsInternal;

    // x_min, x_max, y_min, y_max, z_min, z_max
    IntegerSharedArray     m_limiters; // size is known in advance, std::array ?

    Int32UniqueArray       m_cells_current_to_new_local_id;

    Int32UniqueArray       m_cells_new_to_current_local_id;

    Int32UniqueArray       m_faces_current_to_new_local_id;

    Int32UniqueArray       m_faces_new_to_current_local_id;

    Int32UniqueArray       m_faces_child_to_parent_local_id;

    IntegerUniqueArray     m_coarse_cells_local_ids; //maybe IntegerSharedArray ?

    Int64UniqueArray       m_coarse_cells_unique_ids;// Int64 needed by the function itemsUniqueIdToLocalId

    Integer                m_coarse_cells_nb;

    Integer                m_global_id;

    friend class Modifier;



};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


#endif //ARCANEDEMO_PATCH_H
