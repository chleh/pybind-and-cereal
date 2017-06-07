#pragma once

#include <memory>
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
    std::string s;

    REFLECT((NoCopy), FIELDS(i, s), METHODS())
};

struct Base
{
    int i;
    double d;
    std::vector<float> v;

    std::string say_hello() { return "hello!"; }
    virtual std::string what() = 0;

    virtual ~Base() = default;

    REFLECT((Base), FIELDS(i, d, v), METHODS(say_hello, what))
};

struct Derived1 : Base
{
    std::string s;
    std::unique_ptr<Base> b;
    NoCopy nc;
    std::unique_ptr<NoCopy> ncp;

    Base& get_base() { return *this; }
    std::string what() override { return "der1"; };

    // Destructor prevents implicit move ctor
    // Derived1(Derived1&&) = default;
    // Derived1() = default;
    // ~Derived1() { std::cout << "~Derived1() s=" << s << '\n'; }

    // TODO test also empty FIELDS()
    // test also empty METHODS()
    REFLECT_DERIVED((Derived1), (Base), FIELDS(s, b, nc, ncp), METHODS(get_base))
};

struct Derived2 : Base
{
    bool b;

    std::string what() override { return "der2"; };

    REFLECT_DERIVED((Derived2), (Base), FIELDS(b), METHODS())
};

template<typename T, typename U>
struct Derived3 : Base
{
    T t;
    U u;
    std::vector<float> v;
    std::vector<float> w;

    std::string what() override { return "der3"; };

    // test unique_ptr methods
    // std::unique_ptr<Base> f(std::unique_ptr<int> const&) { return nullptr; }
    std::unique_ptr<Base> f() { return std::make_unique<Derived1>(); }

    // TODO doesn't work: "Holder classes are only supported for custom types"
    // --> maybe wrap into further class?
    std::unique_ptr<int> fi() { return std::make_unique<int>(); }

    std::unique_ptr<int> const& g() const {
        static auto p = std::make_unique<int>();
        return p;
    }

    // also check if aux types are derived from method arguments and return types
    std::unique_ptr<std::vector<NoCopy>>& h() {
        static auto p = std::make_unique<std::vector<NoCopy>>();
        return p;
    }

    REFLECT_DERIVED((Derived3<T, U>), (Base), FIELDS(t, u, v, w), METHODS(f)) // TODO g, h
};

struct VectorTest
{
    std::unique_ptr<std::vector<int>> a;
    std::vector<long> b;
    std::vector<unsigned> get() const { return {}; }
    void set(std::vector<int>) {}
    void set_ref(std::vector<short>&) {}

    REFLECT((VectorTest), FIELDS(a, b), METHODS(get, set, set_ref))
};

