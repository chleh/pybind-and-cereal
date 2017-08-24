#pragma once
#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

namespace pickle_types
{
struct Empty {
    virtual ~Empty() = default;

    REFLECT((Empty), FIELDS(), METHODS())
};

struct DerivedFromEmptyInt : Empty {
    int i;

    REFLECT_DERIVED((DerivedFromEmptyInt), (Empty), FIELDS(i), METHODS())
};

struct DerivedFromEmptyString : Empty {
    std::string s;

    REFLECT_DERIVED((DerivedFromEmptyString), (Empty), FIELDS(s), METHODS())
};

}  // namespace pickle_types
