/*
 * XdmfDataNode.h
 *
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFDATANODE_H_
#define ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFDATANODE_H_

#include <arcane/CommonVariables.h>
#include "arcane/XmlNode.h"
#include <arcane/utils/Array.h>

#include <arcane/utils/NotSupportedException.h>
#include <arcane/utils/StringBuilder.h>

#include "ArcGeoSim/Mesh/Exporters/IXM4/Formatter.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/NoFormatter.h"

#include "ArcGeoSim/Mesh/Exporters/XDMF/XDMFTypeInfo.h"


#include <arcane/std/Hdf5Utils.h>

using namespace Arcane;

template<typename T>
class XMLFormatBuildInfo {

public:
	XMLFormatBuildInfo(const Formatter<T> & formatter = NoFormatter<T>());
	const Formatter<T> & getFormatter() const ;

private :
 const Formatter<T> & m_formatter;

};

template<typename T>
XMLFormatBuildInfo<T>::XMLFormatBuildInfo(const Formatter<T> &  formatter)
  : m_formatter(formatter)
{
}

template<typename T>
inline const Formatter<T> & XMLFormatBuildInfo<T>::getFormatter() const
{
  return m_formatter;
}


class HDFFormatBuildInfo {

public:
	HDFFormatBuildInfo(const String& hdata_name,const String& directory_name)
	  : m_hdata_name(hdata_name)
	  , m_directory_name(directory_name)
	{
	}
	;
	String getHeavyDataName() const {return m_hdata_name;}
	String getDirectoryName() const {return m_directory_name;}


private :
	String m_hdata_name;
	String m_directory_name;
};



template<typename T>
class XDMFDataNode {

public:

	static void createXDMFDataNode(Arcane::XmlNode& parent_node, Array<T>& array, const XMLFormatBuildInfo<T> & buildinfo = XMLFormatBuildInfo<T>(NoFormatter<T>()));

	static void createXDMFDataNode(Arcane::XmlNode& parent_node, Array<T>& array, const HDFFormatBuildInfo & buildinfo );

	//! Insert an Element
	static 	Arcane::XmlElement createFromParent( Arcane::XmlNode& parent_node, const String& format, const Integer nb_elements);

	// fill HDF File
	static 	void writeHDFFile(Array<T> & data, const Arcane::String & hdata_name, const Arcane::String & directory_name);

};

#endif /* ARCGEOSIM_MESH_EXPORTERS_XDMF_XDMFDATANODE_H_ */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
template<typename T>
void XDMFDataNode<T>::createXDMFDataNode(Arcane::XmlNode& parent_node, Array<T>& array, const XMLFormatBuildInfo<T> & buildinfo)
{
	Arcane::XmlNode data_node = XDMFDataNode<T>::createFromParent(parent_node, "XML", array.size());

	Arcane::String str = buildinfo.getFormatter().apply(array);

	data_node.setValue(str);
}


/*---------------------------------------------------------------------------*/
template<typename T>
void XDMFDataNode<T>::createXDMFDataNode(Arcane::XmlNode& parent_node, Array<T>& array, const HDFFormatBuildInfo & buildinfo)
{
	Arcane::XmlNode data_node = XDMFDataNode<T>::createFromParent(parent_node, "HDF", array.size());

	data_node.setValue(buildinfo.getHeavyDataName());

	XDMFDataNode<T>::writeHDFFile(array, buildinfo.getHeavyDataName(), buildinfo.getDirectoryName());
}


/*---------------------------------------------------------------------------*/
template<typename T>
XmlElement
XDMFDataNode<T>::createFromParent( XmlNode& parent_node, const String& format, const Integer nb_elements){
	XmlElement data_node(parent_node,"DataItem");
	data_node.setAttrValue("Format",format);
	data_node.setAttrValue("NumberType", XDMFTypeInfo<T>::name());
	String nameType = XDMFTypeInfo<T>::name();
	if ( nameType == "Float" ) data_node.setAttrValue("Precision","8");

	if (XDMFTypeInfo<T>::size() != 1)
		data_node.setAttrValue("Dimensions",String::format("{0} {1}", nb_elements, XDMFTypeInfo<T>::size()));
	else
		data_node.setAttrValue("Dimensions",String::format("{0}", nb_elements));

	return(data_node);
}

/*---------------------------------------------------------------------------*/
template<typename T>
void XDMFDataNode<T>::
writeHDFFile(Array<T> & data, const Arcane::String & hdata_name, const Arcane::String & directory_name)
{

	Hdf5Utils::StandardTypes m_hdf5_types;
	Hdf5Utils::HFile m_hdf_file;

	SharedArray<String> str_array;
	hdata_name.split(str_array,':');
	Arcane::String file_name = str_array[0];
	Arcane::String file_position = str_array[1].substring(1); // Curiosite HDF a signaler : il ne faut pas de / en debut de position dans le fichier...

	//! the hdf file have been created before
	m_hdf_file.openAppend(Arcane::String::concat(directory_name,"/",file_name));

	Hdf5Utils::StandardArrayT<T> v(m_hdf_file.id(),file_position);

	v.write(m_hdf5_types, data);
}


/*---------------------------------------------------------------------------*/
template<>
inline void XDMFDataNode<Real3>::
writeHDFFile(Array<Real3> & data, const Arcane::String & hdata_name, const Arcane::String & directory_name)
{

	Hdf5Utils::StandardTypes m_hdf5_types;
	Hdf5Utils::HFile m_hdf_file;

	SharedArray<String> str_array;
	hdata_name.split(str_array,':');
	Arcane::String file_name = str_array[0];
	Arcane::String file_position = str_array[1].substring(1); // Curiosite HDF a signaler : il ne faut pas de / en debut de position dans le fichier...

	//! the hdf file have been created before
	m_hdf_file.openAppend(Arcane::String::concat(directory_name,"/",file_name));

	// convert real3 array into real array
	SharedArray<Real> realdata;
	realdata.resize(data.size()*3);
	int pos=0;
	for (int i=0;i<data.size();i++)
	{
		realdata[pos++] = data[i].x;
		realdata[pos++] = data[i].y;
		realdata[pos++] = data[i].z;
	}

	Hdf5Utils::StandardArrayT<Real> v(m_hdf_file.id(),file_position);
	v.write(m_hdf5_types, realdata);
}
