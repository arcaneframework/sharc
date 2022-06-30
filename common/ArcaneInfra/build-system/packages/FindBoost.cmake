#
# Find the BOOST includes and library
#
# This module uses
# BOOST_ROOT
#
# This module defines
# BOOST_FOUND
# BOOST_INCLUDE_DIRS
# BOOST_LIBRARIES
#
# Target boost

if(NOT BOOST_ROOT)
  set(BOOST_ROOT $ENV{BOOST_ROOT})
endif()

if(BOOST_ROOT)
  set(_BOOST_SEARCH_OPTS NO_DEFAULT_PATH)
else()
  set(_BOOST_SEARCH_OPTS)
endif()

# NB: sous linux, les chemins système passent malgré
# l'option Boost_NO_SYSTEM_PATHS...
if(NOT WIN32 AND BOOST_ROOT)
  set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
  set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
endif()

set(Boost_NO_SYSTEM_PATHS          ON) 
set(Boost_USE_STATIC_LIBS          OFF)
set(Boost_USE_MULTITHREADED        ON) 
set(Boost_USE_STATIC_RUNTIME       OFF)
set(Boost_DETAILED_FAILURE_MSG     ON)
set(Boost_PROGRAM_OPTIONS_DYN_LINK ON)
set(Boost_SYSTEM_DYN_LINK          ON)
set(Boost_THREAD_DYN_LINK          ON)
set(Boost_SERIALIZATION_DYN_LINK   ON)
set(Boost_CHRONO_DYN_LINK          ON)
set(Boost_REGEX_DYN_LINK           ON)
set(Boost_DATE_TIME_DYN_LINK       ON)
set(Boost_NO_BOOST_CMAKE           ON)

find_package(Boost COMPONENTS regex program_options thread serialization chrono system date_time QUIET)
 
# pour limiter le mode verbose
set(BOOST_FIND_QUIETLY ON)

find_package_handle_standard_args(BOOST
	DEFAULT_MSG 
	Boost_FOUND 
	Boost_INCLUDE_DIR 
	Boost_PROGRAM_OPTIONS_LIBRARY
	Boost_THREAD_LIBRARY
	Boost_SERIALIZATION_LIBRARY
	Boost_CHRONO_LIBRARY
  Boost_REGEX_LIBRARY
	Boost_SYSTEM_LIBRARY
  Boost_DATE_TIME_LIBRARY)

