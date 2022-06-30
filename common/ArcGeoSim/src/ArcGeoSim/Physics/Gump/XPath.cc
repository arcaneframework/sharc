// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#include "XPath.h"
/* Author : desrozis at Mon Jun 29 16:05:37 2015
 * Generated by createNew
 */


#include "ArcGeoSim/Physics/Gump/RegularExpression.h"

#include <boost/property_tree/ptree.hpp>

#include <arcane/utils/StringBuilder.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Gump {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

struct XPathRecursiveTool
{
  using entity_tree = boost::property_tree::basic_ptree<std::string,Entity>;

  static entity_tree entityTree(const Entity& entity)
  {
    entity_tree tree;
    tree.put(entity.name().localstr(), entity);
    for(const auto& e : entity.entities()) {
      feedEntityTree(entity.name(), e, tree);
    }
    return tree;
  }

  static void feedEntityTree(Arcane::String path,
                             const Entity& entity,
                             entity_tree& tree)
  {
    Arcane::StringBuilder sb;
    sb += path;
    sb += ".";
    sb += entity.name();
    auto localpath = sb.toString();
    tree.put(localpath.localstr(), entity);
    for(const auto& e : entity.entities()) {
      feedEntityTree(sb.toString(), e, tree);
    }
  }

  using property_tree = boost::property_tree::basic_ptree<std::string,Property>;

  static property_tree propertyTree(const Entity& entity)
  {
    property_tree tree;
    for(auto i = 0; i < entity.numberOfProperties(); ++i) {
      auto k = entity.propertyKind(i);
      auto p = entity.property(k);
      Property property(p->kind(), p->owner());
      Arcane::StringBuilder sb;
      sb += entity.name();
      sb += ".";
      sb += p->name();
      tree.put(sb.toString().localstr(), property);
    }
    for(const auto& e : entity.entities()) {
      feedPropertyTree(entity.name(), e, tree);
    }
    return tree;
  }

  static void feedPropertyTree(Arcane::String path,
                               const Entity& entity,
                               property_tree& tree)
  {
    Arcane::StringBuilder sb;
    sb += path;
    sb += ".";
    sb += entity.name();
    auto localpath = sb.toString();
    for(auto i = 0; i < entity.numberOfProperties(); ++i) {
      auto k = entity.propertyKind(i);
      auto p = entity.property(k);
      Property property(p->kind(), p->owner());
      Arcane::StringBuilder sb2;
      sb2 += localpath;
      sb2 += ".";
      sb2 += p->name();
      //std::cout << "put : " << sb2.toString() << " => " << p->name() << std::endl;
      tree.put(sb2.toString().localstr(), property);
    }
    for(const auto& e : entity.entities()) {
      feedPropertyTree(sb.toString(), e, tree);
    }
  }

  template<typename T>
  static void printLowLevel(std::string path,
                            const boost::property_tree::basic_ptree<std::string,T>& tree)
  {
    if(tree.empty()) {
      std::cout << " " << path << std::endl;
      return;
    }
    for(const auto& it : tree) {
      printLowLevel(path + "." + it.first, it.second);
    }
  }

  template<typename T>
  static void print(const boost::property_tree::basic_ptree<std::string,T>& tree)
  {
    std::cout << " ** allowed path **" << std::endl;
    for(const auto& it : tree) {
      printLowLevel(it.first, it.second);
    }
  }
};

/*---------------------------------------------------------------------------*/

Entity
XPath<Entity>::
operator()(const Entity& entity, Arcane::String path) const
{
  auto tree = XPathRecursiveTool::entityTree(entity);

  auto v = tree.get_child_optional(std::string(path.localstr()));

  if(not v.is_initialized()) {
    XPathRecursiveTool::print(tree);
    throw Arcane::FatalErrorException(Arcane::String::format("wrong path {0}", path));
  }

  return v.get().get_value<Entity>();
};

/*---------------------------------------------------------------------------*/

void
XPath<Entity>::
debug(const Entity& entity) const
{
  auto tree = XPathRecursiveTool::entityTree(entity);

  XPathRecursiveTool::print(tree);
};

/*---------------------------------------------------------------------------*/

Entity
XPath<Entity>::
operator()(const Entity& entity, Arcane::String path,
           const IConverter& entity_converter) const
{
  auto tokens = RegularExpression()(path.localstr(), "\\[(.*)\\](.*)");

  if(tokens.size() != 2) {
    auto rules = Arcane::String::format(
        "{0} doesn't match with regular expressions to decode entity\n"
        " =>   entity rule : [<! entity type !>]<! entity name !>", path);
    throw Arcane::FatalErrorException(rules);
  }

  auto entity_kind = entity_converter.convert(tokens[0]);

  if(entity_kind == entity_converter.undefined()) {
    Arcane::String error = Arcane::String::format(
        "{0} is not a valid entity kind (path={1})",
        tokens[0], path);
    throw Arcane::FatalErrorException(error);
  }

  if(entity_kind == entity.kind() && tokens[1] == entity.name())
    return entity;

  if(not entity.contains(tokens[1], entity_kind)) {
    auto error = Arcane::String::format(
        "{0} doesn't contains entity named {1} of type {2}",
        entity.name(), tokens[1], tokens[0]);
    throw Arcane::FatalErrorException(error);
  }

  return entity.entity(tokens[1], entity_kind);
}

