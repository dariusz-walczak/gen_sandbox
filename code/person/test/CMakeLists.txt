set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

add_executable(
  gen_person_test
  src/command/deps.cpp
  src/main.cpp
  src/queries/deps.cpp
)

target_include_directories(
  gen_person_test
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_person_test PRIVATE gen_person_lib)
target_link_libraries(gen_person_test PRIVATE gen_test_lib)

gtest_discover_tests(gen_person_test)
