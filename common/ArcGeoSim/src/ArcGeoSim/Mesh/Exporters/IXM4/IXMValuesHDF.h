#ifndef ARCGEOSIM_MESH_EXPORTERS_IXMVALUESHDF_H
#define ARCGEOSIM_MESH_EXPORTERS_IXMVALUESHDF_H
/*
 * IxmValuesHDF.h
 *
 *  Created on: May 30, 2012
 *      Author: mesriy
 */

#include "IXMValues.h"


#include <arcane/utils/TraceAccessor.h>
#include "arcane/utils/Array.h"
#include "arcane/utils/StringBuilder.h"
#include "arcane/IXmlDocumentHolder.h"
#include "arcane/IIOMng.h"
#include "arcane/IMesh.h"
#include "arcane/IParallelMng.h"
#include "arcane/Directory.h"
#include "arcane/utils/ITraceMng.h"
#include <set>


namespace Arcane {
}
using namespace Arcane;


#include <arcane/std/Hdf5Utils.h>


template<typename T>
class  IXMValuesHDF :public IXMValues<T>
{

public :
	//!
	IXMValuesHDF(const String& directory_name);
	//!
	IXMValuesHDF(IParallelMng* pm,const String& directory_name,
			const String& file_name);
	//!
	virtual ~IXMValuesHDF();

	void setDataNode(Arcane::XmlNode data_node);

	//!
	//const String getClassName() { return("IxmValuesHDF"); } ;

	//! Set the name of the Heavy Data Set (if applicable)
	void setHeavyDataName(const String& hdata_name) {this->m_heavy_data_name=hdata_name;};

	//! Get the name of the Heavy Data Set (if applicable)
	String getHeavyDataName() const {return this->m_heavy_data_name;};

	//! Read the Array from the External Representation
	void beginWrite();

	//! Write the Array to the External Representation
	//template<typename T>
	bool write(Array<T> & data) {
		SharedArray<String> str_array;
		const String& fullname=this->getHeavyDataName();

		this->m_data_node.setValue(fullname);

		splitString(fullname,str_array,':');
		Arcane::String file_name = str_array[0];
		Arcane::String file_position = str_array[1].substring(1); // Curiosite HDF a signaler : il ne faut pas de / en debut de position dans le fichier...

		//! the hdf file have been created before
		m_hdf_file.openAppend(Arcane::String::concat(m_directory_name,"/",file_name));

		Hdf5Utils::StandardArrayT<T> v(m_hdf_file.id(),file_position);
		v.write(m_hdf5_types,data);
		//info(5) << "WRITE_XML_DATA len=" << data.size() << " data=" << data.begin() << "__EOF";
		return true;
	}

	bool write(Array<T> & anArray, const Formatter<T>& formatter) {
		    // TO DO
	  	    return true;
	  	}

private :
	//! Extract only the hdf file path
	void splitString(const String& str,Array<String>& str_array,char c);


private:
	Hdf5Utils::StandardTypes m_hdf5_types;
	IParallelMng* m_parallel_mng;
	String m_directory_name;
	String m_file_name;
	Hdf5Utils::HFile m_hdf_file;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
template<typename T>
IXMValuesHDF<T>::IXMValuesHDF(const String& directory_name)
  : m_parallel_mng(NULL)
  , m_directory_name(directory_name)

{
}

/*---------------------------------------------------------------------------*/
template<typename T>
IXMValuesHDF<T>::IXMValuesHDF(IParallelMng* pm,const String& directory_name,
                       const String& file_name)
  : m_parallel_mng(pm),
    m_directory_name(directory_name), m_file_name(file_name)
  {
  }

/*---------------------------------------------------------------------------*/
template<typename T>
IXMValuesHDF<T>::~IXMValuesHDF() {
}

/*---------------------------------------------------------------------------*/
template<typename T>
void IXMValuesHDF<T>::setDataNode(XmlNode data_node)
{
	this->m_data_node=data_node;
}

/*---------------------------------------------------------------------------*/
template<typename T>
void IXMValuesHDF<T>::beginWrite()
{
//  m_parallel_mng->traceMng()->info(4) << "BEGIN WRITE N=" << vars.count() << " INDEX=" << m_index << " directory=" << m_directory_name;
  Directory out_dir(m_directory_name);
  String full_filename = out_dir.file(m_file_name);
//  m_parallel_mng->traceMng()->info(4) << "OUT FILE_NAME=" << full_filename;
  if (m_parallel_mng->isMasterIO()){
    if (true){
      m_hdf_file.openTruncate(full_filename);
    }
    else
      m_hdf_file.openAppend(full_filename);
  }
}
/*---------------------------------------------------------------------------*/
template<typename T>
void IXMValuesHDF<T>::splitString(const String& str,Array<String>& str_array,char c)
{
  const char* str_str = str.localstr();
  Integer offset = 0;
  Integer len = str.len();
  for( Integer i=0; i<len; ++i ){
    if (str_str[i]==c && i!=offset){
      str_array.add(String(str_str+offset,i-offset));
      offset = i+1;
    }
  }
  if (len!=offset)
    str_array.add(String(str_str+offset,len-offset));
}


#endif /* ARCGEOSIM_MESH_EXPORTERS_IXM4_IXMVALUESHDF_H */
