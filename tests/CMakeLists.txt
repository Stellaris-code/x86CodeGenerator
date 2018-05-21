file(GLOB_RECURSE test_files "*.cpp" "*.hpp")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(tests ${test_files})
target_link_libraries(tests ${GTEST_BOTH_LIBRARIES} ${PROJECT_NAME})

add_test(unit_tests tests)
