cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(
  gen_sandbox VERSION 0.1.0
  DESCRIPTION "Sample C++ application for experiments with RDF"
  LANGUAGES CXX)

set(
  CMAKE_CXX_STANDARD 17)

# ===[ Dependencies ]=========================================================================== #

# ---[ Raptor RDF Syntax Library ]-------------------------------------------------------------- #

# Required by the Redland RDF Library
#
# https://github.com/dajobe/raptor

find_path(RAPTOR2_INCLUDE_PATH raptor2.h PATH_SUFFIXES "raptor2" REQUIRED)
find_library(RAPTOR2_LIBRARY raptor2 REQUIRED)
include_directories(${RAPTOR2_INCLUDE_PATH})

# ---[ Rasqal RDF Query Library ]--------------------------------------------------------------- #

# Required by the Redland RDF Library
#
# https://github.com/dajobe/rasqal

find_path(RASQAL_INCLUDE_PATH rasqal.h PATH_SUFFIXES "rasqal" REQUIRED)
find_library(RASQAL_LIBRARY rasqal REQUIRED)
include_directories(${RASQAL_INCLUDE_PATH})

# ---[ Redland RDF Library ]-------------------------------------------------------------------- #

# Required by the project for rdf data handling purposes.
#
# https://github.com/dajobe/librdf

find_library(REDLAND_LIBRARY rdf REQUIRED)

# ---[ SpdLog Library ]------------------------------------------------------------------------- #

# SpdLog is a Fast C++ Logging Library
# Required by the project for logging purposes.
#
# https://github.com/gabime/spdlog

find_package(spdlog REQUIRED)

# ---[ Tabulate Library ]----------------------------------------------------------------------- #

# Required by the project for output data presentation purposes.
#
# https://github.com/p-ranav/tabulate

set(USE_CPP17 ON)

FetchContent_Declare(
    tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
    GIT_TAG master
)
FetchContent_MakeAvailable(tabulate)

include_directories(${tabulate_SOURCE_DIR}/single_include)

# ---[ CLI11 Library ]-------------------------------------------------------------------------- #

# CLI11 is a command line parser for C++11 and beyond that provides a rich feature set with a
# simple and intuitive interface.
#
# https://github.com/CLIUtils/CLI11

FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.2
)
FetchContent_MakeAvailable(cli11)

include_directories(${cli11_SOURCE_DIR}/include)

# ---[ NLohmann JSON ]-------------------------------------------------------------------------- #

# JSON serializer/deserializer
#
# https://github.com/nlohmann/json

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

include_directories(${cli11_SOURCE_DIR}/include)


# ===[ Components ]============================================================================= #

add_subdirectory(code/common)
add_subdirectory(code/person)
add_subdirectory(code/sandbox)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
