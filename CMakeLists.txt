# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

cmake_minimum_required(VERSION 3.1)
project(pybind_cereal_example)

set(REFLECT_LIB_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING
    "root directory of reflect lib sources")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
enable_testing()

set (CMAKE_CXX_STANDARD 14)
ADD_DEFINITIONS("-Wall")
set(PYBIND11_PYTHON_VERSION 3)

add_subdirectory(pybind11)

add_subdirectory(reflect-lib)
add_subdirectory(aux)
add_subdirectory(test)
