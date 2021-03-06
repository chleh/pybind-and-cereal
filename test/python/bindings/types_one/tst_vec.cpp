/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include <vector>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

struct S {
    std::vector<int> v;
    int i;
};

PYBIND11_MAKE_OPAQUE(std::vector<int>)

PYBIND11_MODULE(tst_vec, m)
{
    pybind11::class_<S>(m, "S")
        .def(pybind11::init<>())
        .def_property("v",
                      [](S& s) { return s.v; },
                      [](S& s, pybind11::object& v) {
                          try {
                              auto v_ = v.cast<std::vector<int>>();
                              s.v = std::move(v_);
                              return;
                          } catch (pybind11::cast_error) {
                          }
                          try {
                              std::cout << ">> pre cast\n";
                              auto it = v.cast<pybind11::iterable>();
                              std::cout << ">> post cast\n";
                              s.v.clear();
                              // s.v.reserve(it.size());
                              for (auto& e : it)
                                  s.v.emplace_back(e.cast<int>());
                              std::cout << ">> post copy\n";
                              return;
                          } catch (pybind11::cast_error) {
                          }
                          throw pybind11::type_error("wrong argument type");
                      })
        .def("__getstate__",
             [](S const& s) { return pybind11::make_tuple(&s.v, s.i); })
        .def("__setstate__", [](S& s, pybind11::tuple& t) {
            if (t.size() != 2)
                throw std::runtime_error("Invalid state!");
            new (&s) S();

            s.v = std::move(t[0]).cast<std::vector<int>>();
            s.i = t[1].cast<int>();
        });

    pybind11::bind_vector<std::vector<int>>(
        m, "vectorInt", pybind11::buffer_protocol())
        .def("__getstate__",
             [](std::vector<int> const& v) {
                 auto l = pybind11::list(v.size());
                 for (std::size_t i=0; i<v.size(); ++i)
                     l[i] = v[i];
                 return l;
             })
        .def("__setstate__", [](std::vector<int>& v, pybind11::list l) {
            new (&v) std::vector<int>();
            v.reserve(pybind11::len(l));
            for (auto& e : l)
                v.emplace_back(e.cast<int>());
        });
}
