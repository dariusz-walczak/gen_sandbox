set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

add_executable(
  gen_common_test
  src/resource.cpp
)

target_link_libraries(gen_common_test PRIVATE gtest_main)
target_link_libraries(gen_common_test PRIVATE gen_common)
