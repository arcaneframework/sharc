// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "HdfReader.h"
/* Author : dechaiss at Tue Nov  6 11:11:36 2012
 * Generated by createNew
 */

#include <arcane/std/Hdf5Utils.h>
#include <arcane/utils/FatalErrorException.h>

/*---------------------------------------------------------------------------*/
// Implementation dans le .cc pour ne pas polluer le code avec les include hdf5 (qui necessite un use hdf5 et un use mpi dans le config)

BEGIN_ARCGEOSIM_NAMESPACE

namespace HdfReader{

template<typename DataType>
void
readBase(Arcane::Array<DataType>& data, const Arcane::String& filename,const String& file_position)
{
  Arcane::Hdf5Utils::HFile file;
  Arcane::Hdf5Utils::StandardTypes types;
  file.openRead(filename);
  if (file.isBad())
    throw Arcane::FatalErrorException(A_FUNCINFO,
                                      Arcane::String::format("Cannot Open hdf file {0} for typed data reading. ", filename));
  Arcane::Hdf5Utils::StandardArrayT<DataType> reader(file.id(),file_position);
  reader.directRead(types,data);
  file.close();
}


template <> void read(Arcane::RealArray& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Real>(data,filename,file_position);}
template <> void read(Arcane::Int64Array& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Int64>(data,filename,file_position);}
template <> void read(Arcane::Int32Array& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Int32>(data,filename,file_position);}
template <> void read(Arcane::Real3Array& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Real3>(data,filename,file_position);}
template <> void read(Arcane::Real3x3Array& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Real3x3>(data,filename,file_position);}
template <> void read(Arcane::Real2Array& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Real2>(data,filename,file_position);}
template <> void read(Arcane::Real2x2Array& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Real2x2>(data,filename,file_position);}
template <> void read(Arcane::ByteArray& data, const Arcane::String& filename, const String& file_position){readBase<Arcane::Byte>(data,filename,file_position);}

}

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
