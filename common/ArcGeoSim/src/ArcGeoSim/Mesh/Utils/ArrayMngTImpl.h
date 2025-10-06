// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_EVOLUTIVEMESH_UTILS_TYPEDDATA_H
#error "Template implementation has to be used with template declaration"
#endif 

#include<iostream>

#include <arcane/utils/FatalErrorException.h>
#include <arcane/SerializeBuffer.h>
#include <arcane/ISerializer.h>
#include <arcane/utils/ValueConvert.h>
#include <arcane/utils/ScopedPtr.h>
#include <arcane/ISerializedData.h>
#include <arcane/IDataFactory.h>
#if ARCANE_VERSION > 30003
#include <arcane/IDataFactoryMng.h>
#endif
#include <arcane/IApplication.h>
#include <arcane/utils/OStringStream.h>

#include "ArcGeoSim/Numerics/Utils/InterpolationUtils.h"
#include "ArcGeoSim/Mesh/Utils/VariableUtils.h"
#include "ArcGeoSim/Utils/HdfReader.h"

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataKey
ArrayMngT<DataType>::
extractDataFromString(const String& data_string)
{
  // Extract string
  SharedArray<DataType> data;
  SharedArray<DataType> array_data(data);
  bool has_error = builtInGetValue(array_data, data_string);
  if (has_error) throw FatalErrorException(A_FUNCINFO,"Cannot build array from string. Exiting");
  return _addData(data);
}

/*---------------------------------------------------------------------------*/

template<>
inline
ArrayMng::DataKey
ArrayMngT<Byte>::
extractDataFromString(const String& data_string)
{
  // Needed specialization cause builtInGetValue cannot extract a Array<Byte>
  // Extract string
  Arcane::SharedArray<bool> bool_data;//(bool_data);
  bool has_error = builtInGetValue(bool_data, data_string);
  if (has_error) throw FatalErrorException(A_FUNCINFO,"Cannot build array from string. Exiting");
  // Copy in a Byte Array
  SharedArray<Byte> data(bool_data.size());
  for (Integer i = 0; i< bool_data.size(); ++i)
    {
      data[i] = bool_data[i];
    }
  return _addData(data);
}

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataKey
ArrayMngT<DataType>::
collectiveExtractDataFromFile(const String& filename, const String& file_position, ArcGeoSim::ParallelUtils& parallel_utils)
{
  SharedArray<DataType> data;
  bool is_parallel = (parallel_utils.parallelMng()->commSize() > 1);
  if (!is_parallel) _readFile(data,filename,file_position);
  else
    {
      Arcane::Integer master_id = parallel_utils.parallelMng()->masterIORank();
      bool is_master = (parallel_utils.parallelMng()->commRank() == master_id);
      if (is_master)
        {
          _readFile(data,filename,file_position);
          parallel_utils.multiBroadcast(data,master_id);
        }
      else parallel_utils.multiBroadcast(data,master_id);
    }

  return _addData(data);
}

/*---------------------------------------------------------------------------*/

template<class DataType>
void
ArrayMngT<DataType>::
_readFile(Array<DataType>& data, const String& filename, const String& file_position)
{
  ArcGeoSim::HdfReader::read<DataType>(data,filename,file_position);
}

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataKey
ArrayMngT<DataType>::
extractSubData(const DataKey& data_key,const Array<Integer>& extracted_value_indexes)
{
  SharedArray<DataType> extracted_data;
  SharedArray<DataType> data = m_data_collection[data_key];
  for (SharedArray<Integer>::const_iterator ite = extracted_value_indexes.begin(); ite != extracted_value_indexes.end(); ++ite)
    {
      if (*ite < data.size())
        extracted_data.add(data[*ite]);
      else
        throw FatalErrorException(A_FUNCINFO,"extracted value index > source value array size.");
    }
  return _addData(extracted_data);

}

/*---------------------------------------------------------------------------*/

template<class DataType>
void
ArrayMngT<DataType>::
deleteData(const DataKey& data_key)
{
  if (m_data_collection.find(data_key) != m_data_collection.end())
    m_data_collection.erase(data_key);
  else
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot delete value since data_key {0} does not exist",data_key));
}

/*---------------------------------------------------------------------------*/

template<class DataType>
Integer
ArrayMngT<DataType>::
dataSize(const DataKey& data_key) const
{
  Integer data_size;
  if (m_data_collection.find(data_key) != m_data_collection.end())
    data_size = m_data_collection.find(data_key)->second.size();
  else
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot return data size since data_key {0} does not exist",data_key));
  return data_size;
}

/*---------------------------------------------------------------------------*/

