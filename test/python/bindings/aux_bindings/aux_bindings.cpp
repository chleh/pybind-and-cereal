/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include "reflect-lib/pybind.h"

#include "test/types/aux_bindings/tuple.h"

REFLECT_LIB_PYTHON_MODULE(aux_bindings, module)
{
    using namespace aux_bindings;

    reflect_lib::Module m(module);

    m.bind<ContainsTuple>();
}
