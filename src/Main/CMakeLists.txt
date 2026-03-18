createExecutable(ShArc.exe)

addSources(ShArc.exe main.cc)

linkLibraries(ShArc.exe
  ShArc
  ArcGeoSim::arcgeosim
  )
if(TARGET alien)
  linkLibraries(ShArc.exe alien)
endif()
if(TARGET intel)
  linkLibraries(ShArc.exe intel)
endif()

commit(ShArc.exe)
