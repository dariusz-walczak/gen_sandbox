add_executable(
  gen_person
  src/main.cpp
)

target_link_libraries(gen_person PRIVATE gen_common)
target_link_libraries(gen_person PRIVATE CLI11::CLI11)
