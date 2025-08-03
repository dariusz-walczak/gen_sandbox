set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

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

target_link_libraries(gen_common_test PRIVATE gtest_main)
target_link_libraries(gen_common_test PRIVATE gen_common)
