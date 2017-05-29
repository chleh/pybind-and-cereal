#pragma once

#include "reflect-lib/reflect-macros.h"

struct Base
{
    int i;
    double d;

    std::string say_hello() { return "hello!"; }
    virtual std::string what() = 0;

    virtual ~Base() = default;

    REFLECT(Base, FIELDS(i, d), METHODS(say_hello, what))
};

struct Derived1 : Base
{
    std::string s;
    std::unique_ptr<Base> b;
    // bool b;

    Base& get_base() { return *this; }
    std::string what() override { return "der1"; };

    REFLECT_DERIVED(Derived1, Base, FIELDS(s, b), METHODS(get_base))
};

struct Derived2 : Base
{
    bool b;

    REFLECT_DERIVED(Derived2, Base, FIELDS(b), METHODS())
};

