set(CMAKE_CXX_CLANG_TIDY clang-tidy;-checks=modernize-*,performance-*,bugprone-*;)

add_executable(
  gen_sandbox
  src/main.cpp
)

target_link_libraries(gen_sandbox PRIVATE gen_common)
