// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Mesh/SurfaceUtils/NotMatchingSurfaceMng/NotMatchingSurfaceMngService.h"

#include "ArcGeoSim/Utils/Utils.h"
#include <map>
#include <list>

#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>
#include <arcane/IParallelMng.h>
#include <arcane/Timer.h>
#include <arcane/utils/OStringStream.h>
#include <arcane/utils/NotImplementedException.h>

#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometry.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/ISurface.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/ContactFamily.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/NotMatchingSurfaceProperty.h"
#include "ArcGeoSim/Mesh/SurfaceUtils/INotMatchingSurfaceMngAccessor.h"

#include <boost/shared_ptr.hpp>

using namespace Arcane;
using namespace ArcGeoSim::Surface ;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_SURFACE_NAMESPACE

class NotMatchingSurfaceMngService;

class NotMatchingSurfaceMngAccessor 
  : public INotMatchingSurfaceMngAccessor {
public:
  //! Constructeur
  NotMatchingSurfaceMngAccessor(NotMatchingSurfaceMngService * p);

  //! Destructeur
  virtual ~NotMatchingSurfaceMngAccessor() { }

public:
  boost::shared_ptr<IContactVariable> & getPropertyVariable(NotMatchingSurfaceProperty::eProperty property);
  INotMatchingSurfaceMng * mng();
  FaceFaceContactFamily *& facefaceContactFamily();
  NodeFaceContactFamily *& nodefaceContactFamily();

public:
  NotMatchingSurfaceMngService * m_parent;
  typedef std::map<NotMatchingSurfaceProperty::eProperty,boost::shared_ptr<IContactVariable> > Variables;
  Variables variables;
};

END_SURFACE_NAMESPACE
END_ARCGEOSIM_NAMESPACE
  
/*---------------------------------------------------------------------------*/

NotMatchingSurfaceMngAccessor::
NotMatchingSurfaceMngAccessor(NotMatchingSurfaceMngService * p) 
  : m_parent(p)
{
  ;
}
 
/*---------------------------------------------------------------------------*/

boost::shared_ptr<IContactVariable> &
NotMatchingSurfaceMngAccessor::
getPropertyVariable(NotMatchingSurfaceProperty::eProperty property) 
{ 
  return variables[property];
}

/*---------------------------------------------------------------------------*/

INotMatchingSurfaceMng * 
NotMatchingSurfaceMngAccessor::
mng() 
{ 
  return m_parent; 
}

/*---------------------------------------------------------------------------*/

FaceFaceContactFamily *& 
NotMatchingSurfaceMngAccessor::
facefaceContactFamily()
{
  return m_parent->m_faceface_family;
}

/*---------------------------------------------------------------------------*/

NodeFaceContactFamily *& 
NotMatchingSurfaceMngAccessor::
nodefaceContactFamily()
{
  return m_parent->m_nodeface_family;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

NotMatchingSurfaceMngService::
NotMatchingSurfaceMngService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneNotMatchingSurfaceMngObject(sbi)
  , m_internal(new NotMatchingSurfaceMngAccessor(this))
  , m_updater(NULL)
  , m_properties(0)
  , m_faceface_family(NULL)
  , m_nodeface_family(NULL)
  , m_is_contact_distribution_computed(false)
{
  ;
}

/*---------------------------------------------------------------------------*/

NotMatchingSurfaceMngService::
~NotMatchingSurfaceMngService() 
{
  delete m_faceface_family;
  delete m_nodeface_family;
  delete m_internal;
}

/*---------------------------------------------------------------------------*/

void 
NotMatchingSurfaceMngService::
init()
{
  if(options()->notMatchingSurfaceCalculator.size()>0)
    {
        m_updater = options()->notMatchingSurfaceCalculator[0];
        m_updater->init();
    }

  m_faceface_family = new FaceFaceContactFamily(traceMng(),mesh()->faceFamily());
  m_nodeface_family = new NodeFaceContactFamily(traceMng(),mesh()->nodeFamily(),mesh()->faceFamily());

  m_is_contact_distribution_computed = options()->computeContactDistribution();
}

/*---------------------------------------------------------------------------*/

void 
NotMatchingSurfaceMngService::
addProperties(Integer property)
{
  ARCANE_ASSERT((NotMatchingSurfaceProperty::check(property)),("Inconsistent property [%d]",property));
  m_properties |= property;
  if(m_updater)
    m_updater->addPropertiesTrigger(property);
}

/*---------------------------------------------------------------------------*/

Integer
NotMatchingSurfaceMngService::
getProperties() const
{
  return m_properties;
}

/*---------------------------------------------------------------------------*/

bool
NotMatchingSurfaceMngService::
hasProperties(Integer property) const
{
  ARCANE_ASSERT((NotMatchingSurfaceProperty::check(property)),("Inconsistent property [%d]",property));
  return ( (m_properties & property) == property );
}

/*---------------------------------------------------------------------------*/

const IContactVariable * 
NotMatchingSurfaceMngService::
getPropertyVariable(enum NotMatchingSurfaceProperty::eProperty property) const
{
  const Internal::Variables & variables = m_internal->variables;
  Internal::Variables::const_iterator finder = variables.find(property);
  if (finder == variables.end())
    return NULL;
  else
    return finder->second.get();
}

/*---------------------------------------------------------------------------*/

FaceFaceContactFamily *
NotMatchingSurfaceMngService::
facefaceContactFamily()
{
  return m_faceface_family;
}

/*---------------------------------------------------------------------------*/

NodeFaceContactFamily *
NotMatchingSurfaceMngService::
nodefaceContactFamily()
{
  return m_nodeface_family;
}

/*---------------------------------------------------------------------------*/

void 
NotMatchingSurfaceMngService::
update()
{
  if(m_updater)
    m_updater->update(m_internal);
  else
    fatal()<<"NotMatchingBuilder has not be set";
  // SDC pour utiliser la distribution des contacts (et y associer des DoFS) il nous faut les contacts fant�mes...
  if (options()->computeContactDistribution()) {
      m_faceface_family->computeSynchronizeInfos();
      m_nodeface_family->computeSynchronizeInfos();
  }
}

/*---------------------------------------------------------------------------*/

INotMatchingSurfaceBuilder * 
NotMatchingSurfaceMngService::
switchBuilder(INotMatchingSurfaceBuilder* updater)
{
  if (updater == m_updater)
    {
      return NULL;
    }
  else
    {
      INotMatchingSurfaceBuilder * old_updater = m_updater;
      m_updater = updater;
      Integer current_property = NotMatchingSurfaceProperty::PEnd;
      while(current_property)
        {
          if (m_properties & current_property)
            m_updater->addPropertiesTrigger(current_property);
          current_property>>=1; // en attendant un enumerateur de propri�t�s
        }
      return old_updater;
    }
}

/*---------------------------------------------------------------------------*/

NotMatchingSurfaceMngService::FaceFaceContactSupportType 
NotMatchingSurfaceMngService::allFaceFaceContacts() const  {
  return m_faceface_family->allContacts() ;
}

/*---------------------------------------------------------------------------*/

NotMatchingSurfaceMngService::FaceSupportType 
NotMatchingSurfaceMngService::allFaceFaceContactFaces() const {
  return m_faceface_family->allFaces() ;
}

/*---------------------------------------------------------------------------*/

bool
NotMatchingSurfaceMngService::
isContactDistributionComputed() const {
  return m_is_contact_distribution_computed;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_NOTMATCHINGSURFACEMNG(NotMatchingSurfaceMng,NotMatchingSurfaceMngService);
