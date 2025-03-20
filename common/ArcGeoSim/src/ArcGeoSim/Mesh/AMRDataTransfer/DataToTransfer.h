// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_AMRDATATRANSFER_DATATOTRANSFER_H
#define ARCGEOSIM_MESH_AMRDATATRANSFER_DATATOTRANSFER_H


// Donn�es pouvant �tre transf�r�es d'un maillage � l'autre

#include <arcane/ArcaneVersion.h>
#include <arcane/Item.h>
#include <arcane/ItemVector.h>
#include <arcane/ItemVectorView.h>
#include <arcane/mesh/ItemFamily.h>
#include <arcane/VariableTypes.h>
#include <map>

using namespace Arcane;

class DataToTransfer
{
public:
  
  enum eDataKind { PK_Variable, PK_Vector, PK_Tensor, PK_Array };

public:
  /** Constructeur de la classe */
  DataToTransfer(String name, eDataKind data_kind)
    : m_name(name)
    , m_data_kind(data_kind)
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~DataToTransfer() {}

public:
  
  // Donne le nom de la propri�t�
  String name() const { return m_name; }
  
  // Donne le type de propri�t�
  eDataKind kind() const { return m_data_kind; }

  virtual void interpolateP0(Array<ItemInternal*>& parents) {
    throw Arcane::FatalErrorException(A_FUNCINFO,"Not yet implemented") ;
  }
  virtual void upscaleP0(Array<ItemInternal*>& parents) {
    throw Arcane::FatalErrorException(A_FUNCINFO,"Not yet implemented") ;
  }

protected:

  // Nom de la propri�t�
  String m_name;

  // Type de propri�t�
  eDataKind m_data_kind;
};

// Donn�e � transf�rer de type VariableCellReal (scalaire)

class VariableCellRealDataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableCellReal Support;

public:

  VariableCellRealDataToTransfer(String name, VariableCellReal& data)
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
  {
    ;
  }

public:

  // Renvoie la donn�e scalaire
  Support& data() const { return m_data; }
  
  void interpolateP0(Array<ItemInternal*>& parents)
  {
        for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                Real value = m_data[parent] ;
                for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
                {
                        m_data[Cell(parent).hChild(j)] = value ;
                }
        }
  }


  void upscaleP0(Array<ItemInternal*>& parents)
  {
        for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                UInt32 nb_children = Cell(parent).nbHChildren() ;
                Real value = 0. ;
                for (UInt32 j = 0; j < nb_children; j++)
                {
                        value += m_data[Cell(parent).hChild(j)] ;
                }
                m_data[parent] = value/nb_children ;
        }
  }

  private:

  // Propri�t�
  VariableCellReal& m_data;
};

// Donn�e � transf�rer de type VariableCellReal (scalaire)
// avec informations du maillage fin port�es par des variables sur les particules

class VariableParticleCellRealDataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableCellReal Support;

public:

  VariableParticleCellRealDataToTransfer(
      String name,
      VariableCellReal& data,
      VariableParticleReal& particle_data,
      std::map<ItemInternal*, ParticleVector>& cell_to_particles)
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
    , m_particle_data(particle_data)
    , m_cell_to_particles(cell_to_particles)
  {
    ;
  }

public:

  // Renvoie la donn�e scalaire
  Support& data() const { return m_data; }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++)
    {
      ItemInternal* parent = parents[i];
      for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
      {
        ItemInternal* child = Cell(parent).hChild(j).internal();
        ParticleVectorView particle_list = m_cell_to_particles[child];
        Real value = 0. ;
        if (particle_list.size()>0)
        {
          ENUMERATE_PARTICLE(iparticle, particle_list)
          {
            value += m_particle_data[iparticle] ;
          }
          m_data[child] = value/particle_list.size() ;
        }
        else
        {
          m_data[child] = m_data[parent] ;
        }
      }
    }
  }


  void upscaleP0(Array<ItemInternal*>& parents)
  {
  }

  private:

  // Propri�t�
  VariableCellReal& m_data;
  VariableParticleReal& m_particle_data;
  std::map<ItemInternal*,ParticleVector>& m_cell_to_particles;
};

// Donn�e � transf�rer de type VariableFaceReal (scalaire)

class VariableFaceRealDataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableFaceReal Support;

