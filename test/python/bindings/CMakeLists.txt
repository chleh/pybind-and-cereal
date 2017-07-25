add_subdirectory(types_one)

# TODO What if two different projects have bindings generated with this
# library and are used in the same python code afterwards?
reflect_lib_add_aux_python_module(AUX aux.cpp)
reflect_lib_target_link_libraries(AUX PRIVATE reflection)
