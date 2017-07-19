#include <iostream>
#include <fstream>
#include <cassert>
#include <typeinfo>

// note: keep that order!
#include "serialization_types2.h"

#include "serialization_types1.h"


int main()
{
    std::ifstream is("archive.xml");
    cereal::XMLInputArchive archive(is);

    std::cout << "Loading...\n";

    std::cout << "... first...\n";
    std::unique_ptr<Base> b;
    archive(b);

    std::cout << "... second...\n";
    std::unique_ptr<Base> b3;
    archive(b3);

    std::cout << "End Loading.\n";

    std::cout << typeid(*b).name() << '\n';

    Derived1* d1;
    // assert(typeid(*b) == typeid(*d1));
    d1 = dynamic_cast<Derived1*>(b.get());
    assert(d1 != nullptr);
    assert(d1->i == 10);
    assert(d1->d == 3.14);
    assert(d1->s == "hello");

    Derived3<int, double>* d3;
    d3 = dynamic_cast<Derived3<int, double>*>(b3.get());
    assert(d3 != nullptr);

    return 0;
}