public:

  VariableFaceRealDataToTransfer(String name, VariableFaceReal& data, mesh::MeshRefinement* mesh_refinement = NULL )
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
    , m_mesh_refinement(mesh_refinement)
  {
    ;
  }

public:

  // Renvoie la donn�e scalaire
  Support& data() const { return m_data; }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer p = 0; p < parents.size(); p++)
    {
      ItemInternal* parent = parents[p];
#if (ARCANE_VERSION>=12201)
      switch (parent->typeId())
      {
        case IT_Tetraedron4:
          _interpolateP0<IT_Tetraedron4>(parent, m_mesh_refinement->getRefinementPattern<IT_Tetraedron4>());
          break;
        case IT_Pyramid5:
          _interpolateP0<IT_Pyramid5>(parent, m_mesh_refinement->getRefinementPattern<IT_Pyramid5>());
          break;
        case IT_Pentaedron6:
          _interpolateP0<IT_Pentaedron6>(parent, m_mesh_refinement->getRefinementPattern<IT_Pentaedron6>());
          break;
        case IT_Hexaedron8:
          _interpolateP0<IT_Hexaedron8>(parent, m_mesh_refinement->getRefinementPattern<IT_Hexaedron8>());
          break;
        case IT_HemiHexa7:
          _interpolateP0<IT_HemiHexa7>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa7>());
          break;
        case IT_HemiHexa6:
          _interpolateP0<IT_HemiHexa6>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa6>());
          break;
        case IT_HemiHexa5:
          _interpolateP0<IT_HemiHexa5>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa5>());
          break;
        case IT_AntiWedgeLeft6:
          _interpolateP0<IT_AntiWedgeLeft6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeLeft6>());
          break;
        case IT_AntiWedgeRight6:
          _interpolateP0<IT_AntiWedgeRight6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeRight6>());
          break;
        case IT_DiTetra5:
          _interpolateP0<IT_DiTetra5>(parent, m_mesh_refinement->getRefinementPattern<IT_DiTetra5>());
          break;
        default:
          throw FatalErrorException(A_FUNCINFO,"Not supported refinement Item Type");
      }
#else /* ARCANE_VERSION */
      for (Int32 c = 0; c < parent->nbHChildren(); c++)
      {
        ItemInternal* child = parent->hChild(c);
        ARCANE_ASSERT((parent->nbFace()==child->nbFace()),("Inconsistent refinement"));
        for (Integer f = 0; f < parent->nbFace(); f++)
        {
          m_data[child->face(f)] = m_data[parent->face(f)];
        }
      }
#endif /* ARCANE_VERSION */
    }
  }

  void upscaleP0(Array<ItemInternal*>& parents)
  {
  }

  private:

#if (ARCANE_VERSION>=12201)
  template<int typeID> inline void _interpolateP0(ItemInternal* parent, const ItemRefinementPatternT<typeID>& rp)
  {
    for (Int32 c = 0; c < Cell(parent).nbHChildren(); c++)
    {
      Cell const& child = Cell(parent).hChild(c);
      for (Integer f = 0; f < child.nbFace(); f++)
      {
        m_data[child.face(f)] = m_data[Cell(parent).face(rp.face_mapping(c,f))];
      }
    }
  }
#endif /* ARCANE_VERSION */


  // Propri�t�
  VariableFaceReal& m_data;
  mesh::MeshRefinement* m_mesh_refinement;
};

// Donn�e � transf�rer de type VariableFaceReal (scalaire)

class VariableFaceArrayRealDataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableFaceArrayReal Support;

public:

  VariableFaceArrayRealDataToTransfer(String name, VariableFaceArrayReal& data, mesh::MeshRefinement* mesh_refinement = NULL )
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
    , m_mesh_refinement(mesh_refinement)
  {
    ;
  }

