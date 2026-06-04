# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_executable(chip_8_tests Chip8Tests.cpp)

# Link GTest libraries and C++ threads
target_link_libraries(chip_8_tests
    PRIVATE
        GTest::gtest_main
        chip8
)

# Register the test with CMake so CTest can run it
include(GoogleTest)
gtest_discover_tests(chip_8_tests)