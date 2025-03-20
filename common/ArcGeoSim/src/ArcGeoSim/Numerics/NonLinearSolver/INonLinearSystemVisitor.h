// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef INONLINEARSYSTEMVISITOR_H_
#define INONLINEARSYSTEMVISITOR_H_
/**
 * \author Jean-Marc GRATIEN
 * \version 1.0
 * \brief Interface INonLinearSystemVisitor de visiteur de system de type 
 * 
 * Une instance � pour objectif de visiter un syst�me concr�t et de
 * lui applique les op�rations qui lui sont attribu� d'effectuer.
 */


#include <arcane/utils/FatalErrorException.h>
#include <arcane/utils/TraceInfo.h>

using namespace Arcane;

class INonLinearSystem;

class INonLinearSystemVisitor
{
public:
  
  INonLinearSystemVisitor()
  {
    ;
  }
  
  virtual ~INonLinearSystemVisitor() {}
  
  //! to force the class to be abstract pure
  virtual String name() const = 0;
  
  virtual Integer visit(INonLinearSystem * system)
  {
    throw Arcane::FatalErrorException(A_FUNCINFO,"not implemented");
  }
};

#endif /*INONLINEARSYSTEMVISITOR_H_*/
