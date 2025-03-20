createLibrary(ArcGeoSim_alientools)

generateCMakeLists(ArcGeoSim_alientools)

# pour windows
linkLibraries(ArcGeoSim_alientools  
  ArcGeoSim_appli 
  ArcGeoSim_utils 
  ArcGeoSim_mesh 
  )

if(TARGET intel)
  linkLibraries(ArcGeoSim_alientools  
    intel
    )
endif()

if(TARGET fortran)
  linkLibraries(ArcGeoSim_alientools  
    fortran
    )
endif()

if(TARGET fftw3)
  linkLibraries(ArcGeoSim_alientools  
    fftw3
    )
endif()
  
commit(ArcGeoSim_alientools)