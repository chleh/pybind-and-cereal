add_subdirectory(types_one_a)
add_subdirectory(types_one_b)
add_subdirectory(types_one_c)

reflect_lib_add_python_module(types_one types_one.cpp)
reflect_lib_target_link_libraries(types_one PRIVATE reflection)

pybind11_add_module(tst tst.cpp)
pybind11_add_module(tst_vec tst_vec.cpp)
