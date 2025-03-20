/*
 * intel.h
 *
 *  Created on: 12 juin 2013
 *      Author: gratienj
 */

#ifndef WINUTIL_H_
#define WINUTUL_H_

#ifdef USE_INTEL_IPL64

#ifndef USE_PETSC
#include <stddef.h> 
void *_intel_fast_memcpy(void *,const void *,size_t);
void *_intel_fast_memset(void *, int, size_t);
#endif // INCLUDED_PETSCFIX_H

#endif // USE_INTEL_IPL64

#endif // INTEL_H_ 

