CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(trackmon
        VERSION 0.1.0
        LANGUAGES CXX )

# ===== Find MOOS =====
# If we can't find MOOS, just don't configure it, and disable the module
find_package(MOOS 10.0 QUIET)
if( MOOS_INCLUDE_DIRS )  # probably wrong variable name
    INCLUDE_DIRECTORIES( SYSTEM ${MOOS_INCLUDE_DIRS})

    SET(ENABLE_MOOS 1)
    # ====== Find the "moos-ivp" base directory ======
    # Search for the moos-ivp folder
    find_path( MOOSIVP_SOURCE_TREE_BASE
              NAMES build-ivp.sh build-moos.sh configure-ivp.sh
              PATHS "~/moos-ivp" "~/moos-ivp/trunk/"
              DOC "Base directory of the MOOS-IvP source tree"
              NO_DEFAULT_PATH
    )

    if (NOT MOOSIVP_SOURCE_TREE_BASE)
        message("Please set MOOSIVP_SOURCE_TREE_BASE to  ")
        message("the location of the \"moos-ivp\" folder ")
        return()
    endif()

  # ====== Specify where to find IvP's headers and libraries... ======
  FILE(GLOB IVP_INCLUDE_DIRS ${MOOSIVP_SOURCE_TREE_BASE}/ivp/src/lib_* )
  INCLUDE_DIRECTORIES(SYSTEM ${IVP_INCLUDE_DIRS})

  FILE(GLOB IVP_LIBRARY_DIRS ${MOOSIVP_SOURCE_TREE_BASE}/lib )
  LINK_DIRECTORIES(${IVP_LIBRARY_DIRS})

endif()

find_package(PROJ CONFIG REQUIRED)

find_package(Curses REQUIRED)
# target_link_libraries(MyApp PRIVATE PROJ::proj)

set(DEPEND_LIBS PROJ::proj)

# Linux Libraries
SET(SYSTEM_LIBS
    m
    pthread )
LINK_DIRECTORIES(/usr/local/lib)

set(CMAKE_MAKE_PROGRAM ninja)

# ====== Specify Compiler Flags ======
# add options
add_compile_options("-fPIC")

# set language
set(CMAKE_CXX_STANDARD 20)
# add_compile_options("-std=c++20") # shouldn't need this...

# set warnings
add_compile_options(-Wall -Wno-pedantic)

string(APPEND CMAKE_CXX_FLAGS_DEBUG " -DDEBUG=1")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -DNDEBUG=1")

if( ${ENABLE_MOOS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_MOOS=1")
endif()

# ====== Add source directory ======
ADD_SUBDIRECTORY( src )
