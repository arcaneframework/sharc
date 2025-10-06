createExecutable(ArcGeoSim.exe)

addSources(ArcGeoSim.exe main.cc)
 
linkLibraries(ArcGeoSim.exe 
  ArcGeoSim_appli
  ArcGeoSim_utils
  ArcGeoSim_mesh
  ArcGeoSim_numerics
  ArcGeoSim_time
  ArcGeoSim_physics
  ArcGeoSim_test
  )
if(TARGET ArcGeoSim_alientools)
linkLibraries(ArGeoSim.exe 
              ArcGeoSim_alientools
             )
endif()
if(TARGET intel)
  linkLibraries(ArcGeoSim.exe 
    intel
    )
endif()

commit(ArcGeoSim.exe)