/*
 * XdmfDataNode.h
 *
 */

#ifndef ARCGEOSIM_MESH_EXPORTERS_IXM_IXM4DATANODE_H
#define ARCGEOSIM_MESH_EXPORTERS_IXM_IXM4DATANODE_H

#include <arcane/CommonVariables.h>
#include "arcane/XmlNode.h"
#include <arcane/utils/Array.h>

#include <arcane/utils/NotSupportedException.h>
#include <arcane/utils/StringBuilder.h>

#include "ArcGeoSim/Mesh/Exporters/IXM4/Formatter.h"
#include "ArcGeoSim/Mesh/Exporters/IXM4/NoFormatter.h"

#include "ArcGeoSim/Mesh/Exporters/IXM4/IXM4TypeInfo.h"

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
	HDFFormatBuildInfo(const String& hdata_name,const String& directory_name, const bool& writeHDFFile = true)
	  : m_hdata_name(hdata_name)
	  , m_directory_name(directory_name)
	  , m_writeHDFFile(writeHDFFile)
	{
	}
	;
	String getHeavyDataName() const {return m_hdata_name;}
	String getDirectoryName() const {return m_directory_name;}
	bool  getWriteHDFFile() const {return m_writeHDFFile;}

private :
	String m_hdata_name;
	String m_directory_name;
	bool m_writeHDFFile;
};



template<typename T>
class IXM4DataNode {

public:

	static void createIXM4DataNode(Arcane::XmlNode& parent_node, Array<T>& array, const Integer array_size=-1, const XMLFormatBuildInfo<T> & buildinfo = XMLFormatBuildInfo<T>(NoFormatter<T>()));

	static void createIXM4DataNode(Arcane::XmlNode& parent_node, Array<T>& array, const HDFFormatBuildInfo & buildinfo, const Integer array_size=-1);

private:

	//! Insert an Element
	static 	Arcane::XmlElement _createFromParent( Arcane::XmlNode& parent_node, const String& format, const Integer array_size);

	// fill HDF File
	static 	void _writeHDFFile(Array<T> & data, const Arcane::String & hdata_name, const Arcane::String & directory_name);

};

#endif /* ARCGEOSIM_MESH_EXPORTERS_IXM_IXM4DATANODE_H */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
template<typename T>
void IXM4DataNode<T>::createIXM4DataNode(Arcane::XmlNode& parent_node, Array<T>& array, const Integer array_size, const XMLFormatBuildInfo<T> & buildinfo)
{
	Arcane::XmlNode data_node = IXM4DataNode<T>::_createFromParent(parent_node, "xml", array_size);

	Arcane::String str = buildinfo.getFormatter().apply(array);

	data_node.setValue(str);
}


/*---------------------------------------------------------------------------*/
template<typename T>
void IXM4DataNode<T>::createIXM4DataNode(Arcane::XmlNode& parent_node, Array<T>& array, const HDFFormatBuildInfo & buildinfo, const Integer array_size)
{
	Arcane::XmlNode data_node = IXM4DataNode<T>::_createFromParent(parent_node, "hdf", array_size);

	data_node.setValue(buildinfo.getHeavyDataName());

	if ( buildinfo.getWriteHDFFile() == true )
		IXM4DataNode<T>::_writeHDFFile(array, buildinfo.getHeavyDataName(), buildinfo.getDirectoryName());
}


/*---------------------------------------------------------------------------*/
template<typename T>
XmlElement
IXM4DataNode<T>::_createFromParent( XmlNode& parent_node, const String& format, const Integer array_size){

	XmlElement data_node(parent_node,"data");
	data_node.setAttrValue("format",format);
	data_node.setAttrValue("type", IXM4TypeInfo<T>::name());

	if (array_size != -1) {  // la variable est de type tableau
	    data_node.setAttrValue("array-size",String::format("{0}", array_size));
	}
	return(data_node);

}

/*---------------------------------------------------------------------------*/
template<typename T>
void IXM4DataNode<T>::
_writeHDFFile(Array<T> & data, const Arcane::String & hdata_name, const Arcane::String & directory_name)
{

	Hdf5Utils::StandardTypes m_hdf5_types;
	Hdf5Utils::HFile m_hdf_file;

	SharedArray<String> str_array;
	hdata_name.split(str_array,':');
	Arcane::String file_name = str_array[0];
	Arcane::String file_position = str_array[1].substring(1); // Curiosite HDF a signaler : il ne faut pas de / en debut de position dans le fichier...

	//! the hdf file has been created before
	m_hdf_file.openAppend(Arcane::String::concat(directory_name,"/",file_name));

	Hdf5Utils::StandardArrayT<T> v(m_hdf_file.id(),file_position);

	v.write(m_hdf5_types, data);
}


/*---------------------------------------------------------------------------*/
/*template<>
inline void IXM4DataNode<Real3>::
_writeHDFFile(Array<Real3> & data, const Arcane::String & hdata_name, const Arcane::String & directory_name)
{

	Hdf5Utils::StandardTypes m_hdf5_types;
	Hdf5Utils::HFile m_hdf_file;

	Array<String> str_array;
	hdata_name.split(str_array,':');
	Arcane::String file_name = str_array[0];
	Arcane::String file_position = str_array[1].substring(1); // Curiosite HDF a signaler : il ne faut pas de / en debut de position dans le fichier...

	//! the hdf file have been created before
	m_hdf_file.openAppend(Arcane::String::concat(directory_name,"/",file_name));

	// convert real3 array into real array
	Array<Real> realdata;
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
}*/