public:

  // Renvoie la donn�e scalaire
  Support& data() const { return m_data; }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer p = 0; p < parents.size(); p++)
    {
      ItemInternal* parent = parents[p];
#if (ARCANE_VERSION>=12201)
      switch (parent->typeId())
      {
        case IT_Tetraedron4:
          _interpolateP0<IT_Tetraedron4>(parent, m_mesh_refinement->getRefinementPattern<IT_Tetraedron4>());
          break;
        case IT_Pyramid5:
          _interpolateP0<IT_Pyramid5>(parent, m_mesh_refinement->getRefinementPattern<IT_Pyramid5>());
          break;
        case IT_Pentaedron6:
          _interpolateP0<IT_Pentaedron6>(parent, m_mesh_refinement->getRefinementPattern<IT_Pentaedron6>());
          break;
        case IT_Hexaedron8:
          _interpolateP0<IT_Hexaedron8>(parent, m_mesh_refinement->getRefinementPattern<IT_Hexaedron8>());
          break;
        case IT_HemiHexa7:
          _interpolateP0<IT_HemiHexa7>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa7>());
          break;
        case IT_HemiHexa6:
          _interpolateP0<IT_HemiHexa6>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa6>());
          break;
        case IT_HemiHexa5:
          _interpolateP0<IT_HemiHexa5>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa5>());
          break;
        case IT_AntiWedgeLeft6:
          _interpolateP0<IT_AntiWedgeLeft6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeLeft6>());
          break;
        case IT_AntiWedgeRight6:
          _interpolateP0<IT_AntiWedgeRight6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeRight6>());
          break;
        case IT_DiTetra5:
          _interpolateP0<IT_DiTetra5>(parent, m_mesh_refinement->getRefinementPattern<IT_DiTetra5>());
          break;
        default:
          throw FatalErrorException(A_FUNCINFO,"Not supported refinement Item Type");
      }
#else /* ARCANE_VERSION */
      for (Int32 c = 0; c < parent->nbHChildren(); c++)
      {
        ItemInternal* child = parent->hChild(c);
        ARCANE_ASSERT((parent->nbFace()==child->nbFace()),("Inconsistent refinement"));
        for (Integer f = 0; f < parent->nbFace(); f++)
        {
          m_data[child->face(f)] = m_data[parent->face(f)];
        }
      }
#endif /* ARCANE_VERSION */
    }
  }

  void upscaleP0(Array<ItemInternal*>& parents)
  {
  }

  private:

#if (ARCANE_VERSION>=12201)
  template<int typeID> inline void _interpolateP0(ItemInternal* parent, const ItemRefinementPatternT<typeID>& rp)
  {
    for (Int32 c = 0; c < Cell(parent).nbHChildren(); c++)
    {
      Cell const&  child = Cell(parent).hChild(c);
      for (Integer f = 0; f < child.nbFace(); f++)
      {
        m_data[child.face(f)].copy(m_data[Cell(parent).face(rp.face_mapping(c,f))]);
      }
    }
  }
#endif /* ARCANE_VERSION */


  // Propri�t�
  VariableFaceArrayReal& m_data;
  mesh::MeshRefinement* m_mesh_refinement;
};

// Donn�e � transf�rer de type VariableFaceInteger (scalaire)

class VariableFaceIntegerDataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableFaceInteger Support;

public:

  VariableFaceIntegerDataToTransfer(String name, VariableFaceInteger& data, mesh::MeshRefinement* mesh_refinement = NULL)
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
    , m_mesh_refinement(mesh_refinement)
  {
    ;
  }

public:

  // Renvoie la donn�e scalaire
  Support& data() const { return m_data; }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer p = 0; p < parents.size(); p++)
    {
      ItemInternal* parent = parents[p];
#if (ARCANE_VERSION>=12201)
      switch (parent->typeId())
      {
        case IT_Tetraedron4:
          _interpolateP0<IT_Tetraedron4>(parent, m_mesh_refinement->getRefinementPattern<IT_Tetraedron4>());
          break;
        case IT_Pyramid5:
          _interpolateP0<IT_Pyramid5>(parent, m_mesh_refinement->getRefinementPattern<IT_Pyramid5>());
          break;
        case IT_Pentaedron6:
          _interpolateP0<IT_Pentaedron6>(parent, m_mesh_refinement->getRefinementPattern<IT_Pentaedron6>());
          break;
        case IT_Hexaedron8:
          _interpolateP0<IT_Hexaedron8>(parent, m_mesh_refinement->getRefinementPattern<IT_Hexaedron8>());
          break;
        case IT_HemiHexa7:
          _interpolateP0<IT_HemiHexa7>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa7>());
          break;
        case IT_HemiHexa6:
          _interpolateP0<IT_HemiHexa6>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa6>());
          break;
        case IT_HemiHexa5:
          _interpolateP0<IT_HemiHexa5>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa5>());
          break;
        case IT_AntiWedgeLeft6:
          _interpolateP0<IT_AntiWedgeLeft6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeLeft6>());
          break;
        case IT_AntiWedgeRight6:
          _interpolateP0<IT_AntiWedgeRight6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeRight6>());
          break;
        case IT_DiTetra5:
          _interpolateP0<IT_DiTetra5>(parent, m_mesh_refinement->getRefinementPattern<IT_DiTetra5>());
          break;
        default:
          throw FatalErrorException(A_FUNCINFO,"Not supported refinement Item Type");
      }
