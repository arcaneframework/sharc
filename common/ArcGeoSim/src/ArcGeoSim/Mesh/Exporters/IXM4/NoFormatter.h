#ifndef ARCGEOSIM_MESH_EXPORTERS_NOFORMATTER_H_
#define ARCGEOSIM_MESH_EXPORTERS_NOFORMATTER_H_

/*
 * NoFormatter.h
 *
 *  Created on: 11 août 2014
 *      Author: pajon
 */


#include "Formatter.h"

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include <arcane/utils/OStringStream.h>

template<typename T>
class NoFormatter : public Formatter<T> {
public:
	NoFormatter();
	virtual ~NoFormatter();

	Arcane::String apply(const Arcane::ConstArrayView<T> anArray) const {

	    Arcane::OStringStream   ss;
	    ss() << "\n              ";
	    for(Integer i=0,is=anArray.size();i<is;i++){
	    	Formatter<T>::writeElement(ss, anArray[i]);
	    }
	    ss() << "\n          ";

		return ss.str();
	}

};

template<typename T>
NoFormatter<T>::NoFormatter() {
	// TODO Auto-generated constructor stub

}

template<typename T>
NoFormatter<T>::~NoFormatter() {
	// TODO Auto-generated destructor stub
}


#endif /* ARCGEOSIM_MESH_EXPORTERS_NOFORMATTER_H_ */
