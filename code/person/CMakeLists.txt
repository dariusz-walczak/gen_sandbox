add_executable(
  gen_person
  src/command/common.cpp
  src/command/deps.cpp
  src/command/details.cpp
  src/command/list.cpp
  src/error.cpp
  src/main.cpp
  src/option_parser.cpp
  src/queries.cpp
)

target_include_directories(
  gen_person
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(gen_person PRIVATE gen_common)
target_link_libraries(gen_person PRIVATE CLI11::CLI11)
target_link_libraries(gen_person PRIVATE nlohmann_json::nlohmann_json)
