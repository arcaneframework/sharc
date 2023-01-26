#-----------------------------------------------------------
# TESTS
#-----------------------------------------------------------

# ArcGeoSim tests
message(STATUS " ** Including ArcGeoSim tests")
include(${PROJECT_SOURCE_DIR}/common/ArcGeoSim/test/Gump.txt)

# ArcaneDemo tests
message(STATUS " ** Including ${PROJECT_NAME} tests")

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulation
        ArcaneDemo
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow.arc)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationWPC
        ArcaneDemo
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-with-capillary-pressure.arc)

ARCANE_ADD_TEST_PARALLEL(TwoPhaseFlowSimulation
        ArcaneDemo
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow.arc
        2)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10
        ArcaneDemo
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10.arc)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10Well
        ArcaneDemo
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-well.arc)

