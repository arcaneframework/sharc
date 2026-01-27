createLibrary(ArcGeoSim_utils)
    
generateCMakeLists(ArcGeoSim_utils)

if(TARGET force-link-google-prof)
  linkLibraries(ArcGeoSim_utils  
    googleprof
  )
endif()

commit(ArcGeoSim_utils DESTINATION Utils)

if (ARCGEOSIM_EXPORT_TARGET)
    target_link_libraries(arcgeosim INTERFACE ArcGeoSim_utils)
    add_library(ArcGeoSim::ArcGeoSim_utils ALIAS ArcGeoSim_utils)
endif()

install(DIRECTORY .
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ArcGeoSim/Utils
        FILES_MATCHING PATTERN "*.h"
        )