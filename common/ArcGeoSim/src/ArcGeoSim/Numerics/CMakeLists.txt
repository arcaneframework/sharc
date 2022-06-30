createLibrary(ArcGeoSim_numerics)

generateCMakeLists(ArcGeoSim_numerics)

# pour windows
linkLibraries(ArcGeoSim_numerics  
  ArcGeoSim_appli 
  ArcGeoSim_utils 
  ArcGeoSim_mesh 
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
  
commit(ArcGeoSim_numerics)