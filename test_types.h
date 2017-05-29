#pragma once

#include "reflect-lib/reflect-macros.h"

struct Base
{
    int i;
    double d;

    std::string say_hello() { return "hello!"; }

    virtual ~Base() = default;

    REFLECT(Base, FIELDS(i, d), METHODS(say_hello))
};

struct Derived1 : Base
{
    std::string s;
    Base b;

    Base& get_base() { return *this; }

    REFLECT_DERIVED(Derived1, Base, FIELDS(s, b), METHODS(get_base))
};

struct Derived2 : Base
{
    bool b;

    REFLECT_DERIVED(Derived2, Base, FIELDS(b), METHODS())
};

