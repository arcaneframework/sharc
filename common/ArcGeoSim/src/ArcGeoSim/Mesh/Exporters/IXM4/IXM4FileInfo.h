#ifndef ARCGEOSIM_MESH_EXPORTERS_IXM4FILEINFO_H
#define ARCGEOSIM_MESH_EXPORTERS_IXM4FILEINFO_H

#include <arcane/ArcaneTypes.h>
#include <set>

// set of groups already written in HDF file
struct IXM4FileInfo {
  std::set<Arcane::String> supportSet;
};


// IXM4 file format info
enum eIXM4DataFormat
  {
    FI_Hdf, FI_Split_Hdf, FI_Xml
  };

// Mesh write info
enum eIXMMeshWriteInfo
  {
    WI_Once, WI_MeshByEvent, WI_IncrementMeshByEvent, WI_MeshByMesh, WI_Never
  };




#endif /* ARCGEOSIM_MESH_EXPORTERS_IXM4FILEINFO_H */
