createLibrary(ArcGeoSim_time)

generateCMakeLists(ArcGeoSim_time)

# pour windows
linkLibraries(ArcGeoSim_time 
  ArcGeoSim_appli
  )

commit(ArcGeoSim_time)