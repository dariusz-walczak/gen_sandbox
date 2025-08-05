set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

# ===[ Library Target ]========================================================================= #

add_library(
  gen_person_lib
  src/command/common.cpp
  src/command/deps.cpp
  src/command/details.cpp
  src/command/list.cpp
  src/error.cpp
  src/option_parser.cpp
  src/queries/common.cpp
  src/queries/deps.cpp
)

target_include_directories(
  gen_person_lib
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_person_lib PUBLIC gen_common)

set_target_properties(gen_person_lib PROPERTIES OUTPUT_NAME "gen_person")

# ===[ Application Target ]====================================================================== #

add_executable(
  gen_person_app
  src/main.cpp
)

target_include_directories(
  gen_person_app
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_person_app PRIVATE gen_person_lib)
target_link_libraries(gen_person_app PRIVATE CLI11::CLI11)
target_link_libraries(gen_person_app PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(gen_person_app PROPERTIES OUTPUT_NAME "gen_person")

# ===[ Components ]============================================================================= #

add_subdirectory(test)
