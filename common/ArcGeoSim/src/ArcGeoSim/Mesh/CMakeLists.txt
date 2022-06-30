createLibrary(ArcGeoSim_mesh)

generateCMakeLists(ArcGeoSim_mesh)

# pour windows
linkLibraries(ArcGeoSim_mesh
  ArcGeoSim_utils 
  ArcGeoSim_time
  )

commit(ArcGeoSim_mesh)