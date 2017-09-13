/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include <pybind11/pybind11.h>

#include "reflect-lib/remangle.h"

PYBIND11_MODULE(py_mangle, m) {
    using namespace reflect_lib;

    m.def("demangle2", demangle2);
}

