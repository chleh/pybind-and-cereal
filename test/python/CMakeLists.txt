
pybind11_add_module(bindings bindings.cpp)
target_link_libraries(bindings PRIVATE reflection)

macro(add_py_test test_name)
    add_test(NAME "py_${test_name}"
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.py"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endmacro()

add_py_test(test_copyable)
add_py_test(test_Derived1)
add_py_test(test_moveonly)
add_py_test(test_unique_ptr)
add_py_test(test_vector)
add_py_test(test_smart_ptr)

