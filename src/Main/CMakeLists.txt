createExecutable(ShArc.exe)

addSources(ShArc.exe main.cc)

if(TARGET ArcGeoSim::arcgeosim)
linkLibraries(ShArc.exe
  ShArc
  ArcGeoSim::arcgeosim
  )
else()
linkLibraries(ShArc.exe
  ShArc
  ArcGeoSim::ArcGeoSim_appli
  ArcGeoSim::ArcGeoSim_utils
  ArcGeoSim::ArcGeoSim_mesh
  ArcGeoSim::ArcGeoSim_numerics
  ArcGeoSim::ArcGeoSim_time
  ArcGeoSim::ArcGeoSim_physics
  ArcGeoSim::ArcGeoSim_tests
/bin/bash: q: command not found
endif()
if(TARGET alien)
  linkLibraries(ShArc.exe alien)
endif()
if(TARGET intel)
  linkLibraries(ShArc.exe intel)
endif()

commit(ShArc.exe)
