#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

#include "test/types/types_one/types_one.h"

namespace types_one
{
namespace types_one_a
{
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
}  // namespace types_one_a
}  // namespace types_one
