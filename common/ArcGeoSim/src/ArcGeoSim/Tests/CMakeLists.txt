createLibrary(ArcGeoSim_test)

generateCMakeLists(ArcGeoSim_test)

# pour windows
if(WIN32)
linkLibraries(ArcGeoSim_test  
  ArcGeoSim_utils 
  ArcGeoSim_mesh 
  )
  
  if(TARGET ArcGeoSim_alientools)
    linkLibraries(ArcGeoSim_test  
	  ArcGeoSim_alientools
	)
  endif()
  if(TARGET ArcGeoSim_numerics_la)
    linkLibraries(ArcGeoSim_test  
	  ArcGeoSim_numerics_la
	)
  endif()
endif()

# si les services testers sont presents, il y a des dependances
# supplementaires
# TODO splitter les librairies pour supprimer le patch suivant
if(NOT DEFINED common/ArcGeoSim/src/ArcGeoSim/Tests/ServiceTesters)
  set(have_tests ON)
endif()
if(${common/ArcGeoSim/src/ArcGeoSim/Tests/ServiceTesters})
  set(have_tests ON)
endif()
if(${have_tests})
  # pour windows
if(WIN32)
  linkLibraries(ArcGeoSim_test  
    ArcGeoSim_numerics 
    ArcGeoSim_physics
    )
	if(TARGET ArcGeoSim_alientools)
    linkLibraries(ArcGeoSim_test  
	  ArcGeoSim_alientools
	)
  endif()
  if(TARGET ArcGeoSim_numerics_la)
    linkLibraries(ArcGeoSim_test  
	  ArcGeoSim_numerics_la
	)
  endif()
endif()
endif()

if(TARGET gump)
  
  generateGumpModel(ServiceTesters/GumpTester/Example/Example.xml)
  
  linkLibraries(ArcGeoSim_test example)

  if(${have_tests})
    linkLibraries(ArcGeoSim_test contributionmodel)
  endif()
endif()

commit(ArcGeoSim_test)
