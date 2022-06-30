# -*- cmake -*-

# driver pour windows 
# NB: d�pendance � cygwin ...
if(WIN32)
  set(SCRIPT_DRIVER c:/cygwin64/bin/sh.exe)
endif()

set(TEST_VALI_DIR ${PROJECT_BINARY_DIR}/test/vali)
file(MAKE_DIRECTORY ${TEST_VALI_DIR})
set(TEST_OUTPUT_ROOT_DIR ${PROJECT_BINARY_DIR}/test/outputs)
file(MAKE_DIRECTORY ${TEST_OUTPUT_ROOT_DIR})

#-----------------------------------------------------------
# DEFINITIONS
#-----------------------------------------------------------

#if(NOT TESTING_ROOT)
#  set(TESTING_ROOT ${PROJECT_ROOT})
#  set(FORCE_EMBEDDED FALSE)
#else()
#  set(FORCE_EMBEDDED TRUE)
#  set(TESTING_FILTER "yes")
#endif()

#set(CODE_NAME ${PROJECT_NAME})

# Main ArcGeoSim test
macro(_arcgeosim_add_test)
  # More infos in http://www.cmake.org/cmake/help/v2.8.8/cmake.html#section_PropertiesonTests
  # CDash example : http://cdash.inria.fr/CDash/
  
  set(options WILL_FAIL RUN_SERIAL SEQUENTIAL)
  set(oneValueArgs TEST_NAME CONFIG_FILE CASE_FILE CASE_DIR)
  list(APPEND oneValueArgs MPI TIMEOUT ENVIRONMENT RESOURCE_LOCK RESTART)
  set(multiValueArgs CONFIGURATIONS)
  
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  if(ARGS_UNPARSED_ARGUMENTS)
    logFatalError("unparsed arguments '${ARGS_UNPARSED_ARGUMENTS}'")
  endif()

  if(NOT ARGS_TEST_NAME)
    logFatalError("_arcgeosim_add_test needs TEST_NAME")
  endif()

  if(NOT ARGS_CONFIG_FILE)
    logFatalError("_arcgeosim_add_test needs CONFIG_FILE")
  endif()

  if(NOT ARGS_CASE_FILE)
    logFatalError("_arcgeosim_add_test needs CASE_FILE")
  endif()

  if(NOT ARGS_CASE_DIR)
    logFatalError("_arcgeosim_add_test needs ARGS_CASE_DIR")
  endif()

  if(ARGS_MPI)
    if(ARGS_SEQUENTIAL)
      logFatalError("MPI and SEQUENTIAL can't be used in same test")
    endif()
    string(REGEX MATCH "^[1-9][0-9]*$" CURRENT_ARG_IS_NUMBER ${ARGS_MPI})
    if(NOT CURRENT_ARG_IS_NUMBER)
      logFatalError("MPI argument is not a positive number : ${ARGS_MPI}")
    endif()
    set(OPT_RUN_MODE Mpi)
    set(OPT_PROCESSORS ${ARGS_MPI})
  else() # d�faut
    set(OPT_RUN_MODE Seq)
    set(OPT_PROCESSORS 1)
  endif()

  
  if(ARGS_ENVIRONMENT)
    set(OPT_ENVIRONMENT ${ARGS_ENVIRONMENT})
  else()
    unset(OPT_ENVIRONMENT)
  endif()

  if(ARGS_RESOURCE_LOCK)
    set(OPT_RESOURCE_LOCK ${ARGS_RESOURCE_LOCK})
  endif()

  if(ARGS_TIMEOUT)
    string(REGEX MATCH "^[1-9][0-9]*$" CURRENT_ARG_IS_NUMBER ${ARGS_TIMEOUT})
    if(NOT CURRENT_ARG_IS_NUMBER)
      logFatalError("TIMEOUT argument is not a positive number : ${ARGS_TIMEOUT}")
    endif()
    set(OPT_TIMEOUT ${ARGS_TIMEOUT})
  else()
    set(OPT_TIMEOUT 120)
  endif()

  if(ARGS_RESTART)
    string(REGEX MATCH "^[1-9][0-9]*$" CURRENT_ARG_IS_NUMBER ${ARGS_RESTART})
    if(NOT CURRENT_ARG_IS_NUMBER)
      logFatalError("RESTART argument is not a positive number : ${ARGS_RESTART}")
    endif()
    set(OPT_RESTART ${ARGS_RESTART})
  else()
    set(OPT_RESTART 0)
  endif()

  if(ARGS_WILL_FAIL)
    set(OPT_WILL_FAIL "true")
  else()
    set(OPT_WILL_FAIL "false")
  endif()

  if(ARGS_RUN_SERIAL)
    set(ARGS_RUN_SERIAL "true")
  else()
    set(ARGS_RUN_SERIAL "false")
  endif()

  set(FULL_TEST_NAME)
  if(${OPT_RUN_MODE} STREQUAL "Seq")
    set(FULL_TEST_NAME "${ARGS_TEST_NAME}")
  elseif(${OPT_RUN_MODE} STREQUAL "Mpi")
    set(FULL_TEST_NAME "${ARGS_TEST_NAME}_${OPT_PROCESSORS}proc")
  else()
    logFatalError("'${OPT_RUN_MODE}' run mode not supported")
  endif()

  if(FORCE_EMBEDDED)
    set(OPT_RUN_MODE "Embedded")
  endif(FORCE_EMBEDDED)

  set(TEST_UNITARY_OUTPUT_DIR ${TEST_OUTPUT_ROOT_DIR}/${FULL_TEST_NAME})
  # creation du repertoire pour les outputs du test
  file(MAKE_DIRECTORY ${TEST_UNITARY_OUTPUT_DIR})
  if(NOT TESTING_FILTER OR ${ARGS_CONFIG_FILE} STREQUAL ${PROJECT_NAME})
    
    if(NOT ${NewBuildSystem})
	    add_test(
			  NAME ${FULL_TEST_NAME}
			  COMMAND ${SCRIPT_DRIVER} ${PROJECT_SOURCE_DIR}/common/ArcaneInfra/bin/ArcaneTest.sh
				${ARGS_TEST_NAME}
				${FULL_TEST_NAME}
				${PROJECT_SOURCE_DIR} 
				${PROJECT_BINARY_DIR}
				${PROJECT_BIN}
				${PROJECT_NAME}
				${ARGS_CONFIG_FILE} 
				${ARGS_CASE_FILE} 
				${TEST_VALI_DIR}
				${OPT_RUN_MODE}
				${OPT_PROCESSORS}
				${OPT_RESTART}
				)
	  else()
	    add_test(
			  NAME ${FULL_TEST_NAME}
			  COMMAND ${SCRIPT_DRIVER} ${PROJECT_SOURCE_DIR}/common/ArcaneInfra/bin/ArcaneTest.sh
				${ARGS_TEST_NAME}
				${FULL_TEST_NAME}
				${PROJECT_SOURCE_DIR} 
				${PROJECT_BINARY_DIR}
				$<TARGET_FILE:${PROJECT_NAME}.exe>
				${PROJECT_NAME}
				${ARGS_CONFIG_FILE} 
				${ARGS_CASE_FILE} 
				${TEST_VALI_DIR}
				${OPT_RUN_MODE}
				${OPT_PROCESSORS}
				${OPT_RESTART}
				)
	  endif()
    
    set_tests_properties(${FULL_TEST_NAME} PROPERTIES WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/${ARGS_CASE_DIR})
    set_tests_properties(${FULL_TEST_NAME} PROPERTIES TIMEOUT ${OPT_TIMEOUT})
    set_tests_properties(${FULL_TEST_NAME} PROPERTIES PROCESSORS ${OPT_PROCESSORS})
    
    set_tests_properties(${FULL_TEST_NAME} PROPERTIES ENVIRONMENT ARCANE_OUTPUT_ROOT_PATH=${TEST_UNITARY_OUTPUT_DIR})
    set_property(TEST ${FULL_TEST_NAME} APPEND PROPERTY ENVIRONMENT ARCANE_PARALLEL_OUTPUT_PREFIX=${TEST_UNITARY_OUTPUT_DIR})

    if(ARGS_CONFIGURATIONS)
      set_tests_properties(${FULL_TEST_NAME} PROPERTIES LABELS "${ARGS_CONFIGURATIONS}")
    endif()
	  if(WIN32)
	    # ajout du chemin des dlls au PATH
	    set(PATH ${BUILDSYSTEM_DLL_COPY_DIRECTORY})
	    list(APPEND PATH $ENV{PATH})
      string(REPLACE ";" "\\;" PATH "${PATH}")
	    string(REPLACE "/" "\\" PATH "${PATH}")
	    set_property(TEST ${FULL_TEST_NAME} APPEND PROPERTY ENVIRONMENT "PATH=${PATH}")
	    if(OPT_ENVIRONMENT)
        set_property(TEST ${FULL_TEST_NAME} APPEND PROPERTY ENVIRONMENT ${OPT_ENVIRONMENT})
      endif()
	  else()
      if(OPT_ENVIRONMENT)
        set_property(TEST ${FULL_TEST_NAME} APPEND PROPERTY ENVIRONMENT ${OPT_ENVIRONMENT})
      endif()
	  endif()
	  if(OPT_WILL_FAIL)
      set_tests_properties(${FULL_TEST_NAME} PROPERTIES WILL_FAIL ${OPT_WILL_FAIL})
    endif()
    if(OPT_RUN_SERIAL)
      set_tests_properties(${FULL_TEST_NAME} PROPERTIES RUN_SERIAL ${OPT_RUN_SERIAL})
    endif()
    if(OPT_RESOURCE_LOCK)
      set_tests_properties(${FULL_TEST_NAME} PROPERTIES RESOURCE_LOCK ${OPT_RESOURCE_LOCK})
    endif()
  else()
    message(STATUS "Skip non ${PROJECT_NAME} test case : ${ARGS_TEST_NAME}")
  endif()