#else /* ARCANE_VERSION */
      for (Int32 c = 0; c < parent->nbHChildren(); c++)
      {
        ItemInternal* child = parent->hChild(c);
        ARCANE_ASSERT((parent->nbFace()==child->nbFace()),("Inconsistent refinement"));
        for (Integer f = 0; f < parent->nbFace(); f++)
        {
          m_data[child->face(f)] = m_data[parent->face(f)];
        }
      }
#endif /* ARCANE_VERSION */
    }
  }

  void upscaleP0(Array<ItemInternal*>& parents)
  {
  }

  private:

#if (ARCANE_VERSION>=12201)
  template<int typeID> inline void _interpolateP0(ItemInternal* parent, const ItemRefinementPatternT<typeID>& rp)
  {
    for (Int32 c = 0; c < Cell(parent).nbHChildren(); c++)
    {
      Cell const& child = Cell(parent).hChild(c);
      for (Integer f = 0; f < child.nbFace(); f++)
      {
        m_data[child.face(f)] = m_data[Cell(parent).face(rp.face_mapping(c,f))];
      }
    }
  }
#endif /* ARCANE_VERSION */

  // Propri�t�
  VariableFaceInteger& m_data;
  mesh::MeshRefinement* m_mesh_refinement;
};

// Donn�e � transf�rer de type VariableFaceInt64 (scalaire)

class VariableFaceInt64DataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableFaceInt64 Support;

public:

  VariableFaceInt64DataToTransfer(String name, VariableFaceInt64& data, mesh::MeshRefinement* mesh_refinement = NULL)
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
    , m_mesh_refinement(mesh_refinement)
  {
    ;
  }

public:

  // Renvoie la donn�e scalaire
  Support& data() const { return m_data; }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer p = 0; p < parents.size(); p++)
    {
      ItemInternal* parent = parents[p];
#if (ARCANE_VERSION>=12201)
      switch (parent->typeId())
      {
        case IT_Tetraedron4:
          _interpolateP0<IT_Tetraedron4>(parent, m_mesh_refinement->getRefinementPattern<IT_Tetraedron4>());
          break;
        case IT_Pyramid5:
          _interpolateP0<IT_Pyramid5>(parent, m_mesh_refinement->getRefinementPattern<IT_Pyramid5>());
          break;
        case IT_Pentaedron6:
          _interpolateP0<IT_Pentaedron6>(parent, m_mesh_refinement->getRefinementPattern<IT_Pentaedron6>());
          break;
        case IT_Hexaedron8:
          _interpolateP0<IT_Hexaedron8>(parent, m_mesh_refinement->getRefinementPattern<IT_Hexaedron8>());
          break;
        case IT_HemiHexa7:
          _interpolateP0<IT_HemiHexa7>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa7>());
          break;
        case IT_HemiHexa6:
          _interpolateP0<IT_HemiHexa6>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa6>());
          break;
        case IT_HemiHexa5:
          _interpolateP0<IT_HemiHexa5>(parent, m_mesh_refinement->getRefinementPattern<IT_HemiHexa5>());
          break;
        case IT_AntiWedgeLeft6:
          _interpolateP0<IT_AntiWedgeLeft6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeLeft6>());
          break;
        case IT_AntiWedgeRight6:
          _interpolateP0<IT_AntiWedgeRight6>(parent, m_mesh_refinement->getRefinementPattern<IT_AntiWedgeRight6>());
          break;
        case IT_DiTetra5:
          _interpolateP0<IT_DiTetra5>(parent, m_mesh_refinement->getRefinementPattern<IT_DiTetra5>());
          break;
        default:
          throw FatalErrorException(A_FUNCINFO,"Not supported refinement Item Type");
      }
#else /* ARCANE_VERSION */
      for (Int32 c = 0; c < parent->nbHChildren(); c++)
      {
        ItemInternal* child = parent->hChild(c);
        ARCANE_ASSERT((parent->nbFace()==child->nbFace()),("Inconsistent refinement"));
        for (Integer f = 0; f < parent->nbFace(); f++)
        {
          m_data[child->face(f)] = m_data[parent->face(f)];
        }
      }
