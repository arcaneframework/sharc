#ifndef ARCGEOSIM_MESH_EXPORTERS_FORMATTER_H
#define ARCGEOSIM_MESH_EXPORTERS_FORMATTER_H

/*
 * Formatter.h
 *
 *  Created on: 6 août 2014
 *      Author: pajon
 */

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "arcane/utils/OStringStream.h"

template<typename T>
class Formatter {
public:
	virtual ~Formatter() { }
	virtual String apply(const ConstArrayView<T> anArray) const = 0;
	static void writeElement( Arcane::OStringStream &  ss, const T & anElement);
};

template<typename T>
inline void Formatter<T>::writeElement( Arcane::OStringStream & ss, const T & anElement) {

  ss() << std::setprecision(10) << anElement << " " ;

}

template<>
inline void Formatter<Real3>::writeElement( Arcane::OStringStream & ss, const Arcane::Real3 & anElement) {

  ss() << std::setprecision(10) << anElement.x << " " << std::setprecision(10) << anElement.y << " " << std::setprecision(10) << anElement.z << " " ;

}

#endif /* ARCGEOSIM_MESH_EXPORTERS_FORMATTER_H */
