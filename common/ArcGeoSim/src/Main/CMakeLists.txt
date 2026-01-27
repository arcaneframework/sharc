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

if(TARGET alien AND WIN32 AND ARCANEFRAMEWORK_ROOT)
    linkDynLibraries(ArcGeoSim.exe alien_ifpen_solvers)
endif()

commit(ArcGeoSim.exe)