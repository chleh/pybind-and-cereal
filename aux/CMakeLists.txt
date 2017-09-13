# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.


pybind11_add_module(py_mangle py_mangle.cpp)
target_link_libraries(py_mangle PRIVATE reflection)
