#-----------------------------------------------------------
# TESTS
#-----------------------------------------------------------

# ArcGeoSim tests
#message(STATUS " ** Including ArcGeoSim tests")
#include(${PROJECT_SOURCE_DIR}/common/ArcGeoSim/test/Gump.txt)

# ShArc tests
message(STATUS " ** Including ${PROJECT_NAME} tests")

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulation
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow.arc)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationWPC
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-with-capillary-pressure.arc)

ARCANE_ADD_TEST_PARALLEL(TwoPhaseFlowSimulation
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow.arc
        2)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10.arc)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-bcgs-ilu0
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-bcgs-ilu0.arc)

if(TARGET hypre)
ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-bcgs-cpr
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-bcgs-cpr.arc)
endif()

#ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-petsc-bcgs-ilu0
#        ShArc
#        test/TwoPhaseFlowSimulation
#        use-case-two-phase-flow-spe10-petsc-bcgs-ilu0.arc)

if(TARGET hypre)
ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-hypre-bcgs-ilu0
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-hypre-bcgs-ilu0.arc)
endif()

if(TARGET trilinos)
ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-tri-bcgs-ilu0
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-tri-bcgs-ilu0.arc)
endif()

if(TARGET htssolver)
ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-hts-bcgs-ilu0
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-hts-bcgs-ilu0.arc)
endif()

if(TARGET ifpsolver)
ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-ifps-bcgs-ilu0
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-ifps-bcgs-ilu0.arc)

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10-ifps-bcgs-cpr
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-ifps-bcgs-cpr.arc)
endif()

ARCANE_ADD_TEST_SEQUENTIAL(TwoPhaseFlowSimulationSPE10Well
        ShArc
        test/TwoPhaseFlowSimulation
        use-case-two-phase-flow-spe10-well.arc)


ARCANE_ADD_TEST_SEQUENTIAL(ExaDiBench-cube3D
        ShArc
        test/ExaDiBench
        bench-cube3d.arc
        )

ARCANE_ADD_TEST_PARALLEL(ExaDiBench-cube3D
        ShArc
        test/ExaDiBench
        bench-cube3d.arc
        2)

#ARCANE_ADD_TEST(ExaDiBench-spe11c
#        ShArc
#        test/ExaDiBench
#        bench-spe11c.arc
#        )

#ARCANE_ADD_TEST(ExaDiBench-spe11c-structured
#        ShArc
#        test/ExaDiBench
#        bench-spe11c-structured.arc
#        )

#ARCANE_ADD_TEST(ExaDiBench-pb
#        ShArc
#        test/ExaDiBench
#        bench-pb.arc
#        )

#ARCANE_ADD_TEST_PARALLEL(ExaDiBench-pb-msh
#        ShArc
#        test/ExaDiBench
#        bench-pb-msh.arc
#        4)

#ARCANE_ADD_TEST(ExaDiBench-ring
#        ShArc
#        test/ExaDiBench
#        bench-ring.arc
#        )

#ARCANE_ADD_TEST(ExaDiBench-ring-msh
#        ShArc
#        test/ExaDiBench
#        bench-ring-msh.arc
#        )
