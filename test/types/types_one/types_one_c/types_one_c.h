#pragma once

#include <string>
#include "reflect-lib/reflect-macros.h"

namespace types_one
{
namespace types_one_c
{
struct Base
{
    Base(std::string) {}
    Base(Base const&) {}
    Base(Base&&) {}

    virtual std::string what() = 0;

    virtual ~Base() = default;

    REFLECT((Base), FIELDS(), METHODS(what))
};
}  // namespace types_one_c
}  // namespace types_one
