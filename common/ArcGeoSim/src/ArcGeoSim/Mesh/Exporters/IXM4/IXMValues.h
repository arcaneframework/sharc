#ifndef ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMVALUES_H
#define ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMVALUES_H
/*
 * IxmValues.h
 *
 *  Created on: May 30, 2012
 *      Author: mesriy
 */

#include "arcane/XmlNode.h"
#include "Formatter.h"

using namespace Arcane;

template<typename T>
class IXMValues {

public :
	//!
	virtual ~IXMValues() { }

	//!
	virtual void setDataNode(Arcane::XmlNode data_node) = 0 ;

	//! Set the name of the Heavy Data Set (if applicable)
	virtual void setHeavyDataName(const String& hdata_name) = 0;

	//! Write the Array to the External Representation
	virtual bool write(Array<T> &) = 0;

	virtual bool write(Array<T> & anArray, const Formatter<T>& formatter) = 0;
protected:

	String      m_heavy_data_name;
	XmlNode     m_data_node;
};

#endif /* ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMVALUES_H */
