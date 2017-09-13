/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

struct S
{
    int i;

    ~S() { std::cout << "deleting S\n"; }
};

int main()
{
    namespace py = pybind11;

    py::scoped_interpreter guard{};
    (void) guard;

    py::module m;
    py::class_<S>(m, "S").def_readwrite("i", &S::i);

    // auto globals = m;

    S s;
    s.i = 42;

    pybind11::dict d;
    d["s"] = &s;

    // m.add_object("d", d);
    auto globals = py::globals();
    // globals["m"] = m;
    globals["d"] = d;

    // m.def("f", [&s]() -> S* { return &s; }, py::return_value_policy::reference);

    py::exec(R"(
        print("Hello World!")
        print(d)
        print("py: ", d["s"].i)
        d["s"].i = 21
    )", globals);

    std::cout << "c++: " << s.i << '\n';

    py::exec(R"(
        print("py: ", d["s"].i)
        d["s"].i = 10
    )", globals);

    std::cout << "c++: " << s.i << '\n';
}
