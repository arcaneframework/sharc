// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * GraphOnDofTools.h
 *
 *  Created on: 18 mars 2023
 *      Author: gratienj
 */

#ifndef ARCGEOSIM_SRC_ARCGEOSIM_MESH_UTILS_GRAPHUTILS_H_
#define ARCGEOSIM_SRC_ARCGEOSIM_MESH_UTILS_GRAPHUTILS_H_


#if (ARCANE_VERSION >= 30003)
#include <arcane/mesh/GraphDoFs.h>
#include <arcane/mesh/GraphBuilder.h>
#else
#include <arcane/IMesh.h>
#include <arcane/IGraph.h>
#include <arcane/IGraphModifier.h>
#endif

#ifdef USE_ARCANE_V3
namespace Arcane {
typedef DoF      DualNode ;
typedef DoFGroup DualNodeGroup ;
typedef VariableDoFArrayReal        VariableDualNodeArrayReal;
typedef VariableDoFReal             VariableDualNodeReal;
typedef VariableDoFReal3            VariableDualNodeReal3;
typedef VariableDoFReal3x3          VariableDualNodeReal3x3;
typedef VariableDoFInteger          VariableDualNodeInteger;
typedef VariableDoFBool             VariableDualNodeBool;

typedef PartialVariableDoFArrayReal PartialVariableDualNodeArrayReal;
typedef PartialVariableDoFReal      PartialVariableDualNodeReal;
typedef PartialVariableDoFReal3     PartialVariableDualNodeReal3;
typedef PartialVariableDoFReal3x3   PartialVariableDualNodeReal3x3;

typedef PartialVariableDoFInteger   PartialVariableDualNodeInteger;
typedef MeshPartialVariableScalarRefT<DoF,Int64>     PartialVariableDualNodeInt64;

typedef DoFVector                   DualNodeVector;
typedef DoFEnumerator               DualNodeEnumerator;

typedef DoF      Link ;
typedef DoFGroup LinkGroup ;
typedef VariableDoFArrayReal        VariableLinkArrayReal;
typedef VariableDoFReal             VariableLinkReal;
typedef VariableDoFReal3            VariableLinkReal3;
typedef VariableDoFReal3x3          VariableLinkReal3x3;
typedef VariableDoFInteger          VariableLinkInteger;
typedef VariableDoFInt64            VariableLinkInt64;

typedef PartialVariableDoFReal      PartialVariableLinkReal;
typedef PartialVariableDoFReal3     PartialVariableLinkReal3;
typedef PartialVariableDoFReal3x3   PartialVariableLinkReal3x3;
typedef PartialVariableDoFInteger   PartialVariableLinkInteger;

typedef DoFVector                   LinkVector;
typedef DoFVectorView               LinkVectorView;
typedef DoFEnumerator               LinkEnumerator;

}

using Arcane::DualNode;
using Arcane::Link;

#endif // USE_ARCANE_V3

BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_MESH_NAMESPACE


class GraphMng
{
  /*!
   * Classe d'implementation a usage interne uniquement. L'API doit manipuler les classes IXMEvolutiveMeshBuilder ou IXMMeshBuilder.
   */
public:

  struct DualNode {} ;
  struct Link {} ;

#if (ARCANE_VERSION >= 30003)
  typedef Arcane::IGraph2 GraphType ;
  //static std::map<Arcane::String,std::unique_ptr<Arcane::IGraph2>> m_default_connection_graph_instance ;
  //static Arcane::IGraph2* defautConnectionGraphInstance(Arcane::IMesh * mesh) ;
  static Arcane::IGraph2* graph(Arcane::IMesh * mesh,bool create_if_required=false) ;
  static const Arcane::IGraphConnectivity* graphConnectivity(Arcane::IMesh * mesh) ;

