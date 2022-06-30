// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "FunctionGraph.h"
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
/* Author : gaynor at Mon Nov  6 2018
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

FunctionGraph::
FunctionGraph(bool fatal_if_unregistered, Arcane::ITraceMng* trace)
: m_fatal_if_unregistered(fatal_if_unregistered)
, m_trace(trace){}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
add(FunctionPtr f)
{
  VertexType new_vertex = boost::add_vertex(VertexProperty(f), m_graph);
  _addVertexInOutputPropertyMap(new_vertex);
  _createAddDownEdge(new_vertex);
  _createAddUpEdge(new_vertex);
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
replace(FunctionPtr f)
{
  // trouve le noeud a supprimer dans le graphe
  std::pair<VertexIterator, VertexIterator> it = boost::vertices(m_graph);
  for(; it.first != it.second; ++it.first){
    const FunctionPtr function = m_graph[*it.first].function;
    if(function->id()==f->id()){
      //!TODO ajouter un check sur la signature
      // si la signature est la meme
      // sinon le traitement est plus complique...
      m_graph[*it.first].function = f;
      return;
    }
  }
}

/*---------------------------------------------------------------------------*/

const FunctionGraph::FunctionPtr
FunctionGraph::
find(Integer i) const
{
  std::pair<VertexIterator, VertexIterator> it = boost::vertices(m_graph);
  for(; it.first != it.second; ++it.first){
    const FunctionPtr function = m_graph[*it.first].function;
    if(function->id()==i){
      return function;
    }
  }
  return end();
}

/*---------------------------------------------------------------------------*/

const FunctionGraph::FunctionPtr
FunctionGraph::
find(const Property& p) const
{
  auto it = m_output_property_to_function_vertex.find(p.id());
  if(it != m_output_property_to_function_vertex.end())
    return  m_graph[it->second].function;
  return end();
}

/*---------------------------------------------------------------------------*/

