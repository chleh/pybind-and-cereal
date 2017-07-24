#pragma once

#include "reflect-lib/pybind.h"

#include "test/types/types_one/types_one_a/types_one_a_a/types_one_a_a.h"

pybind11::class_<types_one::types_one_a::types_one_a_a::Derived2>
bind_Derived2(reflect_lib::Module& m);
