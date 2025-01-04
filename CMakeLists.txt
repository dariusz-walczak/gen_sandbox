cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(
  gen_sandbox VERSION 0.1.0
  DESCRIPTION "Sample C++ application for experiments with RDF"
  LANGUAGES CXX)

set(
  CMAKE_CXX_STANDARD 17)

# ---[ Raptor RDF Syntax Library ]-------------------------------------------------------------- #

# Required by the Redland RDF Library

find_path(RAPTOR2_INCLUDE_PATH raptor2.h PATH_SUFFIXES "raptor2" REQUIRED)
find_library(RAPTOR2_LIBRARY raptor2 REQUIRED)
include_directories(${RAPTOR2_INCLUDE_PATH})

# ---[ Rasqal RDF Query Library ]--------------------------------------------------------------- #

# Required by the Redland RDF Library

find_path(RASQAL_INCLUDE_PATH rasqal.h PATH_SUFFIXES "rasqal" REQUIRED)
find_library(RASQAL_LIBRARY rasqal REQUIRED)
include_directories(${RASQAL_INCLUDE_PATH})

# ---[ Redland RDF Library ]-------------------------------------------------------------------- #

# Required by the project for rdf data handling purposes.

find_library(REDLAND_LIBRARY rdf REQUIRED)

# ---[ SpdLog Library ]------------------------------------------------------------------------- #

# SpdLog is a Fast C++ Logging Library
# Required by the project for logging purposes.

find_package(spdlog REQUIRED)

# ---[ Tabulate Library ]----------------------------------------------------------------------- #

# Required by the project for output data presentation purposes.

set(USE_CPP17 ON)

FetchContent_Declare(
    tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
    GIT_TAG master
)
FetchContent_MakeAvailable(tabulate)

include_directories(${tabulate_SOURCE_DIR}/single_include)



set(
  SOURCES
  src/main.cpp
  src/redland_utils.cpp
)

add_executable(
  ${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${RAPTOR2_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RASQAL_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${REDLAND_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
