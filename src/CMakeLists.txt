createLibrary(ShArc)

generateCMakeLists(ShArc)

generateGumpModel(TwoPhaseFlowSimulation/DataModel/ArcRes.gump)

linkLibraries(ShArc arcres)

# pour windows
linkLibraries(ShArc
  ArcGeoSim_physics
  ArcGeoSim_tests
  )

commit(ShArc)