template<class DataType>
void
ArrayMngT<DataType>::
print(std::ostream& o,const DataKey& data_key) const
{
  typename DataCollection::const_iterator iarray = m_data_collection.find(data_key);
  Arcane::SharedArray<DataType> array;
  if (iarray != m_data_collection.end())
    {
      array = iarray->second;
      o << "--data size = "<< array.size() ;
      o << " --data values = " << array;
    }
  else
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot print data array since data_key {0} does not exist",data_key));

}

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataKey
ArrayMngT<DataType>::
_keyGenerator()
{
  DataKey data_key = String::format("key_{0}",m_counter);
  ++m_counter;
  return data_key;
}

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataKey
ArrayMngT<DataType>::
_addData(const SharedArray<DataType>& data)
{
  DataKey data_key = _keyGenerator();
  if (m_data_collection.find(data_key) != m_data_collection.end())
    throw FatalErrorException(A_FUNCINFO, "DataKey generated already exists. Exiting.");
  else
    m_data_collection[data_key] = data;
  return data_key;
}

/*---------------------------------------------------------------------------*/

template<class DataType>
void
ArrayMngT<DataType>::
fillData(IData* const data, const Int32ConstArrayView& item_indexes, const DataKey& source_value_key) const
{
  // Get Values
  Arcane::SharedArray<DataType> values;

  if (m_data_collection.find(source_value_key) != m_data_collection.end())
    values = m_data_collection.find(source_value_key)->second;
  else
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot fill data since data_key {0} does not exist",source_value_key));

  Arcane::Integer item_size = item_indexes.size();

  // Take into account IArray2Data
  Integer values_size, array_size = 1;// Nb elements to extract (possibly complex types as Real3)
  if (data->dimension() == 2)
    {
      Integer item_number;
      ArcGeoSim::VariableUtils::meshVariableArraySize(data,item_number,array_size);
      values_size = item_size * array_size;
    }
  else values_size = item_size;

  // Check indexes
  if (values.size() != values_size)
    throw FatalErrorException(A_FUNCINFO,Arcane::String::format("For property with value key {4}. Cannot fill data since number of items ({0})* array_size ({1}) not equal to number of given values ({2}. Values {3})."
        ,item_indexes.size(),array_size,values.size(),values, source_value_key));

  // Rque Sdc //
  // On ne peut pas utiliser IData::serialize(buffer,ids) car cela suppose de connaitre la structure du buffer (en particulier les m�ta-donn�es)
  // qui sont de l'ordre de l'impl�mentation. On utilise donc des ISerializedData
  // Procedure de modification d'une variable a partir de son IData (cf ISerializedData.h + utilisation dans arcane/std/BasicReaderWriter.cc)
  // Fin Rque
  // Serialize data : this ISerializedData is const. The Data will be modified using a created ISerializedData
#if ARCANE_VERSION > 30003
  auto sdata = data->createSerializedDataRef(false);
  // 1-Create serialized Data to store new values
  ARCANE_ASSERT((m_subdomain),("ISubdomain required to use TypedData"))
  auto data_factory = m_subdomain->application()->dataFactoryMng();
  auto temp_sdata = arcaneCreateSerializedDataRef(data->dataType(),sdata->memorySize(),
                                                  sdata->nbDimension(),sdata->nbElement(),
                                                  sdata->nbBaseElement(),sdata->isMultiSize(),
                                                  sdata->extents()) ;
#else
  Arcane::ScopedPtrT<const Arcane::ISerializedData> sdata(data->createSerializedData(false));
  // 1-Create serialized Data to store new values
  ARCANE_ASSERT((m_subdomain),("ISubdomain required to use TypedData"))
  Arcane::IDataFactory* data_factory = m_subdomain->application()->dataFactory();
  Arcane::ScopedPtrT<Arcane::ISerializedData> temp_sdata(data_factory->createSerializedData(data->dataType(),sdata->memorySize(),
                                                                                            sdata->nbDimension(),sdata->nbElement(),
                                                                                            sdata->nbBaseElement(),sdata->isMultiSize(),
                                                                                            sdata->dimensions()));
#endif
  // 2-Prepare data to receive temp_sdata values (temp_sdata now points toward data buffer)
  data->allocateBufferForSerializedData(temp_sdata.get());

  // 3-Fill temp_sdata values
  Arcane::ArrayView<DataType> temp_sdata_values(temp_sdata->nbElement(),reinterpret_cast<DataType*> (temp_sdata.get()->buffer().unguardedBasePointer()));
  for (Arcane::Integer index = 0; index < item_size; ++index)
    {
      Arcane::Integer buffer_position = item_indexes[index]*array_size;
      Arcane::Integer values_position = index*array_size;
      for (Arcane::Integer i = 0; i < array_size; ++i) temp_sdata_values[buffer_position+i] = values[values_position+i];
    }

  // 4-Assign temp_sdata values to data
  data->assignSerializedData(temp_sdata.get());

}

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataKey
ArrayMngT<DataType>::
extractData(IData* const data, const Int32ConstArrayView& data_indexes)
{
  SharedArray<DataType> values;
  _extractDataBase(values,data,data_indexes);
  return _addData(values);
}

/*---------------------------------------------------------------------------*/

template<class DataType>
void
ArrayMngT<DataType>::
extractData(DataKey& data_key, Arcane::IData* const data,const Int32ConstArrayView& data_indexes)
{
  if (!data_key.empty() && (m_data_collection.find(data_key) != m_data_collection.end()))
    {
      _extractDataBase(m_data_collection[data_key],data,data_indexes);
    }
  else
    {
      Arcane::SharedArray<DataType> values;
      _extractDataBase(values,data,data_indexes);
      data_key = _addData(values);
    }
}

