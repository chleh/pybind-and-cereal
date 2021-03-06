/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_b/types_one_b.h"

REFLECT_LIB_PYTHON_MODULE(types_one__types_one_b, module)
{
    reflect_lib::Module m(module);

	m.bind<types_one::types_one_b::Derived3<int, int>>();
    m.bind<types_one::types_one_b::Derived3<int, double>>();

    // import necessary because of cross-module inheritance
    pybind11::module::import("types_one.types_one_a");
    m.bind<types_one::types_one_b::NoCopyDerived>();
}