bool
FunctionGraph::
hasCycle() const
{
  bool has_cycle = false;
  cycle_detector vis(has_cycle);
  boost::depth_first_search(m_graph, visitor(vis));
  return has_cycle;
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
checkCycle() const
{
  if(hasCycle())
  {
    std::string output_file_name = "./cyclic_problematic_graph.dot";
    std::ofstream file(output_file_name.c_str());
    printDotGraph(file);
    m_trace->fatal() << "Law functions graph must be direct acyclic graph but contain cycle(s)  \n"
        <<"to visualized cycle(s) used command line: \n"
        <<"dotty "<<output_file_name;
  }
}

/*---------------------------------------------------------------------------*/

FunctionGraph::FunctionVector
FunctionGraph::
toSortVector() const
{
  checkCycle();
  const VertexList sort_vertices  = _sortedVertex();
  return _toFunctionsVector(sort_vertices);
}

/*---------------------------------------------------------------------------*/

FunctionGraph::FunctionVector
FunctionGraph::
toSortVector(IntegerConstArrayView idx_output, const bool recursive) const
{
  checkCycle();
  VertexList sort_vertices  = _sortedVertex();
  _deleteSubProperty(idx_output, sort_vertices, recursive);
  return _toFunctionsVector(sort_vertices);
}

/*---------------------------------------------------------------------------*/

FunctionGraph::DependenciesMap
FunctionGraph::
computeDependencies(DerivativesMode mode) const
{
  std::map<Property, PropertyVector> dependencies;
  if(mode == eComposition){
    checkCycle();
    const VertexList sort_vertices  = _sortedVertex();
    _computeDependencies(sort_vertices, dependencies);
  }
  else if(mode == eDirect){
    std::pair<VertexIterator, VertexIterator> it = boost::vertices(m_graph);
    for(; it.first != it.second; ++it.first){
      const FunctionPtr function = m_graph[*it.first].function;
      const PropertyVector in_props = function->in();
      const PropertyVector out_props = function->out();
      for(Integer i_prop = 0; i_prop<out_props.size();i_prop ++)
        dependencies.insert(std::make_pair(out_props[i_prop],in_props));
    }
  }
  return dependencies;
}
/*---------------------------------------------------------------------------*/

void
FunctionGraph::
printDotGraph(std::ostream& nout) const
{
  boost::write_graphviz(nout, m_graph, boost::make_label_writer(boost::get(&VertexProperty::name, m_graph)),
      boost::make_label_writer(boost::get(&EdgeProperty::name, m_graph)));
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
_createEdge(const VertexType& in_vertex, const VertexType& out_vertex, const PropertyVector& link){
  bool is_linked = false;
  EdgeType new_edge ;
  boost::tie(new_edge, is_linked) = boost::add_edge(out_vertex, in_vertex, EdgeProperty(link), m_graph);
  if(!is_linked){
    const Arcane::String in_function_name =  m_graph[in_vertex].function->name();
    const Arcane::String out_function_name = m_graph[out_vertex].function->name();
    m_trace->fatal() <<"problem connecting function "<<in_function_name<<
        " to function "<<out_function_name;
  }
}

/*---------------------------------------------------------------------------*/
void
FunctionGraph::
_addVertexInOutputPropertyMap(const VertexType& new_vertex){
  const PropertyVector out = m_graph[new_vertex].function->out();
  for(int i=0;i<out.size();++i){
    const Property prop = out[i];
    const int property_id =  prop.id();
    VertexMap::iterator it_out; bool success = false;
    boost::tie(it_out, success) = m_output_property_to_function_vertex.insert(std::make_pair(property_id,new_vertex));
    if(!success)
      m_trace->fatal() <<"Property "<<prop.name()<<" computed by multiple functions";
  }
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
_createAddDownEdge(const VertexType& new_vertex){
  typedef std::map<VertexType, PropertyVector> VertexPropMap;
  const PropertyVector out = m_graph[new_vertex].function->out();
  VertexPropMap connected_vertices_to_properties;
  for(int i=0;i<out.size();++i){
    const Property prop = out[i];
    const int property_id =  prop.id();
    // calcul des proprietes connectees
    auto it = m_free_input_property_to_function_vertices.find(property_id);
    if(it!= m_free_input_property_to_function_vertices.end()){
      VertexList list = it->second;
      for(auto& vert : list){
        connected_vertices_to_properties.insert(std::make_pair(vert,PropertyVector()));
        connected_vertices_to_properties[vert]<<prop;
      }
    }
  }
  // creation d une arete in-out
  for(auto& vert_to_prop : connected_vertices_to_properties){
    PropertyVector edge_prop = vert_to_prop.second;
    _createEdge(vert_to_prop.first, new_vertex, edge_prop);
    // nettoyage de la map d input property
    for(int i=0;i<edge_prop.size();++i)
      m_free_input_property_to_function_vertices.erase(edge_prop[i].id());
  }
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
_createAddUpEdge(const VertexType& new_vertex){
  typedef std::map<VertexType, PropertyVector> VertexPropMap;
  const PropertyVector in = m_graph[new_vertex].function->in();
  VertexPropMap connected_vertices_to_properties;
  for(int i=0;i<in.size();++i){
    const Property prop = in[i];
    const int property_id = prop.id();
    auto it = m_output_property_to_function_vertex.find(property_id);
    if(it == m_output_property_to_function_vertex.end()){
      m_free_input_property_to_function_vertices.insert(std::make_pair(property_id, VertexList()));
      m_free_input_property_to_function_vertices[property_id].push_back(new_vertex);
    }
    else{
      const VertexType vertex = it->second;
      connected_vertices_to_properties.insert(std::make_pair(vertex,PropertyVector()));
      connected_vertices_to_properties[vertex]<<prop;
    }
  }
  // creation d une arete in-out
  for(auto& vert_to_prop : connected_vertices_to_properties)
    _createEdge(new_vertex, vert_to_prop.first , vert_to_prop.second);
}

/*---------------------------------------------------------------------------*/

FunctionGraph::VertexList
FunctionGraph::
_sortedVertex() const
{
  VertexList sort_vertex;
  boost::topological_sort(m_graph,std::front_inserter(sort_vertex));
  return sort_vertex;
}

/*---------------------------------------------------------------------------*/

FunctionGraph::VertexList
FunctionGraph::
_computeGraphRootVertex(const VertexList& vertices, const bool is_sort) const
{
  VertexList root_vertices;
  for(const auto& vertex : vertices){
    std::pair<InEdgeIterator, InEdgeIterator> it = boost::in_edges(vertex, m_graph);
    if(it.first == it.second)
      root_vertices.push_back(vertex);
    else if(is_sort)
      break;
  }
  return root_vertices;
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
_tagUpVertex(const VertexType& source_vertex, std::vector<bool>& is_find) const
{
  std::pair<InEdgeIterator, InEdgeIterator> it = boost::in_edges(source_vertex, m_graph);
  for(; it.first != it.second; ++it.first){
    const EdgeType current_edge = *it.first;
    auto current_vertex = boost::source(current_edge, m_graph);
    if(!is_find[current_vertex]){
      is_find[current_vertex]=true;
      _tagUpVertex(current_vertex, is_find);
    }
  }
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
_deleteSubProperty(IntegerConstArrayView ids, VertexList& sort_vertices, const bool recursive) const
{
  const Integer num_vertices = size();
  std::vector<bool> is_find(num_vertices, false);
  for(Integer i=0; i<ids.size();i++){
    auto it =  m_output_property_to_function_vertex.find(ids[i]);
    if(it == m_output_property_to_function_vertex.end()){
      if(m_fatal_if_unregistered)
        m_trace->fatal() <<"Function of property id="<<ids[i]<<" is not registered or root";
    }
    else{
      const auto current_vertex = it->second;
      if(!is_find[current_vertex]){
        is_find[current_vertex]=true;
        if(recursive)
          _tagUpVertex(current_vertex, is_find);
      }
    }
  }
  // suppression des noeud tagges
  for(Integer i = 0; i<num_vertices;i++){
    if(!is_find[i])
      sort_vertices.remove(i);
  }
}

/*---------------------------------------------------------------------------*/

FunctionGraph::FunctionVector
FunctionGraph::
_toFunctionsVector(const VertexList& vertices) const
{
  FunctionVector functions;
  functions.reserve(static_cast<Arcane::Integer>(vertices.size()));
  for(const auto& vertex : vertices)
    functions.add(m_graph[vertex].function);
  return functions;
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
_dependencies(const VertexType& parent_vertex, DependenciesMap& dependencies, PropertyVector& in_dependencies) const {
  const FunctionPtr func = m_graph[parent_vertex].function;
  const PropertyVector in_props_function = func->in();
  for(Integer j = 0; j<in_props_function.size(); j++){
    const Property prop = in_props_function[j];
    bool current_prop_is_root = true;
    std::pair<InEdgeIterator, InEdgeIterator> it = boost::in_edges(parent_vertex, m_graph);
    for(; it.first != it.second; ++it.first){
      EdgeType current_edge = *it.first;
      PropertyVector in_props_edge = m_graph[current_edge].linked_properties;
      if(in_props_edge.contains(prop) && dependencies[prop].size()>0){
    	current_prop_is_root = false;
    	break;
      }
    }
    // root prop
    if(current_prop_is_root)
      in_dependencies<<prop;
    else
      in_dependencies<<dependencies[prop];
  }
}

/*---------------------------------------------------------------------------*/

void
FunctionGraph::
_computeDependencies(const VertexList& sort_vertices, DependenciesMap& dependencies) const
{
  for(const auto & vertex : sort_vertices){
    FunctionPtr func = m_graph[vertex].function;
    PropertyVector out_props = func->out();
    PropertyVector in_dependencies;
    _dependencies(vertex, dependencies, in_dependencies);
    for(Integer j = 0; j<out_props.size(); j++)
      dependencies.insert(std::make_pair(out_props[j],in_dependencies));
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
