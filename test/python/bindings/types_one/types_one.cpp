/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one.h"

namespace types_one
{
struct BaseTrampoline : public Base
{
    using Base::Base;

    std::string what() override {
        PYBIND11_OVERLOAD_PURE(
            std::string, /* Return type */
            Base,        /* Parent class */
            what         /* Name of function in C++ (must match Python name) */
            /* This function has no arguments */
            );
    }

    REFLECT_DERIVED((BaseTrampoline), (Base), FIELDS(), METHODS())
};

}  // namespace types_one

REFLECT_LIB_PYTHON_MODULE(types_one, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::Base, types_one::BaseTrampoline>();

    m.module.def("say_what", types_one::say_what);

    m.bind<types_one::VectorTest>();

    m.bind<types_one::NonDefaultConstructible>();
}
