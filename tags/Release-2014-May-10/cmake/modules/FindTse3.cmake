# - try to find Tse3
# Once done this will define
#
#  TSE3_FOUND - system has Cairo
#  TSE3_INCLUDE_DIRS - the Cairo include directories
#  TSE3_LIBRARIES - Link these to use Cairo
#
# Copyright (C) 2012, Gopalakrishna Palem, <Gopalakrishna.Palem@Yahoo.com>
#
# Redistribution and use is allowed according to the terms of CarMusTy license.
# Refer to http://CarMusTy.SourceForge.net
include(FindPackageHandleStandardArgs)

FIND_PATH(TSE3_INCLUDE_DIR tse3/TSE3.h     
	PATHS
    /usr/include)

SET(TSE3LIB_NAMES ${TSE3LIB_NAMES} tse3 libtse3)
FIND_LIBRARY(TSE3_LIBRARY NAMES ${TSE3LIB_NAMES} PATHS /usr/lib /usr/local/lib /opt/libtse3/lib)

# handle the QUIETLY and REQUIRED arguments and set TSE3_FOUND to TRUE if all listed variables are TRUE
find_package_handle_standard_args(TSE3  DEFAULT_MSG  TSE3_LIBRARY  TSE3_INCLUDE_DIR)

if(TSE3_FOUND)
  SET(TSE3_INCLUDE_DIRS ${TE3_INCLUDE_DIR})
  SET(TSE3_LIBRARIES ${TSE3_LIBRARY})
endif()

# MARK_AS_ADVANCED(TSE3_INCLUDE_DIR TSE3_LIBRARY)
