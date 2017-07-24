reflect_lib_add_python_module(types_one.types_one_a.types_one_a_a
    types_one_a_a.cpp part1.cpp part2.cpp)
reflect_lib_target_link_libraries(types_one.types_one_a.types_one_a_a
    PRIVATE reflection)
