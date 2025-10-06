// -*- C++ -*-
#ifndef ARCGEOSIM_XDMF_READER_TOOLS_H
#define ARCGEOSIM_XDMF_READER_TOOLS_H


#include <boost/shared_ptr.hpp>

#include <arcane/XmlNode.h>
#include <arcane/IXmlDocumentHolder.h>
#include <arcane/utils/ValueConvert.h>
#include <arcane/utils/FatalErrorException.h>
#include <arcane/utils/StringBuilder.h>
#include <arcane/utils/PlatformUtils.h>

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Utils/HdfReader.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
// XDMF reader tools

/*---------------------------------------------------------------------------*/

inline Arcane::String hdfFileName(const Arcane::String& base_file_name, const Arcane::String& mesh_file_directory)
{
  Arcane::String filename;
  // New convention (ASIM192): path in ixm relative to mesh directory => need to concat filename with mesh directory.
  // Test done in the following to ensure compatibility with previous system (< ASIM192, ie filename in ixm is relative to run directory).
  if (Arcane::platform::isFileReadable(base_file_name)) filename = base_file_name; // Compatibility before ASIM192
  else filename = Arcane::String::concat(mesh_file_directory,'/',base_file_name); // New convention ASIM192
  return filename;
}

/*---------------------------------------------------------------------------*/

template <typename DataType>
class XDMFDataNodeReader
{
public:
  XDMFDataNodeReader(const Arcane::XmlNode& data_node,
                    const Arcane::String& error_message,
                    const Arcane::String& directory_name,
                    const bool data_node_can_be_empty = false)
    : m_error_message(error_message)
    , m_directory_name(directory_name)
  {
    Arcane::String format = data_node.attrValue("Format");
    if (format == "XML")
      _readXmlData(data_node, data_node_can_be_empty);
    else
      _readHdf5Data(data_node);
  }

  virtual ~XDMFDataNodeReader(){}


public:
  typedef Arcane::SharedArray<DataType> DataArray;

  DataArray read(){return m_data;}


private:
  Arcane::String m_error_message;
  DataArray m_data;
  Arcane::String m_directory_name;

  void _readXmlData(const Arcane::XmlNode& data_node, const bool data_node_can_be_empty)
  {
    Arcane::String data_string = data_node.value();
    if (data_string.empty()) {
      if (data_node_can_be_empty) return;
      else throw Arcane::FatalErrorException(A_FUNCINFO,String::concat("Data node is empty ",m_error_message));
    }
    bool error = Arcane::builtInGetValue(m_data,data_string);
    if (error) throw Arcane::FatalErrorException(A_FUNCINFO,m_error_message);
  }

  void _readHdf5Data(const Arcane::XmlNode& data_node)
  {
    Arcane::StringSharedArray file_info;
    data_node.value().split(file_info,':');
    Arcane::String file_position = file_info[1];
    Arcane::String filename = hdfFileName(file_info[0],m_directory_name);
    HdfReader::read(m_data,filename,file_position);
  }
};

typedef XDMFDataNodeReader<Arcane::Int64> XDMFInt64DataNodeReader;
typedef XDMFDataNodeReader<Arcane::Int32> XDMFInt32DataNodeReader;
typedef XDMFDataNodeReader<Arcane::Integer> XDMFIntegerDataNodeReader;
typedef XDMFDataNodeReader<Arcane::Real> XDMFRealDataNodeReader;
typedef XDMFDataNodeReader<Arcane::Real3> XDMFReal3DataNodeReader;

/*---------------------------------------------------------------------------*/


END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


#endif /* ARCGEOSIM_ARCGEOSIM_XDMF_READER_TOOLS_H */
