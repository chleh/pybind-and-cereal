#pragma once

#include "reflect-lib/cereal.h"

#include "test/types/types_one/types_one_a/types_one_a_a/types_one_a_a.h"
#include "test/types/types_one/types_one_b/types_one_b.h"

REGISTER_TYPE_FOR_SERIALIZATION(types_one::Base)
REGISTER_TYPE_FOR_SERIALIZATION(types_one::VectorTest)

REGISTER_TYPE_FOR_SERIALIZATION(types_one::types_one_a::NoCopy)

REGISTER_DERIVED_TYPE_FOR_SERIALIZATION(
    types_one::types_one_a::types_one_a_a::Derived1)
REGISTER_DERIVED_TYPE_FOR_SERIALIZATION(
    types_one::types_one_a::types_one_a_a::Derived2)

REGISTER_DERIVED_TYPE_FOR_SERIALIZATION(
    types_one::types_one_b::Derived3<int, double>)
