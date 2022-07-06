# Find the leveldb includes and library.
# If you need to add a custom library search path, do it via CMAKE_PREFIX_PATH.
# This module defines:
#   LEVELDB_INCLUDE_DIRS, where to find header, etc.
#   LEVELDB_LIBRARIES, the libraries needed to use leveldb.
#   LEVELDB_FOUND, If false, do not try to use leveldb.

include(FindPackageHandleStandardArgs)

# Only look in default directories
find_path(LEVELDB_INCLUDE_DIR NAMES leveldb/db.h DOC "leveldb include dir")

if(LEVELDB_USE_STATIC_LIBS)
  set(names ${CMAKE_STATIC_LIBRARY_PREFIX}leveldb${CMAKE_STATIC_LIBRARY_SUFFIX})
else()
  set(names leveldb)
endif()

find_library(LEVELDB_LIBRARY NAMES ${names}	DOC "leveldb library")

set(LEVELDB_INCLUDE_DIRS ${LEVELDB_INCLUDE_DIR})
set(LEVELDB_LIBRARIES ${LEVELDB_LIBRARY})

# When linking statically we should also include the snappy static lib.
if(LEVELDB_LIBRARY MATCHES ${CMAKE_STATIC_LIBRARY_SUFFIX})
  find_path(SNAPPY_INCLUDE_DIR snappy.h PATH_SUFFIXES snappy)
  find_library(SNAPPY_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}snappy${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(LEVELDB_INCLUDE_DIRS ${LEVELDB_INCLUDE_DIR} ${SNAPPY_INCLUDE_DIR})
  set(LEVELDB_LIBRARIES ${LEVELDB_LIBRARY} ${SNAPPY_LIBRARY})
endif()

# Debug library on Windows.
# Same naming convention as in Qt (appending debug library with d).
# Boost is using the same "hack" as us with "optimized" and "debug".
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  find_library(LEVELDB_LIBRARY_DEBUG NAMES leveldbd DOC "leveldb debug library")
  list(APPEND LEVELDB_LIBRARIES "shlwapi")
  list(APPEND LEVELDB_LIBRARY_DEBUG "shlwapi")
  set(LEVELDB_LIBRARIES optimized ${LEVELDB_LIBRARIES} debug ${LEVELDB_LIBRARY_DEBUG})
endif()

# Handle the QUIETLY and REQUIRED arguments and set LEVELDB_FOUND to TRUE.
# If all listed variables are TRUE, hide their existence from configuration view.
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  LevelDB DEFAULT_MSG
  LEVELDB_LIBRARY LEVELDB_INCLUDE_DIR
)

mark_as_advanced (LEVELDB_INCLUDE_DIR LEVELDB_LIBRARY)

