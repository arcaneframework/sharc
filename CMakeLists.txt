cmake_minimum_required(VERSION 3.2.3)

project(ArcaneDemo VERSION 0.1)

# pour faciliter l'appel de l'outillage
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/common/ArcaneInfra/build-system/arcgeosim)
set(USE_LANGUAGE_GUMP ON)
set(USE_LANGUAGE_LAW ON)

message(status "Arcane_DIR : ${Arcane_DIR}")
  
# chargement du système de compilation
include(LoadArcGeoSim)
  
# ici on met les packages, langages, metas, options...
  
# activation des strong options (uniquement avec le nouveau systeme de compilation)
loadMeta(NAME strong_options)
loadMeta(NAME alien20)
loadMeta(NAME arcane3)
add_definitions(-DUSE_ARCANE_V3)

# affichage des informations
include(PrintArcGeoSimInformations)

logStatus("Loading ArcGeoSim components...")

set(common/ArcGeoSim/src/ArcGeoSim/Mesh/AccessorToolsMng ON)

add_subdirectory(common/ArcGeoSim/src/ArcGeoSim/Appli)
add_subdirectory(common/ArcGeoSim/src/ArcGeoSim/Utils)
add_subdirectory(common/ArcGeoSim/src/ArcGeoSim/Time)
add_subdirectory(common/ArcGeoSim/src/ArcGeoSim/Mesh)
add_subdirectory(common/ArcGeoSim/src/ArcGeoSim/Numerics)
add_subdirectory(common/ArcGeoSim/src/ArcGeoSim/Physics)
add_subdirectory(common/ArcGeoSim/src/ArcGeoSim/Tests)

logStatus("Loading ArcaneDemo components...")

add_subdirectory(src)

logStatus("Loading ArcaneDemo.exe...")

add_subdirectory(src/Main)

logStatus("Loading tests...")

add_subdirectory(test)

# finalisation du système de compilation
include(FinishArcGeoSim)
