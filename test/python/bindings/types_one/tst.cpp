#include "pybind11/pybind11.h"

struct S
{
    int i;
};

int f(int i) { return i; }

int g(std::unique_ptr<S>const& s)
{
    return s->i;
}

std::unique_ptr<S> h() { return std::make_unique<S>(); }

int i(S s) { return s.i; };


struct P {
    P() = default;
    P(std::nullptr_t) {}
    explicit P(S const& s_) : s(new S(s_)) {}

    std::unique_ptr<S> s;
};

int from_ptr(P const& p) {
    if (p.s)
        return p.s->i;
    return -1;
}


PYBIND11_MODULE(tst, m)
{
    pybind11::class_<S>(m, "S")
        .def(pybind11::init<>())
        .def_readwrite("i", &S::i);

    m.def("f", &f);
    // m.def("g", &g); // does not work
    m.def("h", &h);
    m.def("i", &i);

    pybind11::class_<P>(m, "P")
        .def(pybind11::init<S const&>())
        .def(pybind11::init<>())
        .def(pybind11::init<std::nullptr_t>());
    pybind11::implicitly_convertible<std::nullptr_t, P>();

    // m.def("from_ptr", &from_ptr, pybind11::arg().none(true));
    m.def("from_ptr", [](pybind11::object const& o) {
        if (o.is_none()) return from_ptr(P());
        P const* p = o.cast<P const*>();
        return from_ptr(*p);
    });
}