endmacro()


#-----------------------------------------------------------
# testing macros
#-----------------------------------------------------------

macro(ARCGEOSIM_ADD_TEST test_name config_file case_dir case_file)
  _arcgeosim_add_test(
    TEST_NAME   ${test_name} 
    CONFIG_FILE ${config_file} 
    CASE_DIR    ${case_dir} 
    CASE_FILE   ${case_file}
    ${ARGN}
    )
endmacro()

# Add a sequential test
macro(ARCANE_ADD_TEST_SEQUENTIAL test_name config_file case_dir case_file)
  _arcgeosim_add_test(
    TEST_NAME   ${test_name} 
    CONFIG_FILE ${config_file} 
    CASE_DIR    ${case_dir} 
    CASE_FILE   ${case_file}
    SEQUENTIAL
    )
endmacro()

macro(ARCANE_ADD_TEST_SEQUENTIAL_LONGTIME test_name config_file case_dir case_file timeout)
  _arcgeosim_add_test(
    TEST_NAME   ${test_name} 
    CONFIG_FILE ${config_file} 
    CASE_DIR    ${case_dir} 
    CASE_FILE   ${case_file}
    TIMEOUT     ${timeout}
    SEQUENTIAL
    )
endmacro()

# add a standard test
macro(ARCANE_ADD_TEST_PARALLEL test_name config_file case_dir case_file nproc)
  _arcgeosim_add_test(
    TEST_NAME   ${test_name} 
    CONFIG_FILE ${config_file} 
    CASE_DIR    ${case_dir} 
    CASE_FILE   ${case_file}
    MPI         ${nproc}
    )
