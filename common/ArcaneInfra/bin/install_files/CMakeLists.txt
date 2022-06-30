project(%PROJECT_NAME% NONE)
#------------------------------------------------------------------------------#
#-- Enforce requirement on cmake version
#-- CMakeLists.txt on project directory must have a cmake_minimum_required
#--   command, but this line prevent from incorrect requirement
#------------------------------------------------------------------------------#
cmake_minimum_required(VERSION 2.8)
enable_testing()

#message(STATUS "Build root directory is ${CMAKE_BINARY_DIR}")
#message(STATUS "Source root directory is ${CMAKE_SOURCE_DIR}")

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR}/data CACHE PATH "Testing directory")
# You can change launcher by setting PROJECT_BIN
set(PROJECT_BIN ${CMAKE_SOURCE_DIR}/../${PROJECT_NAME}.sh)
set(TESTING_ROOT ${CMAKE_SOURCE_DIR}/bin CACHE BIN "Project executable")

# include macros for testing
include(${CMAKE_SOURCE_DIR}/Macros.txt)

# You can bypass test_architecture() configuration with local BUILD_TEST_COMMAND
test_architecture()

#MACRO(BUILD_TEST_COMMAND test_name config_file case_dir case_file)
#	SET(TEST_COMMAND 
#	        ${PROJECT_BIN}
#		-n ${OPT_PROCESSORS}
#	        ${case_file})
#ENDMACRO(BUILD_TEST_COMMAND)

make_directory(${TEST_OUTPUT_PATH})

add_subdirectory(${CMAKE_SOURCE_DIR}/data/test)

