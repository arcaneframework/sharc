createLibrary(ArcGeoSim_appli)

generateCMakeLists(ArcGeoSim_appli)

commit(ArcGeoSim_appli DESTINATION Appli)

if (ARCGEOSIM_EXPORT_TARGET)
    target_link_libraries(arcgeosim_full INTERFACE ArcGeoSim_appli)
    add_library(ArcGeoSim::ArcGeoSim_appli ALIAS ArcGeoSim_appli)
endif()

install(DIRECTORY .
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ArcGeoSim/Appli
        FILES_MATCHING PATTERN "*.h"
        )
