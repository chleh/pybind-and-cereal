# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

reflect_lib_add_python_module(types_one.types_one_b types_one_b.cpp)
reflect_lib_target_link_libraries(types_one.types_one_b PRIVATE reflection)
