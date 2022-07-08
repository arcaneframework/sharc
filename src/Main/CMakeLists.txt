createExecutable(Demonstrator.exe)

addSources(Demonstrator.exe main.cc)
 
linkLibraries(Demonstrator.exe 
  Demonstrator
  ArcGeoSim_appli
  ArcGeoSim_utils
  ArcGeoSim_mesh
  ArcGeoSim_numerics
  ArcGeoSim_time
  ArcGeoSim_physics
  ArcGeoSim_test
  )
#if(TARGET ArcGeoSim_alientools)
#linkLibraries(Demonstrator.exe 
#              ArcGeoSim_alientools
#             )
#endif()
if(TARGET intel)
  linkLibraries(Demonstrator.exe 
    intel
    )
endif()

commit(Demonstrator.exe)