endmacro()

macro(ARCANE_ADD_TEST_PARALLEL_LONGTIME test_name config_file case_dir case_file nproc timeout)
  _arcgeosim_add_test(
    TEST_NAME   ${test_name} 
    CONFIG_FILE ${config_file} 
    CASE_DIR    ${case_dir} 
    CASE_FILE   ${case_file}
    MPI         ${nproc} 
    TIMEOUT     ${timeout}
    )
endmacro()

macro(ARCANE_ADD_TEST test_name config_file case_dir case_file)
  ARCANE_ADD_TEST_SEQUENTIAL(${test_name} ${config_file} ${case_dir} ${case_file})
  ARCANE_ADD_TEST_PARALLEL(${test_name} ${config_file} ${case_dir} ${case_file} 1)
  ARCANE_ADD_TEST_PARALLEL(${test_name} ${config_file} ${case_dir} ${case_file} 4)
endmacro()

macro(ARCANE_ADD_TEST_LONGTIME test_name config_file case_dir case_file timeout)
  ARCANE_ADD_TEST_SEQUENTIAL_LONGTIME(${test_name} ${config_file} ${case_dir} ${case_file} ${timeout})
  ARCANE_ADD_TEST_PARALLEL_LONGTIME(${test_name} ${config_file} ${case_dir} ${case_file} 1 ${timeout})
  ARCANE_ADD_TEST_PARALLEL_LONGTIME(${test_name} ${config_file} ${case_dir} ${case_file} 4 ${timeout})
