// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
//
// Created by yousefs on 4/29/20.
//
#include "arcane/utils/ScopedPtr.h"
#include "arcane/IArcaneMain.h"
#include "arcane/ISubDomain.h"
#include "arcane/IModuleMng.h"
#include "arcane/IModule.h"
#include "arcane/IModuleMaster.h"
#include "arcane/ServiceUtils.h"
#include "arcane/IFactoryService.h"
#include "arcane/IMeshFactory.h"
#include "arcane/IApplication.h"
#include "arcane/ItemGroup.h"
#include "arcane/ServiceBuilder.h"
#include "arcane/SerializeBuffer.h"

#include "arcane/IMainFactory.h"
#include "arcane/IPrimaryMesh.h"
#include "arcane/Properties.h"
#include <arcane/IMeshModifier.h>
#include "arcane/IMeshReader.h"
#include "arcane/CaseNodeNames.h"
#include "arcane/ICaseDocument.h"
#include "arcane/IItemFamily.h"


#include <vector>

#include "ArcGeoSim/Mesh/PatchGenerator/IPatchGenerator.h"

#include "PatchGenerator_axl.h"

using namespace Arcane;

class PatchGeneratorService
        : public ArcanePatchGeneratorObject
{

public:
    PatchGeneratorService(const Arcane::ServiceBuildInfo & sbi)
      : ArcanePatchGeneratorObject(sbi)
        , m_sd(nullptr)
        , m_mesh(nullptr)
        , m_pm(nullptr)

    {
        ;
    }

    virtual ~PatchGeneratorService();

    SharedArray<Patch*> PatchesList() { return m_patches_list;}

    void init(ISubDomain* sd, IMesh * mesh, IntegerConstArrayView mesh_limiters, IntegerConstArrayView axis_cells_nb);

    void settlePatch(Patch* patch);

    bool checkPatch(Patch* patch, RealConstArrayView flagged_cells_ids);

    Real getEfficiency(Patch* patch, RealConstArrayView flagged_cells_ids);

    IntegerSharedArray computeSignature(Patch* patch, const Integer& axis_id, RealConstArrayView flagged_cells_ids);

    void splitPatch(Patch* patch, const Integer& axis_id, const Integer& cut_index, SharedArray<Patch*>& tmp_patches_list);

    bool findHole(Patch* patch, const Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids);

    bool findInflection(Patch* patch, Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids);

    bool testEfficiency(Patch* patch, const Integer& max_length,const Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids);

    void createPatchesFromCriteria(RealConstArrayView flagged_cells_ids);

    void computeGroups(IItemFamily* current_family,IItemFamily* new_family);

private:

    //! maillage principale
    ISubDomain* m_sd;
    IMesh* m_mesh;
    IParallelMng* m_pm;

    ItemInternalList       m_cellsInternal;

    IntegerSharedArray     m_mesh_limiters;

    IntegerSharedArray     m_axis_cells_nb;

    SharedArray<Patch*>    m_patches_list;

    Integer                m_patch_global_id;

    Integer                m_max_length_allowed;

    Integer                m_min_length_allowed;

    Integer                m_max_elements_allowed;

    Real                   m_efficiency_threshold;

    Real                   m_efficiency_goal;


};


//=======================================================================================
// Destructeur
//=======================================================================================

PatchGeneratorService::
~PatchGeneratorService()
{

}

//==========================================================================================
// initialization
//==========================================================================================
void
PatchGeneratorService ::init(ISubDomain* sd, IMesh * mesh, IntegerConstArrayView mesh_limiters, IntegerConstArrayView axis_cells_nb) {

    m_sd   = sd;
    m_mesh = mesh;
    m_pm   = m_mesh->parallelMng();

    IItemFamily * item_family = m_mesh->cellFamily();
    m_cellsInternal = item_family -> itemsInternal();

    m_mesh_limiters = mesh_limiters;

    m_axis_cells_nb = axis_cells_nb;

    m_patch_global_id = 0;

    m_max_length_allowed   = options()-> maxLength();
    m_min_length_allowed   = options()-> minLength();
    m_max_elements_allowed = options()-> maxElements();
    m_efficiency_threshold = options()-> efficiencyThreshold();
    m_efficiency_goal      = options()-> efficiencyGoal();

}