  template<typename ValueT>
  static Arcane::MeshVariableScalarRefT<Arcane::DualNode,ValueT>* newVariableDualNode(Arcane::IMesh* mesh, const Arcane::String& var_name, Arcane::Integer property=-1)
  {
    if(property==-1)
      return new Arcane::MeshVariableScalarRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"DualNodes")) ;
    else
      return new Arcane::MeshVariableScalarRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"DualNodes",property)) ;
  }

  template<typename ValueT>
  static Arcane::MeshPartialVariableScalarRefT<Arcane::DualNode,ValueT>* newPartialVariableDualNode(Arcane::IMesh* mesh, const Arcane::String& var_name,const Arcane::String& group_name, Arcane::Integer property=-1)
  {
    if(property==-1)
      return new Arcane::MeshPartialVariableScalarRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"DualNodes",group_name)) ;
    else
      return new Arcane::MeshPartialVariableScalarRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"DualNodes",group_name,property)) ;
  }


  template<typename ValueT>
  static Arcane::MeshVariableArrayRefT<Arcane::DualNode,ValueT>* newVariableDualNodeArray(Arcane::IMesh* mesh, const Arcane::String& var_name)
  {
    return new Arcane::MeshVariableArrayRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"DualNodes")) ;
  }

  template<typename ValueT>
  static Arcane::MeshPartialVariableArrayRefT<Arcane::DualNode,ValueT>* newPartialVariableDualNodeArray(Arcane::IMesh* mesh, const Arcane::String& var_name, const Arcane::String& group_name)
  {
    return new Arcane::MeshPartialVariableArrayRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"DualNodes",group_name)) ;
  }


  template<typename ValueT>
  static Arcane::MeshVariableScalarRefT<Arcane::Link,ValueT>* newVariableLink(Arcane::IMesh* mesh, const Arcane::String& var_name, Arcane::Integer property=-1)
  {
    if(property==-1)
      return new Arcane::MeshVariableScalarRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links")) ;
    else
      return new Arcane::MeshVariableScalarRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links",property)) ;
  }

  template<typename ValueT>
  static Arcane::MeshVariableArrayRefT<Arcane::Link,ValueT>* newVariableLinkArray(Arcane::IMesh* mesh, const Arcane::String& var_name,Arcane::Integer property=-1)
  {
    if(property==-1)
      return new Arcane::MeshVariableArrayRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links")) ;
    else
      return new Arcane::MeshVariableArrayRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links",property)) ;
  }


  template<typename ValueT>
  static Arcane::MeshPartialVariableScalarRefT<Arcane::Link,ValueT>* newPartialVariableLink(Arcane::IMesh* mesh, const Arcane::String& var_name, const Arcane::String& group_name, Arcane::Integer property=-1)
  {
    if(property==-1)
      return new Arcane::MeshPartialVariableScalarRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links",group_name)) ;
    else
      return new Arcane::MeshPartialVariableScalarRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links",group_name,property)) ;
  }

  template<typename ValueT>
  static Arcane::MeshPartialVariableArrayRefT<Arcane::Link,ValueT>* newPartialVariableLinkArray(Arcane::IMesh* mesh, const Arcane::String& var_name, const Arcane::String& group_name, Arcane::Integer property=-1)
  {
    if(property==-1)
      return new Arcane::MeshPartialVariableArrayRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links",group_name)) ;
    else
      return new Arcane::MeshPartialVariableArrayRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"Links",group_name,property)) ;
  }
#else
  typedef Arcane::IGraph GraphType ;

  static Arcane::IGraph* graph(Arcane::IMesh * mesh,bool create_if_required=false)
  {
     return mesh->graph() ;
  }

  template<typename ValueT>
  static Arcane::MeshVariableScalarRefT<Arcane::DualNode,ValueT>* newVariableDualNode(Arcane::IMesh* mesh, const Arcane::String& var_name)
  {
    return new Arcane::MeshVariableScalarRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name)) ;
  }

  template<typename ValueT>
  static Arcane::MeshPartialVariableScalarRefT<Arcane::DualNode,ValueT>* newPartialVariableDualNode(Arcane::IMesh* mesh, const Arcane::String& var_name, const Arcane::String& group_name)
  {
    return new Arcane::MeshPartialVariableScalarRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,String(),group_name)) ;
  }

  template<typename ValueT>
  static Arcane::MeshVariableArrayRefT<Arcane::DualNode,ValueT>* newVariableDualNodeArray(Arcane::IMesh* mesh, const Arcane::String& var_name,const Arcane::String& family_name)
  {
    return new Arcane::MeshVariableArrayRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,family_name)) ;
  }

  template<typename ValueT>
  static Arcane::MeshPartialVariableArrayRefT<Arcane::DualNode,ValueT>* newPartialVariableDualNodeArray(Arcane::IMesh* mesh, const Arcane::String& var_name,const Arcane::String& group_name)
  {
    return new Arcane::MeshPartialVariableArrayRefT<Arcane::DualNode,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,"DualNode",group_name)) ;
  }


  template<typename ValueT>
  static Arcane::MeshVariableScalarRefT<Arcane::Link,ValueT>* newVariableLink(Arcane::IMesh* mesh, const Arcane::String& var_name)
  {
    return new Arcane::MeshVariableScalarRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name)) ;
  }

  template<typename ValueT>
  static Arcane::MeshVariableArrayRefT<Arcane::Link,ValueT>* newVariableLinkArray(Arcane::IMesh* mesh, const Arcane::String& var_name, Arcane::Integer property=-1)
  {
    if(property==-1)
       return new Arcane::MeshVariableArrayRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name)) ;
    else
       return new Arcane::MeshVariableArrayRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,property)) ;
  }


  template<typename ValueT>
  static Arcane::MeshPartialVariableScalarRefT<Arcane::Link,ValueT>* newPartialVariableLink(Arcane::IMesh* mesh, const Arcane::String& var_name,const Arcane::String& group_name)
  {
    return new Arcane::MeshPartialVariableScalarRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,group_name)) ;
  }

  template<typename ValueT>
  static Arcane::MeshPartialVariableArrayRefT<Arcane::Link,ValueT>* newPartialVariableLinkArray(Arcane::IMesh* mesh, const Arcane::String& var_name, const Arcane::String& family_name, const Arcane::String& group_name)
  {
    return new Arcane::MeshVariableArrayRefT<Arcane::Link,ValueT>(Arcane::VariableBuildInfo(mesh,var_name,family_name,group_name)) ;
  }
