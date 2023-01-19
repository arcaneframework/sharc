#-----------------------------------------------------------
# TESTS
#-----------------------------------------------------------

# ArcGeoSim tests
message(STATUS " ** Including ArcGeoSim tests")
include(${PROJECT_SOURCE_DIR}/common/ArcGeoSim/test/Gump.txt)

# ArcaneDemo tests
message(STATUS " ** Including ${PROJECT_NAME} tests")

# GTest
message(STATUS " ** Including GTest tests")
add_subdirectory(gTest)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulation
        ArcaneDemo
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow.arc)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationWPC
        ArcaneDemo
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-with-capillary-pressure.arc)

