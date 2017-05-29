#include <iostream>
#include <fstream>
#include <cassert>
#include <typeinfo>

#include "reflect-macros.h"

#include "cereal.h"
#include "pybind.h"

struct Base
{
    int i;
    double d;

    void say_hello() {}

    virtual ~Base() = default;

    REFLECT(Base, FIELDS(i, d), METHODS(say_hello))
};


struct Derived1 : Base
{
    std::string s;
    Base b;

    REFLECT_DERIVED(Derived1, Base, FIELDS(s, b), METHODS())
};
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(Derived1, Base)

struct Derived2 : Base
{
    bool b;

    REFLECT_DERIVED(Derived2, Base, FIELDS(b), METHODS())
};
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(Derived2, Base)



PYBIND11_MODULE(test_pybind, m) {
    wrap_into_pybind<Base>(m);

    // return m.ptr();
}


