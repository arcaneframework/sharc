#ifndef ARCGEOSIM_MESH_EXPORTERS_VARIABLESTRIDEFORMATTER_H_
#define ARCGEOSIM_MESH_EXPORTERS_VARIABLESTRIDEFORMATTER_H_

/*
 * VariableStrideFormatter.h
 *
 *  Created on: 11 ao�t 2014
 *      Author: pajon
 */


#include "Formatter.h"

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include <arcane/utils/OStringStream.h>

template<typename T>
class VariableStrideFormatter : public Formatter<T> {
public:
	VariableStrideFormatter();
	VariableStrideFormatter(Arcane::SharedArray<Int32> strides);
	virtual ~VariableStrideFormatter();

	Arcane::String apply(const Arcane::ConstArrayView<T> anArray) const {

		Arcane::OStringStream   ss;

		if (anArray.length() > 0)
		{

			for(Integer i=0, pos=0;i<m_strides.size();++i) {
				ss() << "\n              ";
				for(Integer j=0;j<m_strides[i];++j,++pos) {
					Formatter<T>::writeElement(ss, anArray[pos]);
				}
			}
			ss() << "\n            ";
		}

		return ss.str();
	}

private:
	Arcane::SharedArray<Int32> m_strides;

	bool check(Arcane::SharedArray<T> anArray);

};

template<typename T>
VariableStrideFormatter<T>::VariableStrideFormatter() {
	// TODO Auto-generated constructor stub

}

template<typename T>
VariableStrideFormatter<T>::~VariableStrideFormatter() {
	// TODO Auto-generated destructor stub
}


template<typename T>
VariableStrideFormatter<T>::VariableStrideFormatter(Arcane::SharedArray<Int32> strides) {
	m_strides = strides;
}


#endif /* ARCGEOSIM_MESH_EXPORTERS_VARIABLESTRIDEFORMATTER_H_ */
