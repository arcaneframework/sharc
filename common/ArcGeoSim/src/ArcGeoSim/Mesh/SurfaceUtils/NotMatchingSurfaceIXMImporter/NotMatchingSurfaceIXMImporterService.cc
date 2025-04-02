// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Mesh/SurfaceUtils/NotMatchingSurfaceIXMImporter/NotMatchingSurfaceIXMImporterService.h"
#include "ArcGeoSim/Utils/Utils.h"

#include <map>
#include <list>

#include <arcane/IMesh.h>
#if (ARCANE_VERSION<30003)
#include <arcane/IGraph.h>
#endif
#include <arcane/IItemFamily.h>

#include <arcane/ArcaneVersion.h>
#include <arcane/Item.h>
#include <arcane/ItemTypes.h>
#include <arcane/ISubDomain.h>
#include <arcane/IVariable.h>
#include <arcane/IVariableMng.h>
#include <arcane/datatype/DataTypes.h>
#include "arcane/VariableTypes.h"

#include "ArcGeoSim/Mesh/SurfaceUtils/INotMatchingSurfaceMngAccessor.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/INotMatchingSurfaceMng.h"

#include "ArcGeoSim/Mesh/Utils/MeshUtils.h"

using namespace Arcane;
using namespace ArcGeoSim::Surface ;

/*---------------------------------------------------------------------------*/

NotMatchingSurfaceIXMImporterService::
NotMatchingSurfaceIXMImporterService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneNotMatchingSurfaceIXMImporterObject(sbi)
{
  m_output_level = 0;
  m_is_initialized = false;
}

/*---------------------------------------------------------------------------*/

NotMatchingSurfaceIXMImporterService::
~NotMatchingSurfaceIXMImporterService()
{
  ;
}

/*---------------------------------------------------------------------------*/

void
NotMatchingSurfaceIXMImporterService::
init()
{
  m_output_level = options()->outputLevel() ;
  m_is_initialized = true ;
}

/*---------------------------------------------------------------------------*/

void
NotMatchingSurfaceIXMImporterService::
addPropertiesTrigger(Integer property)
{
  ARCANE_ASSERT((NotMatchingSurfaceProperty::check(property)),("Inconsistent property [%d]",property));
}

/*---------------------------------------------------------------------------*/

