#include "part1.h"

pybind11::class_<types_one::types_one_a::types_one_a_a::Derived1> bind_Derived1(
    reflect_lib::Module& m)
{
    return m.bind<types_one::types_one_a::types_one_a_a::Derived1>();
}
