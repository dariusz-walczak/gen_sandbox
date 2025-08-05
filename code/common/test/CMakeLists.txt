set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

# ===[ Library Target ]========================================================================= #

add_library(
  gen_test_lib
  src/application.cpp
  src/assertions.cpp
)

target_include_directories(
  gen_test_lib
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_test_lib PUBLIC gtest_main)
target_link_libraries(gen_test_lib PUBLIC spdlog::spdlog)
target_link_libraries(gen_test_lib PUBLIC CLI11::CLI11)
target_link_libraries(gen_test_lib PUBLIC gen_common)

set_target_properties(gen_test_lib PROPERTIES OUTPUT_NAME "gen_test")

# ===[ Application Target ]====================================================================== #

add_executable(
  gen_common_test
  src/main.cpp
  src/redland_utils.cpp
  src/resource.cpp
)

target_include_directories(
  gen_common_test
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_common_test PRIVATE gen_test_lib)
