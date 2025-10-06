// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * Patch.cc
 *
 *  Created on: Mar 6, 2020
 *      Author: yousefs
 */


#include "Patch.h"

/*----------------------------------------------------------------------------*/

void Patch::_extractCellIds(IntegerConstArrayView axis_cells_nb)
{
  // Only for cartesien grid !
  // S.Y. : maybe it is better to move it to PatchGenratorService to avoid the copy of the cell group
    Integer coarse_cells_nb = 0;
    for(Integer k = m_limiters[4]; k < m_limiters[5]; k++) //  z direction
        for(Integer j = m_limiters[2]; j < m_limiters[3]; j++) //  y direction
            for(Integer i = m_limiters[0]; i < m_limiters[1]; i++) //  x direction
            {
                Integer id = k * axis_cells_nb[2] + j * axis_cells_nb[1] + i * axis_cells_nb[0];
                // S.Y : axis_cells_nb[0]=1; // in order to respect the id numerotation in Arcane
                const Cell & coarse_cell = m_coarse_cellsInternal[id];
                // if: to add
                m_coarse_cells_local_ids.add(coarse_cell.localId());
                m_coarse_cells_unique_ids.add(coarse_cell.uniqueId());

                coarse_cells_nb ++;
            }
    m_coarse_cells_nb = coarse_cells_nb;
}

/*----------------------------------------------------------------------------*/

void Patch::computeMaxLength(Integer & max_length, Integer & axis_id){


    max_length = m_limiters[5]-m_limiters[4];
    axis_id = 2;
    Integer len_y = m_limiters[3]-m_limiters[2];

    //max_length = std::max(max_length, len_y);
    if(max_length < len_y){
        max_length = len_y;
        axis_id = 1;
    }
    Integer len_x = m_limiters[1]-m_limiters[0];
    if(max_length < len_x){
        max_length = len_x;
        axis_id = 0;
    }

}

/*----------------------------------------------------------------------------*/

Integer Patch::getCellsNumber(){

    return m_coarse_cells_nb;

}

/*----------------------------------------------------------------------------*/

void Patch::updateCurrentToNewId(IMesh * new_mesh){

    IItemFamily * new_cell_family = new_mesh->cellFamily();
    IItemFamily * coarse_cell_family = m_coarse_mesh->cellFamily();

    ItemGroup new_all_cells = new_cell_family->allItems();
    Integer nb_new_cells = new_all_cells.size();

    Int64UniqueArray new_cells_uid(nb_new_cells);
    Int32UniqueArray new_cells_lid(nb_new_cells);
    {
        Integer index = 0;
        ENUMERATE_CELL(icell,new_all_cells){
            new_cells_uid[index] = (*icell).uniqueId();
            new_cells_lid[index] = (*icell).localId();
            ++index;
        }
    }
    Int32UniqueArray cells_lid(nb_new_cells);
// Détermine le localId() dans le maillage d'origine des entités
    coarse_cell_family->itemsUniqueIdToLocalId(cells_lid,new_cells_uid);

    m_cells_current_to_new_local_id.resize(coarse_cell_family->maxLocalId());
    m_cells_current_to_new_local_id.fill(NULL_ITEM_LOCAL_ID);

    m_cells_new_to_current_local_id.resize(new_cell_family->maxLocalId());
    m_cells_new_to_current_local_id.fill(NULL_ITEM_LOCAL_ID);
    for( Integer i=0; i<nb_new_cells; ++i ) {
        m_cells_current_to_new_local_id[cells_lid[i]] = new_cells_lid[i];
        m_cells_new_to_current_local_id[new_cells_lid[i]] = cells_lid[i];
    }


    IItemFamily * new_face_family = new_mesh->faceFamily();
    IItemFamily * coarse_face_family = m_coarse_mesh->faceFamily();

    ItemGroup new_all_faces = new_face_family->allItems();
    Integer nb_new_faces = new_all_faces.size();

    Int64UniqueArray new_faces_uid(nb_new_faces);
    Int32UniqueArray new_faces_lid(nb_new_faces);
    {
        Integer index = 0;
        ENUMERATE_FACE(iface,new_all_faces){
            new_faces_uid[index] = (*iface).uniqueId();
            new_faces_lid[index] = (*iface).localId();
            ++index;
        }
    }
    Int32UniqueArray faces_lid(nb_new_faces);
// Détermine le localId() dans le maillage d'origine des entités
    coarse_face_family->itemsUniqueIdToLocalId(faces_lid,new_faces_uid);

    m_faces_current_to_new_local_id.resize(coarse_face_family->maxLocalId());
    m_faces_current_to_new_local_id.fill(NULL_ITEM_LOCAL_ID);

    m_faces_new_to_current_local_id.resize(new_face_family->maxLocalId());
    m_faces_new_to_current_local_id.fill(NULL_ITEM_LOCAL_ID);
    for( Integer i=0; i<nb_new_faces; ++i ) {
        m_faces_current_to_new_local_id[faces_lid[i]] = new_faces_lid[i];
        m_faces_new_to_current_local_id[new_faces_lid[i]] = faces_lid[i];
    }

};

/*----------------------------------------------------------------------------*/

