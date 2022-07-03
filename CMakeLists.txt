CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(trackmon
        VERSION 0.1.0
        LANGUAGES CXX )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# ===== Enable option flags: =====
SET(ENABLE_AIS 1)
SET(ENABLE_MOOS 0)


# ===== Find Dependencies =====

find_package(AIS REQUIRED)

find_package(Curses REQUIRED)

find_package(cxxopts REQUIRED)

find_package(fmt REQUIRED)

find_package(PROJ CONFIG REQUIRED)

find_package(PCAP REQUIRED)

find_package(spdlog REQUIRED)


# Linux Libraries
SET(SYSTEM_LIBS
    m
    spdlog::spdlog
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

if( ${ENABLE_AIS})
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -DENABLE_AIS=1")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " -DENABLE_AIS=1")
endif()

if( ${ENABLE_MOOS})
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -DENABLE_MOOS=1")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " -DENABLE_MOOS=1")
endif()

# ====== Add source directory ======
ADD_SUBDIRECTORY( src )
