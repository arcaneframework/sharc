createLibrary(ArcGeoSim_numerics)

generateCMakeLists(ArcGeoSim_numerics)

# pour windows
linkLibraries(ArcGeoSim_numerics  
  ArcGeoSim_appli 
  ArcGeoSim_utils 
  ArcGeoSim_mesh
  #ArcGeoSim_physics
  )

if(TARGET intel)
  linkLibraries(ArcGeoSim_numerics  
    intel
    )
endif()

if(TARGET fftw3)
  linkLibraries(ArcGeoSim_numerics  
    fftw3
    )
endif()
  
commit(ArcGeoSim_numerics DESTINATION Numerics)

if (ARCGEOSIM_EXPORT_TARGET)
    target_link_libraries(arcgeosim_full INTERFACE ArcGeoSim_numerics)
    add_library(ArcGeoSim::ArcGeoSim_numerics ALIAS ArcGeoSim_numerics)
endif()

install(DIRECTORY .
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ArcGeoSim/Numerics
        FILES_MATCHING PATTERN "*.h"
        )
