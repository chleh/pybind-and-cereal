#pragma once

#include "reflect-lib/cereal.h"

#include "test/types/types_one/types_one_b/types_one_b.h"

REGISTER_DERIVED_TYPE_FOR_SERIALIZATION(
    types_one::types_one_b::Derived3<std::string, double>)
