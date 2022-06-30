// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef LOGFUNC_H
#define LOGFUNC_H

/*!
 * \file LogFunc.h
 * \brief Operations logiques
 * \date 01/04/2008
 * \author Xavier TUNC
 * 
 * Operations logiques sur les tableaux/expressions
 * Les operations ==, !=, <, <=, >, >= sont surcharges 
*/

/*! \brief Macro permettant de generaliser les operations logiques == et !=
 * 
 * Retourne 0 si faux, 1 si vrai
 */
#define LOGFUNC1(op,sens) \
template <typename T, typename U> \
inline int operator op(const T & left, const U & right) \
{ \
  int result = 0; \
  const double eps = 1E-7 ; \
  if (left.getSize() == right.getSize()) { \
    result = 1 ; \
    for(int i=0;i<left.getSize();i++) \
    { \
      if((fabs( left.getValue(i) - right.getValue(i) ) sens max(fabs(left.getValue(i)),fabs(right.getValue(i))) * eps)) \
      { \
        ; \
      } \
      else { \
        result = 0; \
        break; \
      } \
    } \
  } \
  return result; \
} \

LOGFUNC1(==,<=)
LOGFUNC1(!=,>=)

#undef LOGFUNC1

/*! \brief Macro permettant de generaliser les operations logiques <, <=, >, >=
 * 
 * Affiche 1 si vrai, 0 si faux
 */
#define LOGFUNC2(op) \
template <typename T, typename U> \
inline void operator op(const T & left, const U & right) { \
  if (left.getSize() == right.getSize() || left.getSize() == 1 || right.getSize() == 1 ) \
  { \
    for(int i=0;i<left.getSize();i++) \
    { \
      if(left.getValue(i) op right.getValue(i)) { \
        std::cout << 1 << std::endl; \
      } \
      else { \
        std::cout << 0 << std::endl; \
      } \
    } \
  } \
  else \
  { \
    std::cout << " !!! WARNING !!! LHS & RHS pas de meme dim (exit(1)) " << std::endl; \
    exit(1); \
  } \
} \

LOGFUNC2(<)
LOGFUNC2(<=)
LOGFUNC2(>)
LOGFUNC2(>=)

#undef LOGFUNC2

#endif