//==========================================================================================
// check if the pre-patch contain flagged cells
//==========================================================================================
bool
PatchGeneratorService ::checkPatch(Patch* patch, RealConstArrayView flagged_cells_ids){

    bool contain_flagged_cell = false;
    IntegerConstArrayView cells_local_ids = patch -> coarseCellsLocalIds();

    for(Integer i=0; i<cells_local_ids.size(); i++){
        Integer cell_id = cells_local_ids[i];
        if(flagged_cells_ids[cell_id]){
            contain_flagged_cell = true;
	    break;
	}
    }

   return contain_flagged_cell;
    
}

//==========================================================================================
// check if the pre-patch contain flagged cells
//==========================================================================================
Real
PatchGeneratorService ::getEfficiency(Patch* patch, RealConstArrayView flagged_cells_ids){

    Real eff = 0.;
    Integer nb_flagged = 0;
    IntegerConstArrayView cells_local_ids = patch -> coarseCellsLocalIds();

    for(Integer i=0; i<cells_local_ids.size(); i++){
        Integer cell_id = cells_local_ids[i];
        if(flagged_cells_ids[cell_id]){
            nb_flagged++;
        }
    }
    eff = (Real)nb_flagged/(Real)cells_local_ids.size();
   return eff;

}
//==========================================================================================
//
//==========================================================================================

IntegerSharedArray
PatchGeneratorService ::computeSignature(Patch* patch, const Integer& axis_id, RealConstArrayView flagged_cells_ids) {

    IntegerConstArrayView limiters = patch -> limiters();
    Integer ax_id_1 = (axis_id+1)%3;
    Integer ax_id_2 = (axis_id+2)%3;


    Integer ax_limiter = 2 * axis_id;
    Integer limiter_1 = 2 * ax_id_1;
    Integer limiter_2 = 2 * ax_id_2;


    Integer sig_size = limiters[ax_limiter+1] - limiters[ax_limiter];
    IntegerSharedArray signature(sig_size);
    signature.fill(0);
    for (Integer it_ax = limiters[ax_limiter]; it_ax < limiters[ax_limiter+1]; it_ax++) {
        for (Integer it_1 = limiters[limiter_1]; it_1 < limiters[limiter_1+1]; it_1++)
            for (Integer it_2 = limiters[limiter_2]; it_2 < limiters[limiter_2+1]; it_2++) {
                Integer id = it_ax * m_axis_cells_nb[axis_id] + it_1 * m_axis_cells_nb[ax_id_1] + it_2 * m_axis_cells_nb[ax_id_2];
                signature[it_ax-limiters[ax_limiter]] += flagged_cells_ids[id];
            }
    }


    return signature;

}
//==========================================================================================
// split the patch
//==========================================================================================
void
PatchGeneratorService ::splitPatch(Patch* patch, const Integer& axis_id, const Integer& cut_index, SharedArray<Patch*>& tmp_patches_list){

    IntegerConstArrayView limiters = patch -> limiters();
    IntegerSharedArray     left_patch_limiters(limiters);
    IntegerSharedArray     right_patch_limiters(limiters);

    Integer right_ax_id = 2 * axis_id;
    Integer left_ax_id  = 2 * axis_id + 1;

    right_patch_limiters[right_ax_id] = cut_index;
    left_patch_limiters[left_ax_id]   = cut_index;

    Patch* left_patch = new Patch(traceMng(), m_mesh, m_cellsInternal, left_patch_limiters, m_axis_cells_nb);
    //left_patch -> extractCellIds(m_axis_cells_nb);

    Patch* right_patch = new Patch(traceMng(), m_mesh, m_cellsInternal, right_patch_limiters, m_axis_cells_nb);
    //right_patch -> extractCellIds(m_axis_cells_nb);

    tmp_patches_list.add(left_patch);
    tmp_patches_list.add(right_patch);

}


