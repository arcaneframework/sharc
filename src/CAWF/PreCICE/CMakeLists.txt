

add_subdirectory(FEMUtils)

createLibrary(CAWF)

generateCMakeLists(CAWF)

target_include_directories(CAWF PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# pour windows
linkLibraries(CAWF ArcGeoSim::ArcGeoSim_physics
                   ArcGeoSim::ArcGeoSim_tests)

if(TARGET FEMUtils)
  linkLibraries(CAWF FEMUtils)
endif(FemUtils)

commit(CAWF)
