# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

add_executable(test_save test_save.cpp)
add_executable(test_load test_load.cpp)

add_test(NAME test_save_load
    COMMAND /bin/sh "${CMAKE_CURRENT_SOURCE_DIR}/test_save_load.sh"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
