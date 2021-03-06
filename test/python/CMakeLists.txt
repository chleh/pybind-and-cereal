# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

include("${REFLECT_LIB_SOURCE_ROOT}/cmake/python_bindings.cmake")

add_subdirectory(bindings)

macro(add_py_test test_name)
    add_test(NAME "py_${test_name}"
        # TODO search for shell, maybe append to PYTHONPATH from environment
        COMMAND /bin/sh -c "export PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/bindings;
            ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bindings")
endmacro()


# From CMake documentation:
# We do not recommend using GLOB to collect a list of source files from your
# source tree. If no CMakeLists.txt file changes when a source is added or
# removed then the generated build system cannot know when to ask CMake to
# regenerate.
file(GLOB py_tests LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "test_*.py")
# message(STATUS "py tests: ${py_tests}")
# install (FILES ${py_tests} DESTINATION "share/reflect_lib/test/python")
foreach(test_name IN LISTS py_tests)
    add_py_test("${test_name}")
endforeach()
