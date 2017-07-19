
add_executable(test_save test_save.cpp)
add_executable(test_load test_load.cpp)

add_test(NAME test_save_load
    COMMAND /bin/sh "${CMAKE_CURRENT_SOURCE_DIR}/test_save_load.sh"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