void
NotMatchingSurfaceIXMImporterService::
update(INotMatchingSurfaceMngAccessor * access)
{
  IMesh* mesh = subDomain()->defaultMesh() ;
#if (ARCANE_VERSION < 30003)
#if (ARCANE_VERSION<11602)
  IItemFamily* family = subDomain()->defaultGraph()->linkFamily() ;
#else
  IItemFamily* family = mesh->graph()->linkFamily() ;
#endif
  String name("FaceFaceConnections") ;
  LinkGroup contact_link_group = family->findGroup(name) ;
  //LinkGroup contact_link_group = m_subdomain->defaultGraph()->allLinks() ;
  if(contact_link_group.null())
  {
    info()<<"The Not Matching Surface is empty";
    return;
  }
  FaceFaceContactFamily *& face_face_family = access->facefaceContactFamily() ;
  ENUMERATE_LINK(ilink,contact_link_group)
  {
     const Link& link = *ilink ;
     DualNodeEnumerator inode(link.dualNodes());
     const Face face1 = (*inode).dualItem().toFace() ;
     ++inode ;
     const Face face2 = (*inode).dualItem().toFace() ;
     face_face_family->addContact(face1,face2) ;
  }
  face_face_family->endUpdate() ;

  typedef ContactVariableT<FaceFaceContactFamily::ContactType,Real3> ContactVariableReal3 ;
  typedef ContactVariableT<FaceFaceContactFamily::ContactType,Real> ContactVariableReal ;

  ContactVariableReal3& center_var = *new ContactVariableReal3(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementCenter).reset(&center_var);
  PartialVariableLinkReal3 center_coordinates(VariableBuildInfo(mesh,"Geom_Intersection_Center_Coordinates",contact_link_group.name(),IVariable::PNoRestore));

  ContactVariableReal3& normal_var = *new ContactVariableReal3(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementNormal).reset(&normal_var);
  PartialVariableLinkReal3 intersection_normal(VariableBuildInfo(mesh,"Geom_Intersection_Normal",contact_link_group.name(),IVariable::PNoRestore));

  ContactVariableReal& vol_var = *new ContactVariableReal(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementArea).reset(&vol_var);
  PartialVariableLinkReal intersection_measure(VariableBuildInfo(mesh,"Geom_Intersection_Measure",contact_link_group.name(),IVariable::PNoRestore));

  //On suppose que iter reste dans l'ordre de declaration des contacts
  FaceFaceContactEnumerator iter((face_face_family->allContacts()).enumerator());
  ENUMERATE_LINK(ilink,contact_link_group)
  {
    vol_var[iter] = intersection_measure[ilink];
    normal_var[iter] = intersection_normal[ilink];
    center_var[iter] = center_coordinates[ilink];
    ++iter ;
  }
#else

  auto graph = GRAPH(mesh) ;
  if(!graph->isUpdated())
    graph->modifier()->endUpdate();

  IItemFamily* family = graph->linkFamily() ;

  String name("FaceFaceConnections") ;
  DoFGroup contact_link_group = family->findGroup(name) ;
  //LinkGroup contact_link_group = m_subdomain->defaultGraph()->allLinks() ;
  if(contact_link_group.null())
  {
    info()<<"The Not Matching Surface is empty";
    return;
  }
  FaceFaceContactFamily *& face_face_family = access->facefaceContactFamily() ;


  auto graph_connectivity = graph->connectivity() ;


  ENUMERATE_DOF(ilink,contact_link_group)
  {
     const DoF& link = *ilink ;
     auto dual_nodes = graph_connectivity->dualNodes(link) ;
     const Face face1 = graph_connectivity->dualItem(dual_nodes[0]).toFace() ;
     const Face face2 = graph_connectivity->dualItem(dual_nodes[1]).toFace() ;
     face_face_family->addContact(face1,face2) ;
  }
  face_face_family->endUpdate() ;

  typedef ContactVariableT<FaceFaceContactFamily::ContactType,Real3> ContactVariableReal3 ;
  typedef ContactVariableT<FaceFaceContactFamily::ContactType,Real> ContactVariableReal ;

  ContactVariableReal3& center_var = *new ContactVariableReal3(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementCenter).reset(&center_var);
  PartialVariableDoFReal3 center_coordinates(VariableBuildInfo(mesh,"Geom_Intersection_Center_Coordinates",
                                                               family->name(),contact_link_group.name(),IVariable::PNoRestore));

  ContactVariableReal3& normal_var = *new ContactVariableReal3(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementNormal).reset(&normal_var);
  PartialVariableDoFReal3 intersection_normal(VariableBuildInfo(mesh,"Geom_Intersection_Normal",
                                                                family->name(),contact_link_group.name(),IVariable::PNoRestore));

  ContactVariableReal& vol_var = *new ContactVariableReal(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementArea).reset(&vol_var);
  PartialVariableDoFReal intersection_measure(VariableBuildInfo(mesh,"Geom_Intersection_Measure",
                                                                family->name(),contact_link_group.name(),IVariable::PNoRestore));

  //On suppose que iter reste dans l'ordre de declaration des contacts
  FaceFaceContactEnumerator iter((face_face_family->allContacts()).enumerator());
  ENUMERATE_DOF(ilink,contact_link_group)
  {
    vol_var[iter] = intersection_measure[ilink];
    normal_var[iter] = intersection_normal[ilink];
    center_var[iter] = center_coordinates[ilink];
    ++iter ;
  }
#endif
  INotMatchingSurfaceMng * mng = access->mng();
  {
    FaceFaceContactGroup allFaceFaceContacts = mng->facefaceContactFamily()->allContacts();

    info()<<"---------------------------------------------";
    info()<<"| Not Matching Surface Info :                ";
    info()<<"| Number of contact objects : "<<allFaceFaceContacts.size();
    info()<<"---------------------------------------------";
    info();
    if(m_output_level>1)
      {
        ENUMERATE_FACEFACECONTACT(iter,allFaceFaceContacts)
          {
            const FaceFaceContactFamily::ContactType & contact = *iter ;
            const Cell& iCell = contact.face1().boundaryCell() ;
            const Cell& jCell = contact.face2().boundaryCell() ;
            info()<<"Contact : Face A : "<<contact.face1().uniqueId();
            info()<<"        : Face B : "<<contact.face2().uniqueId();
            info()<<"        : Cell A : "<<iCell.uniqueId();
            info()<<"        : Cell B : "<<jCell.uniqueId();
          }
      }
  }
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_NOTMATCHINGSURFACEIXMIMPORTER(NotMatchingSurfaceIXMImporter,NotMatchingSurfaceIXMImporterService);
