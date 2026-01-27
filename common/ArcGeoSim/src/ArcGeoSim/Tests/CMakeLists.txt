createLibrary(ArcGeoSim_tests)

generateCMakeLists(ArcGeoSim_tests)

# pour windows
linkLibraries(ArcGeoSim_tests 
  ArcGeoSim_appli	
  ArcGeoSim_utils 
  ArcGeoSim_mesh
  )

commit(ArcGeoSim_tests DESTINATION Tests)

if (ARCGEOSIM_EXPORT_TARGET)
    target_link_libraries(arcgeosim INTERFACE ArcGeoSim_tests)
    add_library(ArcGeoSim::ArcGeoSim_tests ALIAS ArcGeoSim_tests)
endif()

install(DIRECTORY .
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ArcGeoSim/Tests
        FILES_MATCHING PATTERN "*.h"
)