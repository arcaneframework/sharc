createLibrary(ArcGeoSim_physics)

generateCMakeLists(ArcGeoSim_physics)

commit(ArcGeoSim_physics DESTINATION Physics)

if (ARCGEOSIM_EXPORT_TARGET)
    target_link_libraries(arcgeosim INTERFACE ArcGeoSim_physics)
    add_library(ArcGeoSim::ArcGeoSim_physics ALIAS ArcGeoSim_physics)
endif()


install(DIRECTORY .
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ArcGeoSim/Physics
        FILES_MATCHING PATTERN "*.h"
        )