//==========================================================================================
// Finalize the construction of the patch (only used for computation later)
//==========================================================================================
void
PatchGeneratorService ::settlePatch(Patch* patch)
{
    m_patch_global_id ++;
    patch->setGlobalId(m_patch_global_id);
  /* No need !
    warning() << "S.Y. PatchGeneratorService : ATTENTION Group name by patch !?";
    Arcane::CellGroup patch_cells = mesh() -> cellFamily() -> createGroup("PatchesCells") ;
    patch_cells.setItems(cells_local_id) ;
  */
    IntegerConstArrayView cells_local_id = patch ->coarseCellsLocalIds();
    const Int64UniqueArray& cells_unique_id =  patch ->coarseCellsUniqueIds();//Int64 needed by the function itemsUniqueIdToLocalId

    IPrimaryMesh* new_mesh = m_sd->application()->mainFactory()->createMesh(m_sd,m_pm,"PatchMesh"+ std::to_string(m_patch_global_id));
    new_mesh->setDimension(m_mesh->dimension());
    new_mesh->properties()->setBool("compact",false);
    new_mesh->properties()->setBool("sort",false);
    new_mesh->allocateCells(0,Int64ConstArrayView(),false);
    new_mesh->endAllocate();
    new_mesh->destroyGroups();
    new_mesh->modifier()->clearItems();
    new_mesh->modifier()->endUpdate();

    Integer nb_cell_to_copy = cells_local_id.size();
    SerializeBuffer buffer;
    info() << "serialize";
    m_mesh->serializeCells(&buffer,cells_local_id);
    info() << "NB_CELL_TO_SERIALIZE=" << nb_cell_to_copy;
    new_mesh->modifier()->addCells(&buffer);
    new_mesh->modifier()->endUpdate();
    //ItemInternalList new_cells = new_mesh->itemsInternal(IK_Cell);
    //ItemInternalList current_cells = m_mesh->itemsInternal(IK_Cell);
    CellVectorView new_cells = new_mesh->cellFamily()->view();
    CellVectorView current_cells = m_mesh->cellFamily()->view();
    VariableNodeReal3& new_coordinates(new_mesh->nodesCoordinates());
    VariableNodeReal3& current_coordinates(m_mesh->toPrimaryMesh()->nodesCoordinates());
    Int32UniqueArray new_cells_local_id(nb_cell_to_copy);
    new_mesh->cellFamily()->itemsUniqueIdToLocalId(new_cells_local_id,cells_unique_id);
    for( Integer zid=0; zid<nb_cell_to_copy; ++zid ){
        Cell current_cell = current_cells[cells_local_id[zid]];
        Cell new_cell = new_cells[new_cells_local_id[zid]];
        if (current_cell.uniqueId()!=new_cell.uniqueId())
            fatal() << "Inconsistent unique ids";
        Integer nb_node = current_cell.nbNode();
        for( Integer z2=0; z2<nb_node; ++z2 ){
            Real3 coord = current_coordinates[current_cell.node(z2)];
            new_coordinates[new_cell.node(z2)] = coord;
        }
    }
    // Maintenant, il faut recopier les groupes
/*    {
        _computeGroups(current_mesh->nodeFamily(),new_mesh->nodeFamily());
        _computeGroups(current_mesh->edgeFamily(),new_mesh->edgeFamily());
        _computeGroups(current_mesh->faceFamily(),new_mesh->faceFamily());
        _computeGroups(current_mesh->cellFamily(),new_mesh->cellFamily());

    }
*/

    patch->updateCurrentToNewId(new_mesh);



    ICaseDocument* case_doc = m_sd->caseDocument();
    XmlNodeList mesh_elems(case_doc->meshElements());
    XmlNode m_xml_node=mesh_elems[0];

    bool m_is_amr_activated = m_xml_node.attr("amr").valueAsBoolean();
    info() << "Is AMR Activated? = " << m_is_amr_activated;

#if (ARCANE_VERSION < 22200)
    new_mesh ->readAmrActivator(m_xml_node);
#endif

    new_mesh->modifier()->flagCellToRefine(new_cells_local_id);
    bool mesh_has_changed = new_mesh->modifier()->adapt();

    if(mesh_has_changed)
        info() << "refine patch cell nb : "<<(new_mesh-> allActiveCells()).size();

    patch->setMesh(new_mesh);

    // Necessary data to communicate patches with the global mesh
    patch->updateChildToParentFaceIds();




}

