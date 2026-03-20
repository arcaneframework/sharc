createLibrary(Main_tests)

generateCMakeLists(Main_tests)

# pour windows
linkLibraries(Main_tests
  ArcGeoSim_appli
  ArcGeoSim_utils
  ArcGeoSim_mesh
  ArcGeoSim_tests
  ArcGeoSim_numerics
  ArcGeoSim_physics
  )

# houps not consistent with config.xml (pb with langage="Gump" to patch or full cmake)
if(TARGET gump)
  generateGumpModel(ServiceTesters/GumpTester/Example/Example.gump)
  generateGumpModel(ServiceTesters/LawTester/LawContribution/ContributionModel.gump)
  linkLibraries(Main_tests example)
  linkLibraries(Main_tests contributionmodel)
endif()

commit(Main_tests)
