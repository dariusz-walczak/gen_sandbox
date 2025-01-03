cmake_minimum_required(VERSION 3.20)

project(
  gen_sandbox VERSION 0.1.0
  DESCRIPTION "Sample C++ application for experiments with RDF"
  LANGUAGES CXX)

set(
  CMAKE_CXX_STANDARD 17)

# Determine the Raptor RDF Syntax library paths:
find_path(RAPTOR2_INCLUDE_PATH raptor2.h PATH_SUFFIXES "raptor2" REQUIRED)
find_library(RAPTOR2_LIBRARY raptor2 REQUIRED)

# Determine the Rasqal RDF Query library paths:
find_path(RASQAL_INCLUDE_PATH rasqal.h PATH_SUFFIXES "rasqal" REQUIRED)
find_library(RASQAL_LIBRARY rasqal REQUIRED)

# Determine the Redland RDF library path:
find_library(REDLAND_LIBRARY rdf REQUIRED)

find_package(spdlog REQUIRED)

# Include and link Raptor2
include_directories(${RAPTOR2_INCLUDE_PATH})
include_directories(${RASQAL_INCLUDE_PATH})

set(
  SOURCES
  src/main.cpp)

add_executable(
  ${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${RAPTOR2_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RASQAL_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${REDLAND_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
