/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_c/types_one_c.h"

namespace types_one
{
namespace types_one_c
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

    // Note: no need to reflect the trampoline.
};

std::string say_what(Base& b)
{
    return b.what();
}

}  // namespace types_one_c
}  // namespace types_one


REFLECT_LIB_PYTHON_MODULE(types_one__types_one_c, module)
{
    reflect_lib::Module m(module);

    m.bind<types_one::types_one_c::Base,
           types_one::types_one_c::BaseTrampoline>().
            def(pybind11::init<std::string>());

    m.module.def("say_what", types_one::types_one_c::say_what);
}
