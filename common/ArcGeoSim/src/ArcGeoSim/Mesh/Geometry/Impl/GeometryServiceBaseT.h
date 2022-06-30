// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYSERVICEBASET_H
#define ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYSERVICEBASET_H

#include "ArcGeoSim/Mesh/Geometry/Impl/GeometryTemplatesT.h"

template<typename GeometryT>
void 
GeometryServiceBase::
updateGroup(ItemGroup group, GeometryT & geometry) 
{
  PropertyMap::iterator igroup = m_group_property_map.find(group.internal());
  if (igroup == m_group_property_map.end())
    throw FatalErrorException(A_FUNCINFO,"Undefined ItemGroup property");

  bool need_recompute = false;
  ItemGroupGeometryProperty & properties = igroup->second;
  for(typename ItemGroupGeometryProperty::StorageInfos::iterator i = properties.storages.begin(); i != properties.storages.end(); ++i) 
    {
      IGeometryProperty::eProperty property = i->first;
      ItemGroupGeometryProperty::StorageInfo & storage = i->second;

      if (!checkNeedUpdate(group,property,storage))
        {
          if (group.isAllItems())
            traceMng()->debug(Trace::High) << "Property " << IGeometryProperty::name(property) << " is already computed for group " << group.name() << " / " << group.mesh()->name();
          else
            traceMng()->debug() << "Property " << IGeometryProperty::name(property) << " is already computed for group " << group.name() << " / " << group.mesh()->name();
          continue; // skip that property
        }
      else        
        {
          traceMng()->debug() << "Property " << IGeometryProperty::name(property) << " will be computed for group " << group.name() << " / " << group.mesh()->name();
          _checkItemGroupPropertyUsage(group, property, IGeometryProperty::PVariable, storage.usageVarCount, storage.externStorage, storage.storageType);
          _checkItemGroupPropertyUsage(group, property, IGeometryProperty::PItemGroupMap, storage.usageMapCount, storage.externStorage, storage.storageType);
          need_recompute = true;
        }

      // Mise en place des allocations
      if (IGeometryProperty::isScalar(property)) 
        {
          std::shared_ptr<RealVariable> & ivar = storage.realVar;
          if (not ivar and (storage.storageType & IGeometryProperty::PVariable))
            {
              ivar = buildPropertyVariableT<Real>(group, property);
            }

          std::shared_ptr<RealGroupMap> & imap = storage.realMap;
          if (not imap and (storage.storageType & IGeometryProperty::PItemGroupMap))
            {
              imap.reset(new RealGroupMap(group));
              traceMng()->debug() << "Building Map " << imap->name() << " on " << group.fullName();
            }
        }
      else
        {
          ARCANE_ASSERT((IGeometryProperty::isVectorial(property)),("Vectorial property expected"));
          std::shared_ptr<Real3Variable> & ivar = storage.real3Var;
          if (not ivar and (storage.storageType & IGeometryProperty::PVariable))
            {
              ivar = buildPropertyVariableT<Real3>(group, property);
            }

          std::shared_ptr<Real3GroupMap> & imap = storage.real3Map;
          if (not imap and (storage.storageType & IGeometryProperty::PItemGroupMap))
            {
              imap.reset(new Real3GroupMap(group));
              traceMng()->debug() << "Building Map " << imap->name() << " on " << group.fullName();
            }
        }
    }

  if (need_recompute) 
    {
      GenericGSInternalUpdater<GeometryT> updater(geometry, 
                                                  traceMng(),
                                                  &properties,
                                                  this);
      group.applyOperation(&updater);
    }
  
  /*** L'affectation generale est desormais faite dans GeometryTemplatesT ***/
}


template<typename UniqGeometryMap, typename SharedGeometryMap>
void 
GeometryServiceBase::
unregisterMesh(UniqGeometryMap & uniq_geometry, SharedGeometryMap & shared_geometry, IMesh * mesh)
{
  const bool isUniqMesh = (mesh->parentMesh() == NULL);
  
  if (isUniqMesh)
    {
      typename UniqGeometryMap::iterator finder = uniq_geometry.find(mesh);
      if (finder != uniq_geometry.end()) 
        {
          delete finder->second;
        }
      uniq_geometry.erase(finder);

      for(PropertyMap::iterator i = m_group_property_map.begin(); i != m_group_property_map.end(); /* incr inside the loop */)
        {
          PropertyMap::iterator candidate_to_deletion = i++; // incr of main loop
          if (candidate_to_deletion->first->mesh() == mesh)
            m_group_property_map.erase(candidate_to_deletion);
        }
    }
  else
    {
      typename SharedGeometryMap::iterator finder = shared_geometry.find(mesh);
      if (finder != shared_geometry.end()) 
        {
          delete finder->second.first;
          delete finder->second.second;
        }
      shared_geometry.erase(finder);

      for(PropertyMap::iterator i = m_group_property_map.begin(); i != m_group_property_map.end(); /* incr inside the loop */)
        {
          PropertyMap::iterator candidate_to_deletion = i++; // incr of main loop
          if (candidate_to_deletion->first->mesh() == mesh)
            m_group_property_map.erase(candidate_to_deletion);
        }
    }
}

template<typename T>
std::shared_ptr<ItemVariableScalarRefT<T> >
GeometryServiceBase::
buildPropertyVariableT(ItemGroup group, IGeometryProperty::eProperty property)
{
  typedef ItemVariableScalarRefT<T> VarT;
  String varName = IGeometryProperty::name(property)+String("Of")+group.name()+m_suffix;
  traceMng()->debug() << "Building Variable " << varName << " on " << group.fullName();
  return std::shared_ptr<VarT>(new VarT(VariableBuildInfo(group.mesh(),
                                                            varName,
                                                            group.itemFamily()->name(),
                                                            IVariable::PPrivate|IVariable::PNoRestore),
                                          group.itemKind()));
}
#endif /* ARCGEOSIM_GEOMETRY_IMPL_GEOMETRYSERVICEBASET_H */
