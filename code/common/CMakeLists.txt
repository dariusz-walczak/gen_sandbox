set(CMAKE_CXX_CLANG_TIDY clang-tidy;)

add_library(
  gen_common
  src/command_line_utils.cpp
  src/common_exception.cpp
  src/file_system_utils.cpp
  src/person.cpp
  src/redland_utils.cpp
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
target_link_libraries(gen_common PRIVATE nlohmann_json::nlohmann_json)
