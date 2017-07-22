include("${REFLECT_LIB_SOURCE_ROOT}/cmake/python_bindings.cmake")

add_subdirectory(bindings)

macro(add_py_test test_name)
    add_test(NAME "py_${test_name}"
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bindings")
endmacro()

add_py_test(test_copyable)
add_py_test(test_Derived1)
add_py_test(test_moveonly)
add_py_test(test_unique_ptr)
add_py_test(test_vector)
add_py_test(test_smart_ptr)

# TODO auto-add tests by file glob
add_py_test(test_misc)
