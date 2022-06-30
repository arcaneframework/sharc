// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef LAW_VARIABLE_VARIABLEFOLDER_H
#define LAW_VARIABLE_VARIABLEFOLDER_H
/* Author : desrozis at Thu Mar 26 09:54:51 2015
 * Generated by createNew
 */

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Physics/Law2/Contribution/Utils/Traits.h"
#include "ArcGeoSim/Physics/Law2/VariableManager.h"
#include "ArcGeoSim/Physics/Gump/UserProperty.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// should be extended
// for instance Transportable = 2 as add beahviour eq Transportable eq WithDerivative && Transportable
//
enum eVariableProperties
{
  Single = 0,
  WithDerivative = 1
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<ContainerKind::eType CK, ItemKind::eType IK>
struct SupportDefined { enum { value = false }; };

template<>
struct SupportDefined<ContainerKind::Scalar,ItemKind::None> { enum { value = true }; };

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename T, typename V>
void fill(T& t, V v) { t.fill(v); }

template<typename T>
void fill(Arcane::VariableRefScalarT<T>& t, T v) { t = v; }

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<ContainerKind::eType CK, ItemKind::eType IK>
class VariableFolder
{
public:

  typedef typename SupportTraits<IK>::Type Support;
  typedef typename AccessorTraits<CK,IK>::Type Accessor;

public:

  VariableFolder(Arcane::String key, Arcane::IMesh* mesh)
    : m_support_is_defined(SupportDefined<CK,IK>::value)
    , m_key(key)
    , m_variables(mesh)
  {
    m_variables.disableVerbosity();
  }
  
  ~VariableFolder() {}

  const Arcane::String& key() const { return m_key; }

  void setSupport(Support support)
  {
    if(m_support_is_defined)
      throw Arcane::FatalErrorException("support is already defined");

    m_support = support; 

    m_support_is_defined = true;
  }

  void forceSupport(Support support)
  {
    m_support = support;

    m_support_is_defined = true;
  }

  const Support& support() const 
  {
    ARCANE_ASSERT((m_support_is_defined),("support is not defined"));

    return m_support;
  }

  bool contains(const Gump::Property& p) const
  {
    return lawVariableAccessor().contains(p);
  }

  template<Gump::Dimension D, typename T>
  void addVariable(Gump::PropertyT<D,T> p, eVariableProperties desc = Single)
  {
    ARCANE_ASSERT((m_support_is_defined),("support is not defined"));

    auto flag = (desc & WithDerivative) ? Law::eWithDerivative : Law::eWithoutDerivative;

    auto allocator = Law::allocator<CK,IK>(p.cast(), flag, m_support, m_key);

    m_variables << allocator;

    // Pour les Nan
    // On ne remplit que ce qui n'existait pas
    if(allocator->firstAllocation()) 
      _fillZero(p);

  }

  template<typename  E>
  void addVariable(Gump::UserProperty<E> p, eVariableProperties desc = Single)
  {
    ARCANE_ASSERT((m_support_is_defined),("support is not defined"));

    auto flag = (desc & WithDerivative) ? Law::eWithDerivative : Law::eWithoutDerivative;

    auto allocator = Law::allocator<CK,IK>(p.cast(), flag, m_support, m_key);

    m_variables << allocator;

    // Pour les Nan
    // On ne remplit que ce qui n'existait pas
    if(allocator->firstAllocation()) 
      _fillZero(p.cast());

  }

  template<Gump::Dimension D, typename T>
  void _fillZero(Gump::PropertyT<D,T> p)
  {
    auto accessor = lawVariableAccessor();
    auto& v = accessor.values(p.cast());
    fill(v, T(0));
  }

  void addVariable(const Arcane::Array<Gump::ScalarRealProperty>& p, eVariableProperties desc = Single)
  {
    ARCANE_ASSERT((m_support_is_defined),("support is not defined"));

    for(Arcane::Integer i = 0; i < p.size(); ++i)
      addVariable(p[i], desc);
  }

  template<typename E>
  void addVariable(const Arcane::Array< Gump::UserProperty<E> >& p, eVariableProperties desc = Single)
  {
    ARCANE_ASSERT((m_support_is_defined),("support is not defined"));

    for(Arcane::Integer i = 0; i < p.size(); ++i)
      addVariable(p[i], desc);
  }

  Accessor lawVariableAccessor() const
  {
    return AccessorTraits<CK,IK>::accessor(m_variables); 
  } 

  const Law::VariableManager& lawVariableManager() const
  {
    return m_variables;
  }

  void enableVerbosity()
  {
    m_variables.enableVerbosity();
  }

  void disableVerbosity()
  {
    m_variables.disableVerbosity();
  }

public:

  bool m_support_is_defined;

  Arcane::String m_key;

  Support m_support;

  Law::VariableManager m_variables;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

typedef Law::VariableFolder<Law::ContainerKind::Variable,
    Law::ItemKind::Cell> VariableCellFolder;

typedef Law::VariableFolder<Law::ContainerKind::Variable,
    Law::ItemKind::Node> VariableNodeFolder;

typedef Law::VariableFolder<Law::ContainerKind::PartialVariable,
    Law::ItemKind::Cell> PartialVariableCellFolder;

typedef Law::VariableFolder<Law::ContainerKind::PartialVariable,
    Law::ItemKind::Face> PartialVariableFaceFolder;

typedef Law::VariableFolder<Law::ContainerKind::Scalar,
    Law::ItemKind::None> ScalarFolder;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<ContainerKind::eType CK, ItemKind::eType IK>
struct FolderTraits
{
  typedef AccessorTraits<CK,IK> AccessorTraitsType;
  typedef typename AccessorTraitsType::Type Accessor;
  typedef typename ValuesContainerTraits<CK,IK>::Type Values;
  typedef typename DerivativesContainerTraits<CK,IK>::Type Derivatives;
  typedef typename ContributionContainerTraits<CK,IK>::Type Contributions;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* LAW_VARIABLE_VARIABLEFOLDER_H */
