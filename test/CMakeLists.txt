project(gdb_stub_test)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(googletest)

add_executable(gdb_stub_test GdbStubTest.cpp)

target_include_directories(gdb_stub_test PUBLIC ${googletest_SOURCE_DIR}/googletest/include)

target_link_libraries(gdb_stub_test gtest gtest_main gmock gmock_main)
target_link_libraries(gdb_stub_test gdb_stub)