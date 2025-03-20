createLibrary(ArcGeoSim_mesh)

generateCMakeLists(ArcGeoSim_mesh)

# pour windows
linkLibraries(ArcGeoSim_mesh
  ArcGeoSim_utils 
  ArcGeoSim_time
  )

if(TARGET metis)
linkLibraries(ArcGeoSim_mesh
              metis)
endif()

commit(ArcGeoSim_mesh DESTINATION Mesh)

if (ARCGEOSIM_EXPORT_TARGET)
    target_link_libraries(arcgeosim_full INTERFACE ArcGeoSim_mesh)
    add_library(ArcGeoSim::ArcGeoSim_mesh ALIAS ArcGeoSim_mesh)
endif()


install(DIRECTORY .
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ArcGeoSim/Mesh
        FILES_MATCHING PATTERN "*.h"
        )

#install(TARGETS ArcGeoSim_mesh EXPORT ${ARCGEOSIM_EXPORT_TARGET}
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        )

