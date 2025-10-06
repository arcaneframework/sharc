// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifdef USE_ALIEN_V1

#include "ArcGeoSim/Numerics/AlienTools/Space.h"
#include "ArcGeoSim/Numerics/AlienTools/IIndexManager.h"

#include <map>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_NAMESPACE(Alien)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_NAMESPACE(ArcGeoSim)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Space::
Space(Alien::IIndexManager* index_mng)
: m_index_mng(index_mng)
{
  if(not m_index_mng->isPrepared())
    m_index_mng->prepare();
  m_internal = std::make_shared<Alien::Space>(m_index_mng->globalSize());
  this->_init();
}

Space::
Space(Alien::IIndexManager* index_mng, Arcane::Integer block_size)
: m_index_mng(index_mng)
{
 if(not m_index_mng->isPrepared())
    m_index_mng->prepare();
  m_internal = std::make_shared<Alien::Space>(m_index_mng->globalSize()/block_size);
  this->_init();
}

void
Space::
_init()
{
  std::map<Arcane::String,Arcane::IntegerUniqueArray> current_field_indices;
    
  for(auto i = m_index_mng->enumerateEntry(); i.hasNext(); ++i) {
    if(i->hasTag("block-tag")) {
      auto tag = i->tagValue("block-tag");
      auto indices = i->getOwnIndexes();
      Alien::addRange(current_field_indices[tag], indices);
    }
  }
    
  if(current_field_indices.size() == 0) return;
    
  for(auto i = current_field_indices.begin(); i != current_field_indices.end(); ++i) {
    if(i->second.size() > 0)
      m_internal->setField(i->first, i->second);
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif

