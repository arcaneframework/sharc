// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-

#include "arcane/BasicService.h"
#include "arcane/IMeshReader.h"
#include "arcane/FactoryService.h"

#include "ouranos/config.h"
#include "ouranos/kernel/info.hpp"

using namespace Arcane;

class MeshbMeshReader
: public BasicService
, public IMeshReader
{
public:
  MeshbMeshReader(const ServiceBuildInfo& sbi);
public:
virtual bool allowExtension(const String& str);
virtual IMeshReader::eReturnType readMeshFromFile(IPrimaryMesh* mesh,
                                     const XmlNode& mesh_element,
                                     const String& file_name,
                                     const String& dir_name,
                                     bool use_internal_partition);
};

MeshbMeshReader::MeshbMeshReader(const ServiceBuildInfo& sbi)
: Arcane::BasicService(sbi)
{

}

bool MeshbMeshReader::allowExtension(const String& str)
{
  return str== "meshb";
}

IMeshReader::eReturnType
MeshbMeshReader::
readMeshFromFile(IPrimaryMesh* mesh,
                 const XmlNode& mesh_element,
                 const String& file_name,
                 const String& dir_name,
                 bool use_internal_partition)
{
  Arcane::Trace::Setter setter(traceMng(),"IXMMeshReader");

  info() << "=== READING Meshb MESH ===";

  info () << "Testing ouranos linking " << Ouranos::Kernel::version();

  fatal() << "Meshb Reader not yet implemented";

  // auto rns = std::make_shared<Ouranos::Kernel::Ouranos>(com());
  // Ouranos::Mesh::Mesh<3, Ouranos::Kernel::Block> msh(rns);
  // msh.read(file_name.c_str());
  // auto refined_msh = Ouranos::Mesh::refine(rns, msh);

  return RTOk;
};

ARCANE_REGISTER_SUB_DOMAIN_FACTORY(MeshbMeshReader,IMeshReader,MeshbMeshReaderService);
