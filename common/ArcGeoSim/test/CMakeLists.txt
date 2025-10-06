#-----------------------------------------------------------
# TESTS
#-----------------------------------------------------------

# ArcGeoSim tests
message(STATUS " ** Including ArcGeoSim tests")
set(ARCGEOSIM_TEST_DIR $ENV{ARCGEOSIM_TEST_DIR})
# INCLUDE(${ARCGEOSIM_TEST_PATH}/LUSolver.txt)
include(${PROJECT_SOURCE_DIR}/test/AnyItem.txt) 
include(${PROJECT_SOURCE_DIR}/test/AppService.txt)
include(${PROJECT_SOURCE_DIR}/test/AuDi.txt)
include(${PROJECT_SOURCE_DIR}/test/DivKGradTwoPointsService.txt)
include(${PROJECT_SOURCE_DIR}/test/Expressions.txt)
include(${PROJECT_SOURCE_DIR}/test/GenericItemExtractor.txt)
include(${PROJECT_SOURCE_DIR}/test/Gump.txt)
include(${PROJECT_SOURCE_DIR}/test/Interpolator.txt)
include(${PROJECT_SOURCE_DIR}/test/Law.txt)
include(${PROJECT_SOURCE_DIR}/test/LUSolver.txt)
include(${PROJECT_SOURCE_DIR}/test/LinearAlgebra2.txt)
include(${PROJECT_SOURCE_DIR}/test/MakeDelegatedPtr.txt)
include(${PROJECT_SOURCE_DIR}/test/ModuleMaster.txt)
include(${PROJECT_SOURCE_DIR}/test/StrongOptions.txt)
include(${PROJECT_SOURCE_DIR}/test/NNInference.txt)

# To remove when NumericalModel along with TimeUtils has moved from ArcGeoSim 
#include(${PROJECT_SOURCE_DIR}/test/MultiThreadNonLinearTest.txt)
#include(${PROJECT_SOURCE_DIR}/test/NonLinearSolver.txt)
#include(${PROJECT_SOURCE_DIR}/test/MonoPhasiqueModel.txt)

# To remove when sources are migrated in static fault (work in progress)
#include(${PROJECT_SOURCE_DIR}/test/ObservableProperty.txt)

if (NOT WIN32)
include(${PROJECT_SOURCE_DIR}/test/Partitioner.txt)
endif()


