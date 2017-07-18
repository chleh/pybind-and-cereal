
add_executable(test_serialization test_serialization.cpp)

add_test(NAME test_serialization
    COMMAND $<TARGET_FILE:test_serialization>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
