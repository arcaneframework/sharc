createLibrary(ArcGeoSim_utils)
    
generateCMakeLists(ArcGeoSim_utils)

if(TARGET force-link-google-prof)
  linkLibraries(ArcGeoSim_utils  
    googleprof
  )
endif()

commit(ArcGeoSim_utils)
