createLibrary(ShArc)

generateCMakeLists(ShArc)

# pour windows
linkLibraries(ShArc
  ArcGeoSim_physics
  ArcGeoSim_test
  )

commit(ShArc)

if(TARGET gump)
  addDirectory(TwoPhaseFlowSimulation/DataModel)
endif()