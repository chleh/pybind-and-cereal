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

}  // namespace test_unique_ptr
