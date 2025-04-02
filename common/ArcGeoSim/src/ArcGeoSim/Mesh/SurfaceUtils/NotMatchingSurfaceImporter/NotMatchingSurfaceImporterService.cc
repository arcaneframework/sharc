// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Mesh/SurfaceUtils/NotMatchingSurfaceImporter/NotMatchingSurfaceImporterService.h"

#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Mesh/Utils/MeshUtils.h"

#include <map>
#include <list>

#include <arcane/IMesh.h>
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

using namespace Arcane;
using namespace ArcGeoSim::Surface ;

/*---------------------------------------------------------------------------*/

NotMatchingSurfaceImporterService::
NotMatchingSurfaceImporterService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneNotMatchingSurfaceImporterObject(sbi)
{
  m_output_level = 0;
  m_is_initialized = false;
}

/*---------------------------------------------------------------------------*/

NotMatchingSurfaceImporterService::
~NotMatchingSurfaceImporterService()
{
  ;
}

/*---------------------------------------------------------------------------*/

void
NotMatchingSurfaceImporterService::
init()
{
  m_output_level = options()->outputLevel() ;
  m_is_initialized = true ;
}

/*---------------------------------------------------------------------------*/

void
NotMatchingSurfaceImporterService::
addPropertiesTrigger(Integer property)
{
  ARCANE_ASSERT((NotMatchingSurfaceProperty::check(property)),("Inconsistent property [%d]",property));
}

/*---------------------------------------------------------------------------*/

void
NotMatchingSurfaceImporterService::
update(INotMatchingSurfaceMngAccessor * access)
{
  IMesh* mesh = subDomain()->defaultMesh() ;

#ifdef USE_ARCANE_V3
  auto graph = ArcGeoSim::Mesh::GraphMng::graph(mesh,true);
#else
  auto graph = GRAPH(mesh) ;
#endif
  IItemFamily* family = GRAPH(mesh)->linkFamily() ;
#ifdef USE_ARCANE_V3
  m_connectivity = graph->connectivity() ;
#endif
  String name("ContactLinkGroup") ;
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
     DualNodeEnumerator inode(DUALNODES_ENUM(link));
     const Face face1 = DUALITEM(*inode).toFace() ;
     ++inode ;
     const Face face2 = DUALITEM(*inode).toFace() ;
     face_face_family->addContact(face1,face2) ;
  }
  face_face_family->endUpdate() ;

  typedef ContactVariableT<FaceFaceContactFamily::ContactType,Real3> ContactVariableReal3 ;
  typedef ContactVariableT<FaceFaceContactFamily::ContactType,Real> ContactVariableReal ;

  ContactVariableReal3& center_var = *new ContactVariableReal3(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementCenter).reset(&center_var);

  ContactVariableReal3& normal_var = *new ContactVariableReal3(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementNormal).reset(&normal_var);

  ContactVariableReal& vol_var = *new ContactVariableReal(face_face_family);
  access->getPropertyVariable(NotMatchingSurfaceProperty::PSimpleCorefinementArea).reset(&vol_var);
#ifdef USE_ARCANE_V3
  VariableLinkArrayReal contact_link_properties(VariableBuildInfo(mesh,"MshContactLinkProperties","Links",IVariable::PNoRestore));
#else
  VariableLinkArrayReal contact_link_properties(VariableBuildInfo(mesh,"MshContactLinkProperties",IVariable::PNoRestore));
#endif
  enum  eProperties { Vol, Cx, Cy, Cz, Nx, Ny, Nz, NbProperties };

  //On suppose que iter reste dans l'ordre de declaration des contacts
  FaceFaceContactEnumerator iter((face_face_family->allContacts()).enumerator());
  ENUMERATE_LINK(ilink,contact_link_group)
  {
    ArrayView<Real> props = contact_link_properties[ilink] ;
    Real vol = props[Vol] ;
    Real cx = props[Cx] ;
    Real cy = props[Cy] ;
    Real cz = props[Cz] ;
    Real nx = props[Nx] ;
    Real ny = props[Ny] ;
    Real nz = props[Nz] ;
    vol_var[iter] = Real(vol);
    normal_var[iter] = Real3(nx,ny,nz) ;
    center_var[iter] = Real3(cx,cy,cz) ;
    ++iter ;
  }
  //contact_link_properties.resize(0) ;

  INotMatchingSurfaceMng * mng = access->mng();

  //if(m_output_level>0)
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

ARCANE_REGISTER_SERVICE_NOTMATCHINGSURFACEIMPORTER(NotMatchingSurfaceImporter,NotMatchingSurfaceImporterService);
