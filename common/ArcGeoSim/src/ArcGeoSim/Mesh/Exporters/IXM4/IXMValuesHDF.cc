/*
 * IxmValuesHDF.cc
 *
 *  Created on: May 30, 2012
 *      Author: mesriy
 */

#include "arcane/utils/Array.h"
#include "arcane/utils/StringBuilder.h"
#include "arcane/IXmlDocumentHolder.h"
#include "arcane/IIOMng.h"
#include "arcane/IMesh.h"
#include "arcane/IParallelMng.h"
#include "arcane/Directory.h"
#include "arcane/utils/ITraceMng.h"

#include "IXMValuesHDF.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IXMValuesHDF::IXMValuesHDF(const String& directory_name)
  : m_directory_name(directory_name)
  , m_parallel_mng(NULL)
{
}

/*---------------------------------------------------------------------------*/

IXMValuesHDF::IXMValuesHDF(IParallelMng* pm,const String& directory_name,
                       const String& file_name)
  : m_parallel_mng(pm),
    m_directory_name(directory_name), m_file_name(file_name)
  {
  }

/*---------------------------------------------------------------------------*/

IXMValuesHDF::~IXMValuesHDF() {
}

/*---------------------------------------------------------------------------*/

void IXMValuesHDF::beginWrite()
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

void
IXMValuesHDF::splitString(const String& str,Array<String>& str_array,char c)
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
/*---------------------------------------------------------------------------*/

template<typename T>
void IXMValuesHDF::read(SharedArray<T> anArray){
//...
}