endmacro()

# add a convergence test
#macro(ARCANE_ADD_CONVERGENCE_TEST test_name config_file test_dir test_dir_xml test_xml nproc)
#  set(SEQUENTIAL ${nproc} SMALLER 2)
#  set(FULL_TEST_NAME "${test_name}_${nproc}proc")
#  if(SEQUENTIAL OR MPI_FOUND EQUAL "YES")
#  ADD_TEST(
#    NAME ${FULL_TEST_NAME}
#    COMMAND 
#    ${SCRIPT_DRIVER} ${ARCANEINFRA_BIN_DIR}/convergenceTest.sh
#    ${test_name}
#    ${FULL_TEST_NAME}
#    ${TESTING_ROOT}
#    ${CMAKE_BINARY_DIR}
#    ${PROJECT_BIN}
#    ${CODE_NAME}
#    ${config_file}
#    ${test_dir_xml}
#    ${test_xml}
#    ${TEST_OUTPUT_PATH}
#    ${nproc})
#  set_tests_properties(${FULL_TEST_NAME} PROPERTIES WORKING_DIRECTORY ${PROJECT_ROOT}/${test_dir})
#  set_tests_properties(${FULL_TEST_NAME} PROPERTIES TIMEOUT ${DEFAULT_OPT_TIMEOUT})
#  set_tests_properties(${FULL_TEST_NAME} PROPERTIES PROCESSORS ${nproc})
#  endif(SEQUENTIAL OR MPI_FOUND EQUAL "YES")
#endmacro(ARCANE_ADD_CONVERGENCE_TEST)


function(registerLabelTestInfo test)

  set(options)
  set(oneValueArgs)
  set(multiValueArgs LABELS)
  
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  if(NOT ARGS_LABELS)
    logFatalError("no label defined")
  endif()

  get_property(labels GLOBAL PROPERTY ${PROJECT_NAME}_LABELS)
  list(APPEND labels ${ARGS_LABELS}) 
  list(REMOVE_DUPLICATES labels)
  set_property(GLOBAL PROPERTY ${PROJECT_NAME}_LABELS ${labels})
  foreach(label ${ARGS_LABELS})
    get_property(tests GLOBAL PROPERTY ${label}_TESTS)
    list(APPEND tests ${test}) 
    set_property(GLOBAL PROPERTY ${label}_TESTS ${tests})
  endforeach()
endfunction()

