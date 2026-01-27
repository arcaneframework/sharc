
file(READ "version" ARCGEOSIM_VERSION_STR_FULL)
string(REPLACE "_dev" "" ARCGEOSIM_VERSION ${ARCGEOSIM_VERSION_STR_FULL})
message(STATUS "ArcGeoSimVersion = ${ARCGEOSIM_VERSION}")


math(EXPR ARCGEOSIM_VERSION_NUMERIC "((10000 * ${ShArc_VERSION_MAJOR}) + 100 * ${ShArc_VERSION_MINOR}) + ${ShArc_VERSION_PATCH}")
configure_file(arcgeosim_version.h.in ${CMAKE_BINARY_DIR}/arcgeosim_version.h @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/arcgeosim_version.h DESTINATION include/ArcGeoSim)

if (NOT ARCGEOSIM_EXPORT_TARGET)
  set(ARCGEOSIM_EXPORT_TARGET ArcGeoSimTargets)
endif()




set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(namespace "ArcGeoSim::")


# ----------------------------------------------------------------------------
# Create an interface library 'arcgeosim' that groups all ArcGeoSim components.
add_library(arcgeosim INTERFACE)
target_include_directories(arcgeosim INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>)
install(TARGETS arcgeosim EXPORT ${ARCGEOSIM_EXPORT_TARGET})


# ajout d'un include directories pour trouver les export
target_include_directories(arcgeosim INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
  "${version_config}" COMPATIBILITY AnyNewerVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
  "${ARCGEOSIM_FRAMEWORK_ROOT}/ArcaneInfra/build-system/arcgeosim/ArcGeoSimConfig.cmake.in"
  "${project_config}"
  INSTALL_DESTINATION "${config_install_dir}"
  )

set(ArcGeoSim_project_config "${project_config}" CACHE FILEPATH "ArcGeoSim project config" FORCE)
get_filename_component(ArcGeoSim_config_build_dir "${project_config}" DIRECTORY CACHE)
set(ArcGeoSim_config_install_dir "${config_install_dir}" CACHE STRING "ArcGeoSim config install dir" FORCE)
set(ArcGeoSim_VERSION "${ArcGeoSim_VERSION}" CACHE STRING "ArcGeoSim version" FORCE)

install(FILES "${project_config}" "${version_config}"
  DESTINATION "${config_install_dir}"
)

install(EXPORT ${ARCGEOSIM_EXPORT_TARGET}
  DESTINATION ${config_install_dir}
  NAMESPACE "${namespace}")

if (ARCGEOSIM_EXPORT_TARGET STREQUAL ArcGeoSimTargets)
  message(STATUS "Exporting 'ArcGeoSimTargets'")
  export(EXPORT ${ARCGEOSIM_EXPORT_TARGET} FILE ${ArcGeoSim_config_install_dir}/ArcGeoSimTargets.cmake)
endif()

# ----------------------------------------------------------------------------
# Local Variables:
# tab-width: 2
# indent-tabs-mode: nil
# coding: utf-8-with-signature
# End:

