#  AIS_FOUND - system has libais installed
#  AIS_INCLUDE_DIRS - include path to ais.hpp directories
#  AIS_LIBRARIES - link these to use Magick++

# # Use pkg-config to get hints about paths
# include(LibFindMacros)
# libfind_pkg_check_modules(LIBAIS_PKGCONF ImageMagick++)

# Include dir
find_path(AIS_INCLUDE_DIR
    NAMES ais.h
)

# Finally the library itself
find_library(AIS_LIBRARY
  NAMES libais.a
)

#message(STATUS "::AIS: ${AIS_INCLUDE_DIR}")
#message(STATUS "::AIS: ${AIS_LIBRARY}")

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(AIS_INCLUDES AIS_INCLUDE_DIR)
set(AIS_LIBRARIES AIS_LIBRARY)
set(AIS_FOUND 1)

# libfind_process(AIS)
