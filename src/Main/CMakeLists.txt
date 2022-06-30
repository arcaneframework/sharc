createExecutable(ArcaneDemo.exe)

addSources(ArcaneDemo.exe main.cc)
 
linkLibraries(ArcaneDemo.exe 
  ArcaneDemo
  ArcGeoSim_appli
  ArcGeoSim_utils
  ArcGeoSim_mesh
  ArcGeoSim_numerics
  ArcGeoSim_time
  ArcGeoSim_physics
  ArcGeoSim_test
  )
#if(TARGET ArcGeoSim_alientools)
#linkLibraries(ArcaneDemo.exe 
#              ArcGeoSim_alientools
#             )
#endif()
if(TARGET intel)
  linkLibraries(ArcaneDemo.exe 
    intel
    )
endif()

commit(ArcaneDemo.exe)
