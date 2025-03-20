// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ISURFACE_H
#define ISURFACE_H

//! Purely virtual interface for surface representation
/*! Used as a pretty 'void*' pointer. Each implementation has to cast this 
 *  object before using it
 */
class ISurface
{
public:
  virtual ~ISurface() {}
};

//! Purely virtual interface for surface pair representation
/*! Used as a pretty 'void*' pointer. Each implementation has to cast this 
 *  object before using it
 */
class ISurfacePair
{
public:
  ISurfacePair(ISurface * master, ISurface * slave) 
    : m_master(master)
    , m_slave(slave) { }

  virtual ~ISurfacePair() {}

public:
  virtual ISurface * masterSurface() const { return m_master; }
  virtual ISurface * slaveSurface() const { return m_slave; }

private:
  ISurface * m_master;
  ISurface * m_slave;
};

#endif /* ISURFACE_H */
