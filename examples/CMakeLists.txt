file(GLOB_RECURSE test_files "*.cpp" "*.hpp")

add_executable(brainfuck ${test_files})
target_link_libraries(brainfuck ${PROJECT_NAME})