if(BOOST_FOUND AND NOT TARGET boost)
  
  set(BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
  
  set(BOOST_LIBRARIES ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_SERIALIZATION_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY})
  
  # boost program_options
  
  add_library(boost_program_options UNKNOWN IMPORTED)
  
  set_target_properties(boost_program_options PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIRS}") 
  
  set_property(TARGET boost_program_options APPEND PROPERTY
    IMPORTED_CONFIGURATIONS RELEASE)
  
  set_target_properties(boost_program_options PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}")
  
  set_property(TARGET boost_program_options APPEND PROPERTY
    IMPORTED_CONFIGURATIONS DEBUG)
  
  set_target_properties(boost_program_options PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG}")
  
  set_target_properties(boost_program_options PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
	
  set_property(TARGET boost_program_options APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS "BOOST_PROGRAM_OPTIONS_DYN_LINK")
  
  # boost chrono
  
  add_library(boost_chrono UNKNOWN IMPORTED)
  
  set_target_properties(boost_chrono PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIRS}") 
  
  set_property(TARGET boost_chrono APPEND PROPERTY
    IMPORTED_CONFIGURATIONS RELEASE)
  
  set_target_properties(boost_chrono PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${Boost_CHRONO_LIBRARY_RELEASE}")
  
  set_property(TARGET boost_chrono APPEND PROPERTY
    IMPORTED_CONFIGURATIONS DEBUG)
  
  set_target_properties(boost_chrono PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${Boost_CHRONO_LIBRARY_DEBUG}")
  
  set_target_properties(boost_chrono PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
	
  set_property(TARGET boost_chrono APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS "BOOST_CHRONO_DYN_LINK")
  
  # boost thread
  
  add_library(boost_thread UNKNOWN IMPORTED)
  
  set_target_properties(boost_thread PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIRS}") 
  
  set_property(TARGET boost_thread APPEND PROPERTY
    IMPORTED_CONFIGURATIONS RELEASE)
  
  set_target_properties(boost_thread PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${Boost_THREAD_LIBRARY_RELEASE}")
  
  set_property(TARGET boost_thread APPEND PROPERTY
    IMPORTED_CONFIGURATIONS DEBUG)
  
  set_target_properties(boost_thread PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${Boost_THREAD_LIBRARY_DEBUG}")
  
  set_target_properties(boost_thread PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
	
  set_property(TARGET boost_thread APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS "BOOST_THREAD_DYN_LINK")
  
  # boost regex
  
  add_library(boost_regex UNKNOWN IMPORTED)
  
  set_target_properties(boost_regex PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIRS}") 
  
  set_property(TARGET boost_regex APPEND PROPERTY
    IMPORTED_CONFIGURATIONS RELEASE)
  
  set_target_properties(boost_regex PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${Boost_REGEX_LIBRARY_RELEASE}")
  
  set_property(TARGET boost_regex APPEND PROPERTY
    IMPORTED_CONFIGURATIONS DEBUG)
  
  set_target_properties(boost_regex PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${Boost_REGEX_LIBRARY_DEBUG}")
  
  set_target_properties(boost_regex PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
  
  set_property(TARGET boost_regex APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS "BOOST_REGEX_DYN_LINK")
  
  # boost system
  
  add_library(boost_system UNKNOWN IMPORTED)
  
  set_target_properties(boost_system PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIRS}") 
  
  set_property(TARGET boost_system APPEND PROPERTY
    IMPORTED_CONFIGURATIONS RELEASE)
  
  set_target_properties(boost_system PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${Boost_SYSTEM_LIBRARY_RELEASE}")
  
  set_property(TARGET boost_system APPEND PROPERTY
    IMPORTED_CONFIGURATIONS DEBUG)
  
  set_target_properties(boost_system PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${Boost_SYSTEM_LIBRARY_DEBUG}")
  
  set_target_properties(boost_system PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
	
  set_property(TARGET boost_system APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS "BOOST_SYSTEM_DYN_LINK")
  
  # boost serialization
  
  add_library(boost_serialization UNKNOWN IMPORTED)
  
  set_target_properties(boost_serialization PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIRS}") 
  
  set_property(TARGET boost_serialization APPEND PROPERTY
    IMPORTED_CONFIGURATIONS RELEASE)
  
  set_target_properties(boost_serialization PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${Boost_SERIALIZATION_LIBRARY_RELEASE}")
  
  set_property(TARGET boost_serialization APPEND PROPERTY
    IMPORTED_CONFIGURATIONS DEBUG)
  
  set_target_properties(boost_serialization PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${Boost_SERIALIZATION_LIBRARY_DEBUG}")
  
  set_target_properties(boost_serialization PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
	
  set_property(TARGET boost_serialization APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS "BOOST_SERIALIZATION_DYN_LINK")
  
  # boost date_time
  
  add_library(boost_date_time UNKNOWN IMPORTED)
  
  set_target_properties(boost_date_time PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_INCLUDE_DIRS}") 
  
  set_property(TARGET boost_date_time APPEND PROPERTY
    IMPORTED_CONFIGURATIONS RELEASE)
  
  set_target_properties(boost_date_time PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
    IMPORTED_LOCATION_RELEASE "${Boost_DATE_TIME_LIBRARY_RELEASE}")
  
  set_property(TARGET boost_date_time APPEND PROPERTY
    IMPORTED_CONFIGURATIONS DEBUG)
  
  set_target_properties(boost_date_time PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
    IMPORTED_LOCATION_DEBUG "${Boost_DATE_TIME_LIBRARY_DEBUG}")
  
  set_target_properties(boost_date_time PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
  
  set_property(TARGET boost_date_time APPEND PROPERTY
    INTERFACE_COMPILE_DEFINITIONS "BOOST_DATE_TIME_DYN_LINK")
  
  # boost
  
  add_library(boost INTERFACE IMPORTED)
  
  set_property(TARGET boost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost_program_options")

  set_property(TARGET boost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost_chrono")

  set_property(TARGET boost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost_thread")

  set_property(TARGET boost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost_system")

  set_property(TARGET boost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost_serialization")
  
  set_property(TARGET boost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost_regex")
 
  set_property(TARGET boost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost_date_time")
  
  add_library(extraboost INTERFACE IMPORTED)
  
  # pour existant

  set_property(TARGET extraboost APPEND PROPERTY 
    INTERFACE_LINK_LIBRARIES "boost")

endif()