#endif
} ;

template<typename ItemT>
struct GraphTraits ;

#ifdef USE_ARCANE_V3
template<>
struct GraphTraits<GraphMng::DualNode>
{
  static const bool is_graph_item = true ;
  static const bool is_link = false ;
  typedef Arcane::DoF item_type ;
};

template<>
struct GraphTraits<GraphMng::Link>
{
  static const bool is_graph_item = true ;
  static const bool is_link = true ;
  typedef Arcane::DoF item_type ;
};
#endif

template<typename ItemT>
struct GraphTraits
{
  static const bool is_graph_item = false ;
  static const bool is_link = false ;
  typedef ItemT item_type ;
};


END_MESH_NAMESPACE

#ifdef USE_ARCANE_V3
class DualGraphBasedModel
{
public :
  DualGraphBasedModel() {}
  DualGraphBasedModel(Arcane::IMesh* mesh,bool create_if_required=false)
  {
    init(mesh,create_if_required) ;
  }
  ~DualGraphBasedModel()
  {
    if(m_graph && m_observer_id != -1)
      m_graph->releaseGraphConnectivityObserver(m_observer_id) ;
  }

  void init(Arcane::IMesh* mesh,bool create_if_required=false) {
    if(m_graph) return ;
    m_graph = ArcGeoSim::Mesh::GraphMng::graph(mesh,create_if_required) ;
    m_connectivity = m_graph->connectivity() ;
#if ARCANE_VERSION <= 31115
    auto observer = new Arcane::GraphConnectivityObserverT<DualGraphBasedModel>(this) ;
#else
    auto observer = new Arcane::mesh::GraphConnectivityObserverT<DualGraphBasedModel>(this) ;
#endif
    m_observer_id = m_graph->registerNewGraphConnectivityObserver(observer) ;
  }

  void updateGraphConnectivity()
  {
    m_connectivity = m_graph->connectivity() ;
  }
protected :
  Arcane::IGraph2* m_graph = nullptr ;
  const Arcane::IGraphConnectivity* m_connectivity = nullptr ;
  Integer m_observer_id = -1 ;
};
#endif // USE_ARCANE_V3

END_ARCGEOSIM_NAMESPACE

#ifdef USE_ARCANE_V3

#define TODUALNODE(item) (item).toDoF()
#define DUALITEM(node) this->m_connectivity->dualItem((node))
#define DUALNODES(link) this->m_connectivity->dualNodes((link))
#define DUALNODES_ENUM(link) this->m_connectivity->dualNodes((link)).enumerator()
#define NB_DUALNODE(link) this->m_connectivity->dualNodes((link)).size()
#define LINKS(node) this->m_connectivity->links((node))
#define LINKS_ENUM(node) this->m_connectivity->links((node)).enumerator()
#define NB_LINK(node) this->m_connectivity->links((node)).size()
#define GRAPH(mesh) ArcGeoSim::Mesh::GraphMng::graph((mesh))


#define ENUMERATE_DUALNODE(name, group) \
for( ::Arcane::ItemEnumeratorT< Arcane::DoF > name((group).enumerator()); name.hasNext(); ++name)

#define ENUMERATE_LINK(name, group) \
for( ::Arcane::ItemEnumeratorT< Arcane::DoF > name((group).enumerator()); name.hasNext(); ++name)

#else // USE_ARCANE_V3

#define TODUALNODE(item) (item).toDualNode()
#define DUALITEM(node) (node).dualItem()
#define DUALNODES(link) (link).dualNodes()
#define DUALNODES_ENUM(link) (link).dualNodes().enumerator()
#define NB_DUALNODE(link) (link).nbDualNode()
#define LINKS(node) (node).links()
#define LINKS_ENUM(node) (node).links().enumerator()
#define NB_LINK(node) (node).nbLink()

#define GRAPH(mesh) (mesh)->graph()

#endif // USE_ARCANE_V3

#endif /* ARCGEOSIM_SRC_ARCGEOSIM_MESH_UTILS_GRAPHUTILS_H_ */
