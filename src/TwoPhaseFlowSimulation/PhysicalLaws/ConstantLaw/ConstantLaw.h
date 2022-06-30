// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * Constant.h
 */

#ifndef CONSTANTLAW_H_
#define CONSTANTLAW_H_


#include "ArcGeoSim/Utils/Utils.h"

//=====================================================

class ConstantLaw
{
public :

  ConstantLaw()
    : m_value(0.) {}
  
  // Evaluation de la fonction
  void eval( Real& C ) const;
  
private :
  
  Real m_value ;
  
public:

void setParameters( const Real value  );

};


#endif /* CONSTANTLAW_H_ */
