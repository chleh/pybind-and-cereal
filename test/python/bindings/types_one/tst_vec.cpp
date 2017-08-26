#include <vector>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

struct S {
    std::vector<int> v;
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
             [](S const& s) { return pybind11::make_tuple(&s.v); })
        .def("__setstate__", [](S& s, pybind11::tuple& t) {
            if (t.size() != 1)
                throw std::runtime_error("Invalid state!");
            new (&s) S();

            s.v = std::move(t[0]).cast<std::vector<int>>();
        });

    pybind11::bind_vector<std::vector<int>>(
        m, "vectorInt", pybind11::buffer_protocol())
        .def("__getstate__",
             [](std::vector<int> const& v) {
                 auto l = pybind11::list();
                 for (int i : v)
                     l.append(i);
                 return l;
             })
        .def("__setstate__", [](std::vector<int>& v, pybind11::list l) {
            new (&v) std::vector<int>();
            for (auto& e : l)
                v.emplace_back(e.cast<int>());
        });
}