/*---------------------------------------------------------------------------*/

template<class DataType>
void
ArrayMngT<DataType>::
_extractDataBase(Arcane::SharedArray<DataType> & values, Arcane::IData* const data,const Int32ConstArrayView& data_indexes)
{
  //-Extract values from IData to Serialize buffer
  // Allocate buffer
  SerializeBuffer sbuf;
  sbuf.setMode(ISerializer::ModeReserve);
  data->serialize(&sbuf,data_indexes,0);
  sbuf.allocateBuffer();

  // Store in the buffer data values for wanted items using serialize method
  sbuf.setMode(ISerializer::ModePut);
  data->serialize(&sbuf,data_indexes,0);

  // Store the buffer in a new IData: only way to obtain a serialized data for the element at data_indexes position
  sbuf.setMode(ISerializer::ModeGet);
#if ARCANE_VERSION > 30003
  auto extracted_data = data->cloneRef(); // cloneEmpty won't work for a mesh array variable (no size for dim2)
#else
  ScopedPtrT<IData> extracted_data(data->clone()); // cloneEmpty won't work for a mesh array variable (no size for dim2)
#endif
  extracted_data.get()->resize(data_indexes.size());// Resize only dim1 for a mesh array variable

  // Store in the new IData the extracted elements.
  Arcane::Integer data_index_size = data_indexes.size();
  Arcane::IntegerSharedArray position(data_index_size);
  for (Arcane::Integer i = 0; i < data_index_size; ++i) position[i] = i;

  extracted_data.get()->serialize(&sbuf,position,0);

  // Serialize the new IData in a byte buffer
#if ARCANE_VERSION > 30003
  auto extracted_data_serialized = extracted_data.get()->createSerializedDataRef(false) ;
  // Store byte buffer in an array
  ISerializedData const* sdata_ptr = extracted_data_serialized.get() ;
  const void * ptr = sdata_ptr->bytes().data();
  Integer nb_element = extracted_data_serialized.get()->nbElement();
  ConstArrayView<DataType> values_view(nb_element, (const DataType*) ptr);
  values.copy(values_view) ;
#else
  ScopedPtrT<const ISerializedData> extracted_data_serialized(extracted_data.get()->createSerializedData(false));
  // Store byte buffer in an array
  const void * ptr = extracted_data_serialized.get()->buffer().unguardedBasePointer();
  Integer nb_element = extracted_data_serialized.get()->nbElement();
  ConstArrayView<DataType> values_view(nb_element, (const DataType*) ptr);
  values = values_view;
#endif
}

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataInterpolatedPoint
ArrayMngT<DataType>::
interpolateData(const Real& interpolation_time,
                const DataInterpolationPoint& first_point,
                const DataInterpolationPoint& second_point)
{
  // Store init_time and end_time
  Real init_time = first_point.second;
  Real end_time = second_point.second;

  // Get values
  SharedArray<DataType> init_values;
  SharedArray<DataType> end_values;
  if (m_data_collection.find(first_point.first) != m_data_collection.end())
    init_values = m_data_collection.find(first_point.first)->second;
  else
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot interpolate data since data_key {0} does not exist",first_point.first));

  if (m_data_collection.find(second_point.first) != m_data_collection.end())
    end_values = m_data_collection.find(second_point.first)->second;
  else
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot interpolate data since data_key {0} does not exist",second_point.first));

  // Check number of element
  Integer values_size = init_values.size();
  if (values_size != end_values.size())
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot interpolate data since the two points have a different number of elements : first ({0}) second ({1})",
                                                         values_size,end_values.size()));

  // Do interpolation
  SharedArray<DataType> interpolated_values(values_size);
  // Loop over values
  for (Integer i =0; i < values_size; ++i)
    {
      interpolated_values[i] =
        ArcGeoSim::InterpolationUtils::linearInterpolation<DataType>(interpolation_time,
                                                                     init_time,
                                                                     end_time,
                                                                     init_values[i],
                                                                     end_values[i]);
    }

  return std::make_pair(_addData(interpolated_values),this);
}

/*---------------------------------------------------------------------------*/

template<class DataType>
Arcane::SharedArray<DataType>
ArrayMngT<DataType>::
array(const DataKey& data_key) const
{
  typename DataCollection::const_iterator iarray = m_data_collection.find(data_key);
  Arcane::SharedArray<DataType> array;
  if (iarray != m_data_collection.end())
    {
      array = iarray->second;
    }
  else
    throw FatalErrorException(A_FUNCINFO, String::format("Cannot return data array since data_key {0} does not exist",data_key));
  return array;
}

/*---------------------------------------------------------------------------*/

template<class DataType>
ArrayMng::DataKey
ArrayMngT<DataType>::
registerArray(const Arcane::SharedArray<DataType>& array)
{
  return _addData(array);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
