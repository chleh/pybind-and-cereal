#include <iostream>
#include <fstream>

#include "serialization_types1.h"

int main()
{
    auto d1 =
        std::make_unique<types_one::types_one_a::types_one_a_a::Derived1>();
    d1->i = 10;
    d1->d = 3.14;
    d1->s = "hello";

    std::unique_ptr<types_one::Base> b = std::move(d1);

    auto d3 = std::make_unique<types_one::types_one_b::Derived3<int, double>>();
    d3->i = 9;
    d3->t = 42;
    d3->u = 2.5;

    std::ofstream os("archive.xml");
    cereal::XMLOutputArchive archive(os);

    archive(b);

    // Note: saving as derived type and reading as base type does not work!
    // That might be a problem for using cereal in production
    archive(std::unique_ptr<types_one::Base>(std::move(d3)));
    return 0;
}
