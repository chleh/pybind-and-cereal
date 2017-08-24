#pragma once
#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

namespace pickle_types
{
struct Empty {
    virtual std::string what() const = 0;

    virtual ~Empty() = default;

    REFLECT((Empty), FIELDS(), METHODS(what))
};

struct DerivedFromEmptyInt : Empty {
    std::string what() const override { return std::to_string(i); }
    int i;

    REFLECT_DERIVED((DerivedFromEmptyInt), (Empty), FIELDS(i), METHODS())
};

struct DerivedFromEmptyString : Empty {
    std::string what() const override { return s; }
    std::string s;

    REFLECT_DERIVED((DerivedFromEmptyString), (Empty), FIELDS(s), METHODS())
};

struct OwnsEmpty {
    std::unique_ptr<Empty> e;

    REFLECT((OwnsEmpty), FIELDS(e), METHODS())
};

}  // namespace pickle_types