//==========================================================================================
// Find hole to cut the patch
//==========================================================================================

bool
PatchGeneratorService ::findHole(Patch* patch, const Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids) {

    cut_index = -1;
    IntegerConstArrayView limiters = patch->limiters();
    Integer ax_limiter = 2 * axis_id;

    IntegerConstArrayView patch_signature = computeSignature(patch, axis_id, flagged_cells_ids);
    Integer len = patch_signature.size();

    IntegerSharedArray holes_index;
    for (Integer i =  m_min_length_allowed-1;
         i <= len - m_min_length_allowed; i++) { // limiters[ax_limiter] -> limiters[ax_limiter + 1]
        if (patch_signature[i] == 0) {        // i-limiters[ax_limiter]
            holes_index.add(i);
            holes_index.add(i+1);    // Attention : To avoid cutting before the last sig =0
       }

    }

    if (!holes_index.empty()) {
        Integer closestHoleToMiddle(holes_index[0]+limiters[ax_limiter]);
        Integer oldDistanceToMiddle(std::abs(holes_index[0] - len / 2));
        Integer newDistanceToMiddle(oldDistanceToMiddle);
        for (Integer i = 0; i < holes_index.size(); i++) {
            newDistanceToMiddle = std::abs(holes_index[i] - len / 2);
            if (newDistanceToMiddle < oldDistanceToMiddle) {
                oldDistanceToMiddle = newDistanceToMiddle;
                closestHoleToMiddle = holes_index[i]+limiters[ax_limiter]; // holes_index[i]
            }
        }
        cut_index = closestHoleToMiddle;
        return true;
    }
    return false;
}
//==========================================================================================
// Find inflection to cut the patch
//==========================================================================================