/*---------------------------------------------------------------------------*/

Arcane::SharedArray<Entity>
XPath<Entity>::
pack(const Entity& entity, Arcane::String path,
     const IConverter& entity_converter) const
{
  Arcane::SharedArray<Entity> entities;

  {
    // coding rule "\\[(.*)\\](.*)->\\[(.*)\\]\\*"
    auto tokens =
        RegularExpression()(path.localstr(), "\\[(.*)\\](.*)->\\[(.*)\\]\\*");

    if(tokens.size() == 3) {

      auto entity_kind = entity_converter.convert(tokens[0]);

      if(entity_kind == entity_converter.undefined()) {
        Arcane::String error = Arcane::String::format(
            "{0} is not a valid entity kind (path={1})",
            tokens[0], path);
        throw Arcane::FatalErrorException(error);
      }

      Entity e;

      if(entity_kind == entity.kind() && tokens[1] == entity.name()) {

        e = entity;

      } else {

        if(not entity.contains(tokens[1], entity_kind)) {
          auto error = Arcane::String::format(
              "{0} doesn't contains entity named {1} of type {2}",
              entity.name(), tokens[1], tokens[0]);
          throw Arcane::FatalErrorException(error);
        }

        e = entity.entity(tokens[1], entity_kind);
      }

      auto sub_entity_kind = entity_converter.convert(tokens[2]);

      if(sub_entity_kind == entity_converter.undefined()) {
        auto error = Arcane::String::format(
            "{0} is not a valid entity kind (path={1})",
            tokens[2], path);
        throw Arcane::FatalErrorException(error);
      }

      auto enumerator = e.entities(sub_entity_kind);

      for(; enumerator.hasNext(); ++enumerator)
        entities.add(*enumerator);

      return entities;
    }
  }

  {
    // coding rule "\\[(.*)\\]\\*"
    auto tokens =
        RegularExpression()(path.localstr(), "\\[(.*)\\]\\*");

    if(tokens.size() == 1) {

      auto entity_kind = entity_converter.convert(tokens[0]);

      if(entity_kind == entity_converter.undefined()) {
        auto error = Arcane::String::format(
            "{0} is not a valid entity kind (path={1})",
            tokens[0], path);
        throw Arcane::FatalErrorException(error);
      }

      Entity e = entity;

      auto enumerator = e.entities(entity_kind);

      for(; enumerator.hasNext(); ++enumerator)
        entities.add(*enumerator);

      return entities;
    }
  }

  // Sinon erreur
  auto rules = Arcane::String::format(
      "{0} doesn't match with regular expressions to decode entity pack\n"
      " =>   entity pack rule : [<! entity type !>]<! entity name !>::All[<! sub entity type !>]", path);
  throw Arcane::FatalErrorException(rules);

  return entities;

}

/*---------------------------------------------------------------------------*/

Property
XPath<Property>::
operator()(const Entity& entity, Arcane::String path) const
{
  auto tree = XPathRecursiveTool::propertyTree(entity);

  auto v = tree.get_child_optional(std::string(path.localstr()));

  if(not v.is_initialized()) {
    XPathRecursiveTool::print(tree);
    throw Arcane::FatalErrorException(Arcane::String::format("wrong path {0}", path));
  }

  return v.get().get_value<Property>();
};

/*---------------------------------------------------------------------------*/

void
XPath<Property>::
debug(const Entity& entity) const
{
  auto tree = XPathRecursiveTool::propertyTree(entity);

  XPathRecursiveTool::print(tree);
}

/*---------------------------------------------------------------------------*/

Property
XPath<Property>::
operator()(const Entity& entity, Arcane::String path,
           const IConverter& entity_converter,
           const IConverter& property_converter) const
{
  auto tokens = RegularExpression()(path.localstr(), "\\[(.*)\\](.*)::(.*)");

  if(tokens.size() != 3){
    auto rules = Arcane::String::format(
        "{0} doesn't match with regular expressions to decode property\n"
        " =>   property rule : [<! entity type !>]<! entity name !>::<! property name !>", path);
    throw Arcane::FatalErrorException(rules);
  }

  auto entity_kind = entity_converter.convert(tokens[0]);

  if(entity_kind == entity_converter.undefined()) {
    Arcane::String error = Arcane::String::format(
        "{0} is not a valid entity kind (path={1})",
        tokens[0], path);
    throw Arcane::FatalErrorException(error);
  }

  auto property_kind = property_converter.convert(tokens[2]);

  if(property_kind == property_converter.undefined()) {
    Arcane::String error = Arcane::String::format(
        "{0} is not a valid property kind (path={1})",
        tokens[2], path);
    throw Arcane::FatalErrorException(error);
  }

  if(entity_kind == entity.kind() && tokens[1] == entity.name())
    return Property(property_kind, entity);

  if(not entity.contains(tokens[1], entity_kind)) {
    auto error = Arcane::String::format(
        "{0} doesn't contains entity named {1} of type {2}",
        entity.name(), tokens[1], tokens[0]);
    throw Arcane::FatalErrorException(error);
  }

  return Property(property_kind,
    entity.entity(tokens[1], entity_kind));
}

