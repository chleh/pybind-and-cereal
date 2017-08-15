#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

namespace test_unique_ptr
{
struct UniquePtrTest {
    int i_up_cr(std::unique_ptr<UniquePtrTest> const& p)
    {
        if (p)
            return 2 * (p->i);
        return 0;
    }

    int i_up_r(std::unique_ptr<UniquePtrTest>& p)
    {
        if (p)
            return 3 * (p->i);
        return 0;
    }

    int i_up_rr(std::unique_ptr<UniquePtrTest>&& p)
    {
        if (p)
            return 5 * (p->i);
        return 0;
    }

    int i_up(std::unique_ptr<UniquePtrTest> p)
    {
        if (p)
            return 7 * (p->i);
        return 0;
    }

    int i;

    REFLECT((UniquePtrTest), FIELDS(i), METHODS(i_up_cr, i_up_r, i_up_rr, i_up))
};

}  // namespace test_unique_ptr
