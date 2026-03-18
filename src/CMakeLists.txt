createLibrary(ShArc)

generateCMakeLists(ShArc)

generateGumpModel(TwoPhaseFlowSimulation/DataModel/ArcRes.gump)

linkLibraries(ShArc arcres)

# pour windows
linkLibraries(ShArc
              ArcGeoSim::ArcGeoSim_mesh
              ArcGeoSim::ArcGeoSim_physics
              ArcGeoSim::ArcGeoSim_tests
             )

linkLibraries(ShArc CAWF)

commit(ShArc)