#endif /* ARCANE_VERSION */
    }
  }

  void upscaleP0(Array<ItemInternal*>& parents)
  {
  }

  private:

#if (ARCANE_VERSION>=12201)
  template<int typeID> inline void _interpolateP0(ItemInternal* parent, const ItemRefinementPatternT<typeID>& rp)
  {
    for (Int32 c = 0; c < Cell(parent).nbHChildren(); c++)
    {
      Cell const& child = Cell(parent).hChild(c);
      for (Integer f = 0; f < child.nbFace(); f++)
      {
        m_data[child.face(f)] = m_data[Cell(parent).face(rp.face_mapping(c,f))];
      }
    }
  }
#endif /* ARCANE_VERSION */

  // Propri�t�
  VariableFaceInt64& m_data;
  mesh::MeshRefinement* m_mesh_refinement;
};

// Donn�e � transf�rer de type VariableCellInteger (scalaire)

class VariableCellIntegerDataToTransfer 
  : public DataToTransfer
{
public:

  typedef VariableCellInteger Support;

public:
  
  VariableCellIntegerDataToTransfer(String name, VariableCellInteger& data)
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
  {
    ;
  }

public:

  // Renvoie la donn�e scalaire
  Support& data() const { return m_data; }
  
  void interpolateP0(Array<ItemInternal*>& parents)
  {
        for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                Integer value = m_data[parent] ;
                for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
                {
                        m_data[Cell(parent).hChild(j)] = value ;
                }
        }
  }

  void upscaleP0(Array<ItemInternal*>& parents)
  {
        for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                UInt32 nb_children = Cell(parent).nbHChildren() ;
                Integer value = 0 ;
                for (UInt32 j = 0; j < nb_children; j++)
                {
                        value += m_data[Cell(parent).hChild(j)] ;
                }
                m_data[parent] = value/nb_children ;
        }
  }


private:

  // Propri�t�
  VariableCellInteger& m_data;
};


// Donn�e � transf�rer de type VariableCellReal3 (vecteur)

class VariableCellReal3DataToTransfer 
  : public DataToTransfer
{
public:

  typedef VariableCellReal3 Support;

public:
  
  VariableCellReal3DataToTransfer(String name, VariableCellReal3& data)
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
  {
    ;
  }

public:

  // Renvoie la donn�e vectorielle
  Support& data() const { return m_data; }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
        for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                Real3 value = m_data[parent] ;
                for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
                {
                        m_data[Cell(parent).hChild(j)] = value ;
                }
        }
  }

  
  void upscaleP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                UInt32 nb_children = Cell(parent).nbHChildren() ;
                Real3 value ;
                for (UInt32 j = 0; j < nb_children; j++)
                {
                        value += m_data[Cell(parent).hChild(j)] ;
                }
                m_data[parent] = value/nb_children ;
    }
}


private:

  // Propri�t�
  VariableCellReal3& m_data;
};


// Donn�e � transf�rer de type VariableCellReal3x3 (tenseur)

class VariableCellReal3x3DataToTransfer 
  : public DataToTransfer
{
public:

  typedef VariableCellReal3x3 Support;

public:
  
  VariableCellReal3x3DataToTransfer(String name, VariableCellReal3x3 & data)
    : DataToTransfer(name,PK_Variable)
    , m_data(data)
  {
    ;
  }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                Real3x3 value = m_data[parent] ;
                for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
                {
                        m_data[Cell(parent).hChild(j)] = value ;
                }
    }
  }


  void upscaleP0(Array<ItemInternal*>& parents)
  {
        for (Integer i = 0; i < parents.size(); i++){
                ItemInternal* parent=parents[i];
                UInt32 nb_children = Cell(parent).nbHChildren() ;
                Real3x3 value ;
                for (UInt32 j = 0; j < nb_children; j++)
                {
                        value +=  m_data[Cell(parent).hChild(j)] ;
                }
                m_data[parent] = value/nb_children ;
        }
  }

public:

  // Renvoie la donn�e tensorielle
  Support& data() const { return m_data; }
  
private:

  // Propri�t�
  VariableCellReal3x3 & m_data;
};


// Donn�e intensive � transf�rer de type VariableCellArrayReal (tableau de reels)

class VariableCellArrayRealDataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableCellArrayReal Support;

public:
  VariableCellArrayRealDataToTransfer(String name, VariableCellArrayReal& data) :
      DataToTransfer(name, PK_Array), m_data(data)
  {
    ;
  }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
          ItemInternal* parent=parents[i];
          ConstArrayView<Real> value = m_data[parent] ;
      for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
      {
         m_data[Cell(parent).hChild(j)].copy(value) ;
      }
    }
  }


  void upscaleP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
        ItemInternal* parent=parents[i];
        UInt32 nb_children = Cell(parent).nbHChildren() ;
        ArrayView<Real> pdata = m_data[parent] ;
        pdata.fill(0.) ;
        for (UInt32 j = 0; j < nb_children; j++)
        {
                ConstArrayView<Real> cdata = m_data[Cell(parent).hChild(j)] ;
                for(Integer k=0;k<pdata.size();++k)
                        pdata[k] += cdata[k] ;
        }
        for(Integer k=0;k<pdata.size();++k)
                pdata[k] /= nb_children ;
    }
  }

public:

  // Renvoie la donn�e tableau
  Support& data() const
  {
    return m_data;
  }

private:

  // Propri�t�
  VariableCellArrayReal& m_data;
};

// Donn�e intensive � transf�rer de type VariableCellArrayReal3 (tableau de reels)

class VariableCellArrayReal3DataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableCellArrayReal3 Support;

public:
  VariableCellArrayReal3DataToTransfer(String name, VariableCellArrayReal3& data) :
      DataToTransfer(name, PK_Array), m_data(data)
  {
    ;
  }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
          ItemInternal* parent=parents[i];
          ConstArrayView<Real3> value = m_data[parent] ;
      for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
      {
         m_data[Cell(parent).hChild(j)].copy(value) ;
      }
    }
  }


  void upscaleP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
        ItemInternal* parent=parents[i];
        UInt32 nb_children = Cell(parent).nbHChildren() ;
        ArrayView<Real3> pdata = m_data[parent] ;
        pdata.fill(Real3(0.,0.,0.)) ;
        for (UInt32 j = 0; j < nb_children; j++)
        {
                ConstArrayView<Real3> cdata = m_data[Cell(parent).hChild(j)] ;
                for(Integer k=0;k<pdata.size();++k)
                        pdata[k] += cdata[k] ;
        }
        for(Integer k=0;k<pdata.size();++k)
                pdata[k] /= nb_children ;
    }
  }

public:

  // Renvoie la donn�e tableau
  Support& data() const
  {
    return m_data;
  }

private:

  // Propri�t�
  VariableCellArrayReal3& m_data;
};

// Donn�e extensive � transf�rer de type VariableCellArrayReal (tableau de reels)

class VariableCellArrayRealExtensiveDataToTransfer
  : public DataToTransfer
{
public:

  typedef VariableCellArrayReal Support;

public:
  VariableCellArrayRealExtensiveDataToTransfer(String name, VariableCellArrayReal& data) :
      DataToTransfer(name, PK_Array), m_data(data)
  {
    ;
  }

  void interpolateP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
          ItemInternal* parent=parents[i];
          UInt32 nb_children = Cell(parent).nbHChildren() ;
      for (UInt32 j = 0; j < nb_children; j++)
      {
        ArrayView<Real> cdata = m_data[Cell(parent).hChild(j)] ;
        cdata.fill(0.);
      }
    }
  }


  void upscaleP0(Array<ItemInternal*>& parents)
  {
    for (Integer i = 0; i < parents.size(); i++){
        ItemInternal* parent=parents[i];
        ArrayView<Real> pdata = m_data[parent] ;
        pdata.fill(0.) ;
        for (UInt32 j = 0, js = Cell(parent).nbHChildren(); j < js; j++)
        {
                ConstArrayView<Real> cdata = m_data[Cell(parent).hChild(j)] ;
                for(Integer k=0;k<pdata.size();++k)
                        pdata[k] += cdata[k] ;
        }
    }
  }

public:

  // Renvoie la donn�e tableau
  Support& data() const
  {
    return m_data;
  }

private:

  // Propri�t�
  VariableCellArrayReal& m_data;
};

#endif /* ARCGEOSIM_MESH_AMRDATATRANSFER_DATATOTRANSFER_H */
