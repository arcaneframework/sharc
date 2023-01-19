include_directories(${GTEST_INCLUDE_DIRS})

list(APPEND TEST_SRC
        gtest_main.cpp
        gtest_RelativePermeabilityPowerLaw.cpp
        )

add_executable(gtest_main.exe ${TEST_SRC})

target_link_libraries(gtest_main.exe
        ShArc
        ArcGeoSim_appli
        ArcGeoSim_utils
        ArcGeoSim_mesh
        ArcGeoSim_numerics
        ArcGeoSim_time
        ArcGeoSim_physics
        ArcGeoSim_test
        ${GTEST_LIBRARIES}
        )

if(${CMAKE_VERSION} VERSION_GREATER "3.10")
    gtest_discover_tests(gtest_main.exe PROPERTIES LABELS validation DISCOVERY_TIMEOUT 60)
else()
    gtest_add_tests(gtest_main.exe "" AUTO)
endif()
