// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_NUMERICS_UTILS_SPACE_H
#define ARCGEOSIM_NUMERICS_UTILS_SPACE_H


#ifdef USE_ALIEN_V1

#include <ALIEN/Data/Space.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_NAMESPACE(Alien)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IIndexManager;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_NAMESPACE(ArcGeoSim)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Space final
  : public Alien::ISpace
{
 public:
  
  Space()
    : m_index_mng(nullptr) {}
  
  Space(Alien::IIndexManager* index_mng);

  Space(Alien::IIndexManager* index_mng, Arcane::Integer block_size);
  
  Space(const Space& s) 
    : m_internal(s.m_internal) 
    , m_index_mng(s.m_index_mng) {}
    
  Space(Space&& s)
    : m_internal(s.m_internal)
    , m_index_mng(s.m_index_mng) {}
    
  ~Space() {}
    
  Space& operator=(const Space& s)
    {
      m_internal = s.m_internal;
      m_index_mng = s.m_index_mng;
      return *this;
    }
  Space& operator=(Space&& s)
    {
      m_internal = s.m_internal;
      m_index_mng = s.m_index_mng;
      return *this;
    }
    
  //! Comparaison entre deux espaces
  /*! Deux espaces sont ��gaux si c'est le m��me */
  bool operator==(const Alien::ISpace& space) const
  {
    return *m_internal == space;
  }
  bool operator!=(const Alien::ISpace& space) const
  {
    return *m_internal != space;
  }
    
  //! Taille de l'espace (SD: i.e. taille de la base, dimension ou cardinalit��)
  Arcane::Integer size() const { return m_internal->size(); }
    
  //! Nom de l'espace
  const Arcane::String& name() const { return m_internal->name(); }
    
  //! Ajout de champs, ie indices par label
  void setField(Arcane::String label, const Arcane::IntegerUniqueArray& indices)
  {
    m_internal->setField(label, indices);
  }
    
  //! Nombre de champs
  Arcane::Integer nbField() const { return m_internal->nbField(); }
    
  //! Label du ieme champ
  Arcane::String fieldLabel(Arcane::Integer i) const { return m_internal->fieldLabel(i); }
    
  //! Retrouve les indices du ieme champ
  const Arcane::IntegerUniqueArray& field(Arcane::Integer i) const { return m_internal->field(i); }
    
  //! Retrouve les indices du champ �� partir du label
  const Arcane::IntegerUniqueArray& field(Arcane::String label) const { return m_internal->field(label); }
    
  //! Cheap clone
  std::shared_ptr<Alien::ISpace> clone() const 
  {
    return std::make_shared<Space>(*this); 
  }
    
  //! Index manager
  Alien::IIndexManager* indexManager() const { return m_index_mng; }
  
 private:
  void _init();
  
 private:
  
  std::shared_ptr<Alien::Space> m_internal;
    
  Alien::IIndexManager* m_index_mng;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif


#endif /* ARCGEOSIM_NUMERICS_LINEARALGEBRA2_SPACE_H */