bool
PatchGeneratorService ::findInflection(Patch* patch, Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids) {

    bool cut_found(false);
    cut_index = -1;
    Integer sign;
    Integer dim = m_mesh->dimension();
    IntegerConstArrayView limiters = patch->limiters();

    std::vector<Integer> best_zero_cross_index(dim, -1);
    std::vector<Integer> best_zero_cross_value(dim, -1);

    for (Integer id_ax = 0; id_ax < dim-1; id_ax++) {

        IntegerSharedArray derivate_second_order, gradient_absolute, zero_cross, max_cross_list;
        std::vector<Integer> edge;
        std::vector<Real> distance;
        IntegerConstArrayView patch_signature = computeSignature(patch, id_ax, flagged_cells_ids);
        Integer len = patch_signature.size();

        //derivate_second_order.add(-1);
        for (Integer i = 1; i < len - 1; i++)
            derivate_second_order.add(patch_signature[i - 1] - 2 * patch_signature[i] + patch_signature[i + 1]);
        //derivate_second_order.add(-1);


        //gradient_absolute.add(abs(derivate_second_order[0]));
        // Gradient absolute value
        for (Integer i = 1; i < derivate_second_order.size(); i++)
            gradient_absolute.add(abs(derivate_second_order[i] - derivate_second_order[i - 1]));
        if (derivate_second_order.empty())
            continue;
        for (Integer i = 1; i < derivate_second_order.size() - 1; i++) {
            if (derivate_second_order[i] * derivate_second_order[i + 1] < 0)
                sign = -1;
            if (derivate_second_order[i] * derivate_second_order[i + 1] > 0)
                sign = 1;
            if (derivate_second_order[i] * derivate_second_order[i + 1] == 0)
                sign = 0;

            if (sign == 0 || sign == -1)
                if (i+2 >= m_min_length_allowed - 2 &&
                    i+2 <= len - m_min_length_allowed - 2) {
                    //info()<<"added i: "<<i<<" , grad : "<<gradient_absolute[i];
                    zero_cross.add(i+2);    // +2 because we do not compute delta on 0 and sig size-1
                    edge.push_back(gradient_absolute[i]);
                }
        }
        if (zero_cross.size() > 0) {
            Integer max_cross = *std::max_element(edge.begin(), edge.end());
            for (std::size_t i = 0; i < edge.size(); i++)
                if (edge[i] == max_cross){
                    max_cross_list.add(zero_cross[i] );//+ 1 (no need because we already added +2)
                }

            Real center((len / 2.0));

            for (Integer i = 0; i < max_cross_list.size(); i++) {
                distance.push_back(fabs(max_cross_list[i]  - center));//+ 1 (no need because we already added +2)
            }

            Real distance_min = *std::min_element(distance.begin(), distance.end());
            Integer pos_distance_min = std::find(distance.begin(), distance.end(), distance_min) - distance.begin();
            Integer best_place = max_cross_list[pos_distance_min] + limiters[2 * id_ax] ;// ax_limiter = 2*id_ax
            if (max_cross >= 0) {
                best_zero_cross_index[id_ax] = best_place;
                best_zero_cross_value[id_ax] = max_cross;
            }
        }
        derivate_second_order.clear();
        gradient_absolute.clear();
        zero_cross.clear();
        edge.clear();
        max_cross_list.clear();
        distance.clear();
    }

    if ( best_zero_cross_index[0]!=-1 || best_zero_cross_index[1]!=-1  )
    {
      Integer max_cross_dims = *std::max_element(best_zero_cross_value.begin(),best_zero_cross_value.end()) ;

      if (best_zero_cross_value[0]==max_cross_dims &&  best_zero_cross_value[1]==max_cross_dims )
        {
          int x_size(limiters[1]-limiters[0]);
          int y_size(limiters[3]-limiters[2]);
          if ( x_size >=  y_size )
            max_cross_dims = 0 ;
          else
            max_cross_dims = 1 ;
        }
      else
        max_cross_dims=std::find(best_zero_cross_value.begin(),best_zero_cross_value.end(),max_cross_dims)-best_zero_cross_value.begin();
      cut_found=true;
      cut_index=best_zero_cross_index[max_cross_dims];
      axis_id=max_cross_dims ;
    }

    return cut_found;

}

//==========================================================================================
// Test to cut the patch
//==========================================================================================

bool
PatchGeneratorService ::testEfficiency(Patch* patch, const Integer& max_length,const Integer& axis_id, Integer& cut_index, RealConstArrayView flagged_cells_ids) {

    Integer limite_id = 2 * axis_id;
    IntegerConstArrayView limiters = patch->limiters();

    if (getEfficiency(patch, flagged_cells_ids) < m_efficiency_goal){
        if(max_length > 2*m_min_length_allowed+1){
            cut_index=max_length/2+limiters[limite_id]-1;
            return true;
        }
        return false;
    }

    if (max_length > m_max_length_allowed || patch->getCellsNumber() > m_max_elements_allowed) {
        cut_index = std::floor((limiters[limite_id] + limiters[limite_id + 1]) / 2.);
        return true;

    }

    return false;


}
//==========================================================================================
// Find and seperate the patches using criteria
//==========================================================================================

void 
PatchGeneratorService :: createPatchesFromCriteria(RealConstArrayView flagged_cells_ids){

    m_patches_list.clear();
    SharedArray<Patch*> patches_list;
    Patch* initial_patch = new Patch(traceMng(), m_mesh, m_cellsInternal, m_mesh_limiters, m_axis_cells_nb);
    //initial_patch -> extractCellIds(m_axis_cells_nb);

    patches_list.add(initial_patch);
    while(!patches_list.empty()) {
        SharedArray<Patch *> tmp_patches_list;
        for (SharedArray<Patch *>::iterator it = patches_list.begin(); it != patches_list.end(); it++) {

            Integer max_length, axis_id, cut_index;

            if (!checkPatch(*it, flagged_cells_ids))
                continue;


            (*it)->computeMaxLength(max_length, axis_id);

            if (getEfficiency(*it, flagged_cells_ids)>m_efficiency_threshold) {
                if (max_length > m_max_length_allowed || (*it)->getCellsNumber() > m_max_elements_allowed) {
                    Integer limite_id = 2 * axis_id;
                    IntegerConstArrayView limiters = (*it)->limiters();
                    cut_index = std::floor((limiters[limite_id] + limiters[limite_id + 1]) / 2.);

                    splitPatch(*it, axis_id, cut_index, tmp_patches_list);
                    continue;
                }
            }
            if (findHole(*it, axis_id, cut_index, flagged_cells_ids)) {
               splitPatch(*it, axis_id, cut_index, tmp_patches_list);
                continue;
            }
            if (findInflection(*it, axis_id, cut_index, flagged_cells_ids)) {
                splitPatch(*it, axis_id, cut_index, tmp_patches_list);
                continue;
            }
            if (testEfficiency(*it, max_length ,axis_id, cut_index, flagged_cells_ids)) {
                splitPatch(*it, axis_id, cut_index, tmp_patches_list);
                continue;
            }else {
                settlePatch(*it);
                m_patches_list.add(*it);
            }
        }
        patches_list = tmp_patches_list;

    }

    for(SharedArray<Patch*>::iterator it=m_patches_list.begin();it!=m_patches_list.end();it++)
	  (*it)-> print();

    
}

void PatchGeneratorService ::
computeGroups(IItemFamily* current_family,IItemFamily* new_family)
{
    info() << "Compute groups family=" << current_family->name();

    ItemGroup new_all_items = new_family->allItems();
    Integer nb_new_item = new_all_items.size();

    Int64UniqueArray new_items_uid(nb_new_item);
    Int32UniqueArray new_items_lid(nb_new_item);
    {
        Integer index = 0;
        ENUMERATE_ITEM(iitem,new_all_items){
            new_items_uid[index] = (*iitem).uniqueId();
            new_items_lid[index] = iitem.itemLocalId();
            ++index;
        }
    }
    Int32UniqueArray items_lid(nb_new_item);
    // Détermine le localId() dans le maillage d'origine des entités
    current_family->itemsUniqueIdToLocalId(items_lid,new_items_uid);

    Int32UniqueArray items_current_to_new_local_id(current_family->maxLocalId());
    items_current_to_new_local_id.fill(NULL_ITEM_LOCAL_ID);
    for( Integer i=0; i<nb_new_item; ++i )
        items_current_to_new_local_id[items_lid[i]] = new_items_lid[i];

    Int32UniqueArray create_local_ids;
    for( ItemGroupCollection::Enumerator igroup(current_family->groups()); ++igroup; ){
        ItemGroup group = *igroup;
        if (group.isOwn())
            continue;
        create_local_ids.clear();
        ENUMERATE_ITEM(iitem,group){
            Int32 current_uid = iitem.itemLocalId();
            Int32 new_lid = items_current_to_new_local_id[current_uid];
            if (new_lid!=NULL_ITEM_LOCAL_ID)
                create_local_ids.add(new_lid);
        }
        new_family->createGroup(group.name(),create_local_ids,true);
    }
}


//==============================================
// L'enregistrement du service
//==============================================
ARCANE_REGISTER_SERVICE_PATCHGENERATOR(PatchGenerator,PatchGeneratorService);
