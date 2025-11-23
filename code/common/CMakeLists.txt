set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

# ===[ Dependencies ]=========================================================================== #

# Dependency:
#     boost::url library
# Rationale:
#     needed for resource URIs representation and transformation
# Scope:
#     the gen_common library and targets linking it
find_package(Boost REQUIRED url)

# ===[ Target ]================================================================================= #

add_library(
  gen_common
  src/command_line_utils.cpp
  src/common_exception.cpp
  src/contract.cpp
  src/file_system_utils.cpp
  src/note.cpp
  src/person.cpp
  src/redland_utils.cpp
  src/resource.cpp
  src/spdlog_utils.cpp
  src/string.cpp
  src/variable.cpp
)

target_include_directories(
  gen_common
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_common PRIVATE ${RAPTOR2_LIBRARY})
target_link_libraries(gen_common PRIVATE ${RASQAL_LIBRARY})
target_link_libraries(gen_common PRIVATE ${REDLAND_LIBRARY})
target_link_libraries(gen_common PRIVATE spdlog::spdlog)
target_link_libraries(gen_common PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(gen_common PUBLIC Boost::url)

# ===[ Components ]============================================================================= #

add_subdirectory(test)
