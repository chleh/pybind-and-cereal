# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

file(GLOB sources LIST_DIRECTORIES false *.cpp *.h)
reflect_lib_add_python_module(types_one.types_one_a.types_one_a_a
    ${sources})
reflect_lib_target_link_libraries(types_one.types_one_a.types_one_a_a
    PRIVATE reflection)
