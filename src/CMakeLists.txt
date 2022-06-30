createLibrary(ArcaneDemo)

generateCMakeLists(ArcaneDemo)

# pour windows
linkLibraries(ArcaneDemo  
#  ArcGeoSim_appli
#  ArcGeoSim_utils
#  ArcGeoSim_mesh
#  ArcGeoSim_time
  ArcGeoSim_physics
  ArcGeoSim_test
  )
#if(TARGET ArcGeoSim_alientools)
#linkLibraries(ArcaneDemo  
#  ArcGeoSim_alientools
#  )
#endif()
if(TARGET gump)
  addDirectory(DiPhasicTest/Gump)
endif()

commit(ArcaneDemo)
