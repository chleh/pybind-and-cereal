/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

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

struct NoCopy {
    explicit NoCopy(int i_) : i(i_) {}
    NoCopy() = default;
    NoCopy(NoCopy&&) = default;
    NoCopy(NoCopy const&) = delete;
    NoCopy& operator=(NoCopy&&) = default;
    NoCopy& operator=(NoCopy const&) = delete;

    int i;

    REFLECT((NoCopy), FIELDS(i), METHODS())
};

struct CtorTest {
    CtorTest() {}
    CtorTest(std::unique_ptr<CtorTest>&& c_, NoCopy&& n_, int i_)
        : c(std::move(c_)), n(std::move(n_)), i(i_)
    {
    }
    CtorTest(CtorTest const&) = delete;
    CtorTest(CtorTest&& other)
        : c(std::move(other.c)), n(std::move(other.n)), i(other.i)
    {
    }

    std::unique_ptr<CtorTest> c;
    NoCopy n;
    int i = 0;

    void f(CtorTest& c_) { c_.i = 8; }

    REFLECT((CtorTest), FIELDS(c, n, i), METHODS(f))
};

}  // namespace test_unique_ptr
