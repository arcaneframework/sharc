#ifndef ARCGEOSIM_MESH_EXPORTERS_FIXEDSTRIDEFORMATTER_H_
#define ARCGEOSIM_MESH_EXPORTERS_FIXEDSTRIDEFORMATTER_H_

/*
 * FixedStrideFormatter.h
 *
 *  Created on: 11 août 2014
 *      Author: pajon
 */


#include "Formatter.h"

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include <arcane/utils/OStringStream.h>

template<typename T>
class FixedStrideFormatter : public Formatter<T> {
public:
	FixedStrideFormatter();
	FixedStrideFormatter(Int32 stride);
	virtual ~FixedStrideFormatter();

	Arcane::String apply(const Arcane::ConstArrayView<T> anArray) const {

		OStringStream   ss;

		Int32 nbLines = anArray.size()/m_stride;

		for(Integer i=0, pos=0;i<nbLines;++i) {
			ss() << "\n              ";
			for(Integer j=0;j<m_stride;++j,++pos) {
				Formatter<T>::writeElement(ss, anArray[pos]);
			}
		}
		ss() << "\n            ";

		return ss.str();
	}

private:
	Int32 m_stride;
};

template<typename T>
FixedStrideFormatter<T>::FixedStrideFormatter() {
	// TODO Auto-generated constructor stub

}

template<typename T>
FixedStrideFormatter<T>::~FixedStrideFormatter() {
	// TODO Auto-generated destructor stub
}


template<typename T>
FixedStrideFormatter<T>::FixedStrideFormatter(Int32 stride) {
	m_stride = stride;
}



#endif /* ARCGEOSIM_MESH_EXPORTERS_FIXEDSTRIDEFORMATTER_H_ */
