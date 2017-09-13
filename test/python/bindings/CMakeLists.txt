# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

add_subdirectory(aux_bindings)
add_subdirectory(pickle_types)
add_subdirectory(test_unique_ptr)
add_subdirectory(types_one)

# TODO What if two different projects have bindings generated with this
# library and are used in the same python code afterwards?
reflect_lib_add_aux_python_module(AUX aux.cpp)
reflect_lib_target_link_libraries(AUX PRIVATE reflection)
