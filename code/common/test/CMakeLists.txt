set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

# ===[ Dependencies ]=========================================================================== #

# Dependency:
#     boost::system library
# Rationale:
#     The boost::system library is needed to use the boost::dll library which in turn provides the
#     boost::dll::program_location() function.
#     See: https://www.boost.org/doc/libs/1_83_0/doc/html/boost_dll/getting_started.html
# Scope:
#     the gen_test_lib library
find_package(Boost REQUIRED system)

# ===[ Library Target ]========================================================================= #

add_library(
  gen_test_lib
  src/tools/application.cpp
  src/tools/assertions.cpp
  src/tools/error.cpp
  src/tools/note.cpp
  src/tools/person.cpp
  src/tools/redland.cpp
  src/tools/resource.cpp
  src/tools/string.cpp
  src/tools/variable.cpp
)

target_include_directories(
  gen_test_lib
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_test_lib PUBLIC gtest_main)
target_link_libraries(gen_test_lib PUBLIC GTest::gmock)
target_link_libraries(gen_test_lib PUBLIC spdlog::spdlog)
target_link_libraries(gen_test_lib PUBLIC CLI11::CLI11)
target_link_libraries(gen_test_lib PUBLIC gen_common)
target_link_libraries(gen_test_lib PRIVATE Boost::system)

set_target_properties(gen_test_lib PROPERTIES OUTPUT_NAME "gen_test")

# This boost::dll library configuration definition makes the library to rely on the std::filesystem
#  rather than than boost::filesystem library and eliminates the need to link against the later.
target_compile_definitions(gen_test_lib PRIVATE BOOST_DLL_USE_STD_FS=1)

# ===[ Application Target ]====================================================================== #

add_executable(
  gen_common_test
  src/contract.cpp
  src/main.cpp
  src/note.cpp
  src/person.cpp
  src/redland_utils.cpp
  src/resource.cpp
  src/resource_utils.cpp
  src/string.cpp
  src/variable.cpp
)

target_include_directories(
  gen_common_test
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_common_test PRIVATE gen_test_lib)

gtest_discover_tests(gen_common_test)
