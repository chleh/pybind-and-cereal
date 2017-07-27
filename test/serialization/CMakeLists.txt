
add_executable(test_save test_save.cpp)
add_executable(test_load test_load.cpp)

# maybe move that to a separate category
add_executable(test_iterate_fields test_iterate_fields.cpp)
target_link_libraries(test_iterate_fields reflection)

add_test(NAME test_save_load
    COMMAND /bin/sh "${CMAKE_CURRENT_SOURCE_DIR}/test_save_load.sh"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME test_iterate_fields
    COMMAND ./test_iterate_fields
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