/*---------------------------------------------------------------------------*/

Arcane::SharedArray<Property>
XPath<Property>::
pack(const Entity& entity, Arcane::String path,
     const IConverter& entity_converter,
     const IConverter& property_converter) const
{
  Arcane::SharedArray<Property> properties;

  {
    // coding rule "\\[(.*)\\](.*)->\\[(.*)\\]\\*::(.*)"
    auto tokens =
        RegularExpression()(path.localstr(), "\\[(.*)\\](.*)->\\[(.*)\\]\\*::(.*)");

    if(tokens.size() == 4) {

      auto entity_kind = entity_converter.convert(tokens[0]);

      if(entity_kind == entity_converter.undefined()) {
        auto error = Arcane::String::format(
            "{0} is not a valid entity kind (path={1})",
            tokens[0], path);
        throw Arcane::FatalErrorException(error);
      }

      Entity e;

      if(entity_kind == entity.kind() && tokens[1] == entity.name()) {

        e = entity;

      } else {

        if(not entity.contains(tokens[1], entity_kind)) {
          auto error = Arcane::String::format(
              "{0} doesn't contains entity named {1} of type {2}",
              entity.name(), tokens[1], tokens[0]);
          throw Arcane::FatalErrorException(error);
        }

        e = entity.entity(tokens[1], entity_kind);
      }

      auto sub_entity_kind = entity_converter.convert(tokens[2]);

      if(sub_entity_kind == entity_converter.undefined()) {
        auto error = Arcane::String::format(
            "{0} is not a valid entity kind (path={1})",
            tokens[2], path);
        throw Arcane::FatalErrorException(error);
      }

      auto property_kind = property_converter.convert(tokens[3]);

      if(property_kind == property_converter.undefined()) {
        auto error = Arcane::String::format(
            "{0} is not a valid property kind (path={1})",
            tokens[3], path);
        throw Arcane::FatalErrorException(error);
      }

      auto enumerator = e.entities(sub_entity_kind);

      for(; enumerator.hasNext(); ++enumerator)
        properties.add(Property(property_kind, *enumerator));

      return properties;
    }
  }

  {
    // coding rule "\\[(.*)\\]\\*::(.*)"
    auto tokens =
        RegularExpression()(path.localstr(), "\\[(.*)\\]\\*::(.*)");

    if(tokens.size() == 2) {

      auto entity_kind = entity_converter.convert(tokens[0]);

      if(entity_kind == entity_converter.undefined()) {
        Arcane::String error = Arcane::String::format(
            "{0} is not a valid entity kind (path={1})",
            tokens[0], path);
        throw Arcane::FatalErrorException(error);
      }

      Entity e = entity;

      auto property_kind = property_converter.convert(tokens[1]);

      if(property_kind == property_converter.undefined()) {
        Arcane::String error = Arcane::String::format(
            "{0} is not a valid property kind (path={1})",
            tokens[1], path);
        throw Arcane::FatalErrorException(error);
      }

      auto enumerator = e.entities(entity_kind);

      for(; enumerator.hasNext(); ++enumerator)
        properties.add(Property(property_kind, *enumerator));

      return properties;
    }
  }

  auto rules = Arcane::String::format(
      "{0} doesn't match with regular expressions to decode property\n"
      " =>   property rule : [<! entity type !>]<! entity name !>::All[<! sub entity type !>]::<! property name !>", path);
  throw Arcane::FatalErrorException(rules);

  return properties;

}

/*---------------------------------------------------------------------------*/

template<typename T> class XPath;

class DefaultXPath
  : public IXPath
{
public:

  DefaultXPath(Entity root)
    : m_root(root) {}

  Entity entity(Arcane::String path)
  {
    return XPath<Entity>()(m_root, path);
  }

  Property property(Arcane::String path)
  {
    return XPath<Property>()(m_root, path);
  }

private:

  Entity m_root;
};

/*---------------------------------------------------------------------------*/

std::shared_ptr<IXPath>
makeDefaultXPath(Entity root)
{
  return std::make_shared<DefaultXPath>(root);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
