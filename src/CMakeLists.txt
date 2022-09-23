createLibrary(ShArc)

generateCMakeLists(ShArc)

# pour windows
linkLibraries(ShArc
#  ArcGeoSim_appli
#  ArcGeoSim_utils
#  ArcGeoSim_mesh
#  ArcGeoSim_time
  ArcGeoSim_physics
  ArcGeoSim_test
  )
#if(TARGET ArcGeoSim_alientools)
#linkLibraries(ShArc
#  ArcGeoSim_alientools
#  )
#endif()

commit(ShArc)
