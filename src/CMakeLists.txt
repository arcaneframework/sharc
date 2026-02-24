createLibrary(ShArc)

generateCMakeLists(ShArc)

generateGumpModel(TwoPhaseFlowSimulation/DataModel/ArcRes.gump)

linkLibraries(ShArc arcres)

# pour windows
linkLibraries(ShArc
  ArcGeoSim::ArcGeoSim_physics
  ArcGeoSim::ArcGeoSim_tests
  )

commit(ShArc)
