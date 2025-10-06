// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef TIME_STAMP_H
#define TIME_STAMP_H

static inline void rdtsc(volatile unsigned long long int *counter){
#ifndef WIN32
  asm volatile ("rdtsc \n\t"
      "movl %%eax,%0 \n\t"
      "movl %%edx,%1 \n\t"
      : "=m" (((unsigned *)counter)[0]), "=m" (((unsigned *)counter)[1])
      :
      : "eax" , "edx");
#endif  
}

#define RDTSC(X) asm volatile ("rdtsc \n\t"\
		       "movl %%eax,%0 \n\t"\
		       "movl %%edx,%1 \n\t"\
		       : "=m" (((unsigned *)(X))[0]), "=m" (((unsigned *)(X))[1])\
		       :\
		       : "eax" , "edx")
#endif
