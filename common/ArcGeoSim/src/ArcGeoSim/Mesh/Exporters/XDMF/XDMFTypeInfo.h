/*
 * XDMFTypeInfo.h
 *
 *  Created on: 20 août 2014
 *      Author: pajon
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFTYPEINFO_H_
#define ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFTYPEINFO_H_

using namespace Arcane;

template <typename T>
struct XDMFTypeInfo{};


template <>
    struct XDMFTypeInfo<Int32>{
	static const char* name() {return "Int";}
	static const int size() {return 1;}
};

template <>
struct XDMFTypeInfo<Int64>{
	static const char* name() {return "Int";}
	static const int size() {return 1;}
};

template <>
struct XDMFTypeInfo<Real>{
	static const char* name() {return "Float";}
	static const int size() {return 1;}
};

template <>
struct XDMFTypeInfo<Real3>{
	static const char* name() {return "Float";}
	static const int size() {return 3;}
};

template <>
struct XDMFTypeInfo<Real2>{
	static const char* name() {return "Float";}
	static const int size() {return 2;}
};

template <>
struct XDMFTypeInfo<Real2x2>{
	static const char* name() {return "Float";}
	static const int size() {return 4;}
};

template <>
struct XDMFTypeInfo<Real3x3>{
	static const char* name() {return "Float";}
	static const int size() {return 9;}
};

template <>
struct XDMFTypeInfo<unsigned char>{
	static const char* name() {return "Char";}
	static const int size() {return 1;}
};



#endif /* ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFTYPEINFO_H_ */
