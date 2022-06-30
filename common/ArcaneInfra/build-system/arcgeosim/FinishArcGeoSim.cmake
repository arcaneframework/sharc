# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------

# pour la recopie de dlls si besoin
include(common/ArcaneInfra/build-system/arcgeosim/FinishBuildSystem.cmake)

message_separator()

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------

if(TARGET vendor)
  if(NOT TARGET flexlm)
    logStatus("${Yellow}Warning${ColourReset} Using vendor mode without FlexLM")
    message_separator()
  endif()
endif()
  
if(TARGET flexlm)
  if(NOT TARGET vendor)
    logStatus("${Yellow}Warning${ColourReset} Using FlexLM without vendor mode")
    message_separator()
  endif()
  if(TARGET metis)
    logStatus("${Yellow}Warning${ColourReset}Prohibited Metis dependency found using FlexLM")
  endif()
  if(TARGET parmetis)
    logStatus("${Yellow}Warning${ColourReset}Prohibited ParMetis dependency found using FlexLM")
  endif()
endif()

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
