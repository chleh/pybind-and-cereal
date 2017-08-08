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

PYBIND11_MODULE(tst, m)
{
    auto c = pybind11::class_<S>(m).def_readwrite("i", &S::i);

    m.def("f", &f);
    // m.def("g", &g); // does not work
    m.def("h", &h);
    m.def("i", &i);
}
