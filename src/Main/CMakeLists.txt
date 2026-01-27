createExecutable(ShArc.exe)

addSources(ShArc.exe main.cc)

linkLibraries(ShArc.exe
  ShArc
  ArcGeoSim_appli
  ArcGeoSim_utils
  ArcGeoSim_mesh
  ArcGeoSim_numerics
  ArcGeoSim_time
  ArcGeoSim_physics
  ArcGeoSim_tests
  )
#if(TARGET ArcGeoSim_alientools)
#linkLibraries(ShArc.exe
#              ArcGeoSim_alientools
#             )
#endif()
if(TARGET intel)
  linkLibraries(ShArc.exe
    intel
    )
endif()

commit(ShArc.exe)
