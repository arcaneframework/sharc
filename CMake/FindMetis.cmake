#
# Find the Parmetis includes and library
#
# This module defines
# PARMETIS_INCLUDE_DIR, where to find headers,
# PARMETIS_LIBRARIES, the libraries to link against to use Parmetis.
# PARMETIS_FOUND, If false, do not try to use Parmetis.
 
FIND_PATH(METIS_INCLUDE_DIR metis.h
  ${METIS_INCLUDE_PATH}
)
 
FIND_LIBRARY(METIS_LIBRARY metis
  ${METIS_LIBRARY_PATH}
)
 
SET( METIS_FOUND "NO" )
IF(METIS_INCLUDE_DIR)
  IF(METIS_LIBRARY)
    SET( METIS_FOUND "YES" )
    SET( METIS_LIBRARIES ${METIS_LIBRARY})
  ENDIF(METIS_LIBRARY)
ENDIF(METIS_INCLUDE_DIR)

list(APPEND REQUIRED_DEPENDENCIES METIS)
