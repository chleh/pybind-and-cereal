#pragma once

#include <tuple>

#include "reflect-lib/reflect-macros.h"

namespace aux_bindings
{
struct ContainsTuple
{
    std::tuple<int, double> t;

    REFLECT((ContainsTuple), FIELDS(t), METHODS())
};

}  // namespace pickle_types
