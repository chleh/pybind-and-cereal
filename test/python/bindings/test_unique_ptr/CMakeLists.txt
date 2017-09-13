# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

reflect_lib_add_python_module(test_unique_ptr test_unique_ptr.cpp)
reflect_lib_target_link_libraries(test_unique_ptr PRIVATE reflection)
