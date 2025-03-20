/*
 * IXM4TypeInfo.h
 *
 *  Created on: 20 août 2014
 *      Author: pajon
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_IXM4TYPEINFO_H_
#define ARCGEOSIM_MESH_EXPORTERS_IXM4TYPEINFO_H_

using namespace Arcane;

template <typename T>
struct IXM4TypeInfo{};

template <>
    struct IXM4TypeInfo<Int32>{
	static const char* name() {return "int32";}
	static const int size() {return 1;}
};

template <>
struct IXM4TypeInfo<Int64>{
	static const char* name() {return "int64";}
	static const int size() {return 1;}
};

template <>
struct IXM4TypeInfo<Real>{
	static const char* name() {return "real";}
	static const int size() {return 1;}
};

template <>
struct IXM4TypeInfo<Real3>{
	static const char* name() {return "real3";}
	static const int size() {return 1;}
};

template <>
struct IXM4TypeInfo<Real2>{
	static const char* name() {return "real2";}
	static const int size() {return 2;}
};

template <>
struct IXM4TypeInfo<Real2x2>{
	static const char* name() {return "real2x2";}
	static const int size() {return 4;}
};

template <>
struct IXM4TypeInfo<Real3x3>{
	static const char* name() {return "real3x3";}
	static const int size() {return 9;}
};

template <>
struct IXM4TypeInfo<unsigned char>{
	static const char* name() {return "unsigned char";}
	static const int size() {return 1;}
};



#endif /* ARCGEOSIM_MESH_EXPORTERS_IXM4TYPEINFO_H_ */
