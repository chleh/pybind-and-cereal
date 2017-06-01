#pragma once

#include "reflect-lib/reflect-macros.h"

#include <iostream>

struct NoCopy
{
    NoCopy() = default;
    NoCopy(NoCopy&&) = default;
    NoCopy(NoCopy const&) = delete;
    NoCopy& operator=(NoCopy&&) = default;
    NoCopy& operator=(NoCopy const&) = delete;

    int i = 10;

    REFLECT(NoCopy, FIELDS(i), METHODS())
};

struct Base
{
    int i;
    double d;
    std::vector<float> v;

    std::string say_hello() { return "hello!"; }
    virtual std::string what() = 0;

    virtual ~Base() = default;

    REFLECT(Base, FIELDS(i, d, v), METHODS(say_hello, what))
};

struct Derived1 : Base
{
    Derived1() = default;

    std::string s;
    std::unique_ptr<Base> b;
    NoCopy nc;

    Base& get_base() { return *this; }
    std::string what() override { return "der1"; };

    // Destructor prevents implicit move ctor
    // Derived1(Derived1&&) = default;
    // ~Derived1() { std::cout << "~Derived1() s=" << s << '\n'; }

    REFLECT_DERIVED(Derived1, Base, FIELDS(s, b, nc), METHODS(get_base))
};

struct Derived2 : Base
{
    bool b;

    REFLECT_DERIVED(Derived2, Base, FIELDS(b), METHODS())
};

