// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef IINFOMODEL_H_
#define IINFOMODEL_H_

#include <fstream>

class IInfoModel
{
public :
  virtual ~IInfoModel() {}
  virtual void printInfo() = 0 ;
  virtual void printCurrentTimeInfo() = 0 ;
  virtual void initHistoryCurve(std::ofstream& fout) {}
};

#endif /*IINFOMODEL_H_*/
