#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

#include "test/types/types_one/types_one_a/types_one_a.h"

namespace types_one
{
namespace types_one_a
{
namespace types_one_a_a
{
struct Derived1 : Base {
    std::string s;
    std::unique_ptr<Base> b;
    NoCopy nc;
    std::unique_ptr<NoCopy> ncp;

    Base& get_base() { return *this; }
    std::string what() override { return "der1"; }

    // Destructor prevents implicit move ctor
    // Derived1(Derived1&&) = default;
    // Derived1() = default;
    // ~Derived1() { std::cout << "~Derived1() s=" << s << '\n'; }

    // TODO test also empty FIELDS()
    // test also empty METHODS()
    REFLECT_DERIVED((Derived1), (Base), FIELDS(s, b, nc, ncp),
                    METHODS(get_base))
};

struct Derived2 : Base {
    bool b;

    std::string what() override { return "der2"; }

    REFLECT_DERIVED((Derived2), (Base), FIELDS(b), METHODS())
};

}  // namespace types_one_a_a
}  // namespace types_one_a
}  // namespace types_one
