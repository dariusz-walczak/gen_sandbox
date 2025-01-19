add_executable(
  gen_sandbox
  src/main.cpp
)

target_link_libraries(gen_sandbox PRIVATE gen_common)
