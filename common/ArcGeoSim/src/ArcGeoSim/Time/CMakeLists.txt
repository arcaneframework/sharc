createLibrary(ArcGeoSim_time)

generateCMakeLists(ArcGeoSim_time)

# pour windows
linkLibraries(ArcGeoSim_time 
  ArcGeoSim_appli
  )

commit(ArcGeoSim_time DESTINATION Time)

if (ARCGEOSIM_EXPORT_TARGET)
    target_link_libraries(arcgeosim INTERFACE ArcGeoSim_time)
    add_library(ArcGeoSim::ArcGeoSim_time ALIAS ArcGeoSim_time)
endif()

install(DIRECTORY .
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ArcGeoSim/Time
        FILES_MATCHING PATTERN "*.h"
        )