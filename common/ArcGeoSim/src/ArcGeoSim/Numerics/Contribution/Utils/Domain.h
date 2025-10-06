/*
 * Domain.h
 *
 *  Created on: 23 janv. 2025
 *      Author: gratienj
 */

#pragma once


namespace ArcNum {

  template<ContainerKind::eType CK, ItemKind::eType IK>
  struct SupportDefined { enum { value = false }; };

  template<>
  struct SupportDefined<ContainerKind::Scalar,ItemKind::None> { enum { value = true }; };


template<ContainerKind::eType CK, ItemKind::eType IK>
class Domain
{
public:

  typedef typename SupportTraits<IK>::Type Support;

public:

  Domain(Arcane::String key="Undefined")
    : m_support_is_defined(SupportDefined<CK,IK>::value)
    , m_key(key)
  {
  }

  ~Domain() {}

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

  void init(const Arcane::String key,Support support)
  {
    m_key = key ;
    setSupport(support) ;
  }

public:

  bool m_support_is_defined;

  Arcane::String m_key;

  Support m_support;

};
}

