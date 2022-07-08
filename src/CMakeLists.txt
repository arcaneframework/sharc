createLibrary(Demonstrator)

generateCMakeLists(Demonstrator)

# pour windows
linkLibraries(Demonstrator  
#  ArcGeoSim_appli
#  ArcGeoSim_utils
#  ArcGeoSim_mesh
#  ArcGeoSim_time
  ArcGeoSim_physics
  ArcGeoSim_test
  )
#if(TARGET ArcGeoSim_alientools)
#linkLibraries(Demonstrator  
#  ArcGeoSim_alientools
#  )
#endif()

commit(Demonstrator)