void Patch::updateCommunicationData(){
    /*(CellVectorView new_cells, CellVectorView global_cells,
    FaceVectorView new_faces, FaceVectorView global_faces){*/

    /* ********
    IItemFamily * item_family = m_mesh->cellFamily();
    m_new_coarse_cells = item_family -> itemsInternal();

    IItemFamily * coarse_item_family = m_coarse_mesh->cellFamily();
    m_global_cells = coarse_item_family -> itemsInternal();
    */
    /*
    m_new_coarse_cells = m_mesh->cellFamily()->view();
    m_global_cells = m_coarse_mesh->cellFamily()->view();

    m_new_coarse_cells_local_id.resize(coarse_cells_local_ids.size());

    m_mesh->cellFamily()->itemsUniqueIdToLocalId(m_new_coarse_cells_local_id,coarse_cells_unique_ids);
    */
     /*  **********
    IItemFamily * item_family_face = m_mesh->faceFamily();
    m_new_coarse_faces = item_family_face -> itemsInternal();

    IItemFamily * coarse_item_family_face = m_coarse_mesh->faceFamily();
    m_global_faces = coarse_item_family_face -> itemsInternal();
    */
     /*
    m_new_coarse_faces = m_mesh->faceFamily()->view();
    m_global_faces = m_coarse_mesh->faceFamily()->view();

    Int64UniqueArray coarse_faces_unique_ids(m_new_coarse_faces.size()); // Int64SharedArray
    m_coarse_faces_local_ids.resize(m_new_coarse_faces.size());
    m_coarse_faces_local_ids.fill(0);
    ENUMERATE_FACE(i_face, m_new_coarse_faces)
    {
          coarse_faces_unique_ids[i_face.index()] = (*i_face).uniqueId();
          m_coarse_faces_local_ids[i_face.index()] = (*i_face).localId();

    }

    m_new_coarse_faces_local_id.resize(coarse_faces_unique_ids.size());

    m_mesh->faceFamily()->itemsUniqueIdToLocalId(m_new_coarse_faces_local_id,coarse_faces_unique_ids);
    */
}

/*----------------------------------------------------------------------------*/

void Patch::updateChildToParentFaceIds() {

    FaceVectorView current_faces = m_coarse_mesh->faceFamily()->view();
    mesh::FaceFamily *new_face_family = dynamic_cast<mesh::FaceFamily *> (m_mesh->faceFamily());
    m_faces_child_to_parent_local_id.resize(new_face_family->maxLocalId());
    m_faces_child_to_parent_local_id.fill(NULL_ITEM_LOCAL_ID);
    ENUMERATE_FACE(iface, m_mesh->allFaces()) {
        const Arcane::Face &face = *iface;
        //bool isActive = (face.boundaryCell().isActive()) || (face.backCell().isActive() && face.frontCell().isActive());
        // m_trace_mng->info()<<"is active : " <<isActive;
       //if(!isActive)
       {
            SharedArray<ItemInternal *> subfaces;
            Integer face_id = face.localId();

            new_face_family->subFaces(face.internal(), subfaces);
            if(subfaces.size()>0){
                Integer coarse_face_id = current_faces[m_faces_new_to_current_local_id[face_id]].localId();
                for (Integer j = 0; j < subfaces.size(); j++) {
                    const Arcane::Face &sub_face = subfaces[j];
                    m_faces_child_to_parent_local_id[sub_face.localId()] = coarse_face_id;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------*/

void Patch::print(){

    m_trace_mng->info()<<"========================================================= \n";
    m_trace_mng->info()<<"             Print Patch info \n";
    m_trace_mng->info()<<"========================================================= \n";
    m_trace_mng->info()<<m_limiters[0]<<" - "<<m_limiters[1]<<" - "<<m_limiters[2]<<" - "
           <<m_limiters[3]<<" - "<<m_limiters[4]<<" - "<<m_limiters[5]<<"\n";

    m_trace_mng->info()<<"========================================================= \n";
/*
    CellVectorView new_cells = m_mesh->cellFamily()->view();
    CellVectorView current_cells = m_coarse_mesh->cellFamily()->view();

    for(Integer i=0; i<m_coarse_cells_local_ids.size(); i++){
        Integer current_id = m_coarse_cells_local_ids[i];

        Integer cell_id = m_cells_current_to_new_local_id[current_id];

        m_trace_mng->info()<<"Coarse cells :: local id : "<<cell_id;

        Cell new_cell = new_cells[cell_id];
        Cell current_cell = current_cells[current_id];
        m_trace_mng->info()<<"new cells :: local id : "<<new_cell.localId() <<" : unique "<< new_cell.uniqueId();
        m_trace_mng->info()<<"current cells :: local id : "<<current_cell.localId() <<" : unique "<< current_cell.uniqueId();


    }
*//*
    CellVectorView current_cells = m_coarse_mesh->cellFamily()->view();
    ENUMERATE_CELL(icell, m_mesh-> allActiveCells()){
        m_trace_mng->info()<<"Patch cells ::  local id : "<<icell->localId()<<", unique id : "<<icell->uniqueId();
        //Integer cell_parent_id = icell->parent()->localId();
        //m_trace_mng->info()<<"Patch parent cell ::  local id : "<<icell->parent()->localId()<<", unique id : "<<icell->parent()->uniqueId();

        Cell cell_parent = icell ->hParent().toCell();
        Cell current_cell = current_cells[m_cells_new_to_current_local_id[cell_parent.localId()]];

        m_trace_mng->info()<<"Patch parent cells ::  local id : "<<cell_parent.localId()<<", unique id : "<<cell_parent.uniqueId();
        m_trace_mng->info()<<"Patch current cells ::  local id : "<<current_cell.localId()<<", unique id : "<<current_cell.uniqueId();


    }

    FaceVectorView current_faces = m_coarse_mesh->faceFamily()->view();
    ENUMERATE_FACE(iface, m_mesh-> outerActiveFaces()){
        m_trace_mng->info()<<"Patch outer face ::  local id : "<<iface->localId()<<", unique id : "<<iface->uniqueId();

        Integer face_parent_id = m_faces_child_to_parent_local_id[iface->localId()];

        Face current_face = current_faces[face_parent_id];
         m_trace_mng->info()<<"Mesh outer face ::  local id : "<<current_face.localId()<<", unique id : "<< current_face.uniqueId();
   }*/

}



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