macro(AddTest)
  
  set(options)
  set(oneValueArgs CONFIG FILE NAME MPI TIMEOUT RESTART TIMEOUT_RELEASE TIMEOUT_DEBUG)
  set(multiValueArgs LABELS)
  
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(ARGS_UNPARSED_ARGUMENTS)
    logFatalError("unparsed arguments '${ARGS_UNPARSED_ARGUMENTS}'")
  endif()

  if(NOT ARGS_NAME)
    logFatalError("test should have a name")
  endif()

  if(NOT ARGS_FILE)
    logFatalError("test should have a file")
  else()
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${ARGS_FILE})
      logFatalError("test file does not exist : ${ARGS_FILE}")
    endif()
    get_filename_component(path ${ARGS_FILE} DIRECTORY)
    get_filename_component(name ${ARGS_FILE} NAME)
  endif()
  
  unset(labels)
  unset(config)
  unset(parallel)
  unset(timeout)
  unset(restart)
  
  if(NOT ARGS_CONFIG)
    set(config ${PROJECT_NAME})
  endif()

  if(NOT ARGS_MPI)
    set(parallel SEQUENTIAL)
    list(APPEND labels Sequential)
  else()
    set(parallel MPI ${ARGS_MPI})
    list(APPEND labels Parallel)
  endif()

  if(ARGS_TIMEOUT_RELEASE AND CMAKE_BUILD_TYPE STREQUAL "Release")
    set(ARGS_TIMEOUT ${ARGS_TIMEOUT_RELEASE})
  endif()

  if(ARGS_TIMEOUT_DEBUG AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ARGS_TIMEOUT ${ARGS_TIMEOUT_DEBUG})
  endif()

  if(ARGS_TIMEOUT)
    set(timeout TIMEOUT ${ARGS_TIMEOUT})
    # Removed label Timeout as it might not be usefull
    #    list(APPEND labels Timeout)
    # TIMEOUT value "120" appears twice. If it should be modified, it should be modified everywhere
    if(NOT WIN32)
      if(ARGS_TIMEOUT GREATER 120)
        list(APPEND labels Long)
      endif()
    else()
      if(ARGS_TIMEOUT GREATER 300)
        list(APPEND labels Long)
      endif()
    endif()
  endif()

  if(ARGS_RESTART)
    set(restart RESTART ${ARGS_RESTART})
    list(APPEND labels Restart)
  endif()

  if(ARGS_LABELS)
    list(APPEND labels ${ARGS_LABELS})
    list(REMOVE_DUPLICATES labels)
  endif()
  
  registerLabelTestInfo(${ARGS_NAME} LABELS ${labels})

  ARCGEOSIM_ADD_TEST(${ARGS_NAME} 
    ${config} 
    ${path} 
    ${name} 
    CONFIGURATIONS "${labels}"
    ${parallel}
    ${timeout}
    ${restart}
    )

endmacro()

macro(AddSeqAndParallelTest)
  set(options)
  set(oneValueArgs MPI)
  set(multiValueArgs)
  cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(NOT ARGS_MPI)
    AddTest(${ARGN})
  else()
    set(nbProc ${ARGS_MPI})
    set(unParsedAsList ${ARGN})
    list(FIND unParsedAsList "MPI" MPIIndex)
    MATH(EXPR NbProcsIndex "${MPIIndex}+1")
    list(REMOVE_AT unParsedAsList ${MPIIndex} ${NbProcsIndex})
    AddTest(${unParsedAsList}) # Sequential
    if(nbProc GREATER 0)
      AddTest(MPI 1 ${unParsedAsList}) # Parallel 1 proc
    endif()
    if(nbProc GREATER 1)
      AddTest(MPI ${nbProc} ${unParsedAsList}) # Parallel more than 1 proc
    endif()
  endif()

endmacro()

macro(resumeTestInfos)
  
  logStatus("---------------------------------------------------")
  logStatus("** Testing Informations :")

  get_property(labels GLOBAL PROPERTY ${PROJECT_NAME}_LABELS)
  list(LENGTH labels nb_labels)

  logStatus("** We have defined ${nb_labels} label(s) :")

  foreach(label ${labels})
    get_property(tests GLOBAL PROPERTY ${label}_TESTS)
    list(LENGTH tests nb_tests)

    logStatus(" * ${label} : ${nb_tests} test(s)")

  endforeach()

endmacro()
