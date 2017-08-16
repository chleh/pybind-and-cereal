#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

namespace test_unique_ptr
{
struct RValueRefTest {
    int f(RValueRefTest&& r)
    {
        return 2 * r.i;
    }

    int i;

    REFLECT((RValueRefTest), FIELDS(i), METHODS(f))
};

struct CtorTest {
    CtorTest() {}
    CtorTest(std::unique_ptr<CtorTest>&& c_, int i_) : c(std::move(c_)), i(i_)
    {
    }
    CtorTest(CtorTest const&) = delete;
    CtorTest(CtorTest&& other) : c(std::move(other.c)), i(other.i) {}

    std::unique_ptr<CtorTest> c;
    int i = 0;

    void f(CtorTest& c_) { c_.i = 8; }

    REFLECT((CtorTest), FIELDS(c, i), METHODS(f))
};

}  // namespace test_unique_ptr
