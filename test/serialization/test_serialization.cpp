#include <iostream>
#include <fstream>
#include <cassert>
#include <typeinfo>

#include "test_serialization.h"


int main()
{
    std::cout << typeid(decltype(Base::Meta::fields())).name() << '\n';
    std::cout << typeid(decltype(Base::Meta::methods())).name() << '\n';
    std::cout << typeid(decltype(Derived1::Meta::fields())).name() << '\n';
    std::cout << typeid(decltype(Derived1::Meta::methods())).name() << '\n';
    std::cout << typeid(decltype(Derived2::Meta::fields())).name() << '\n';
    std::cout << typeid(decltype(Derived2::Meta::methods())).name() << '\n';


    {
        auto d1 = std::make_unique<Derived1>();
        d1->i = 10;
        d1->d = 3.14;
        d1->s = "hello";

        std::unique_ptr<Base> b = std::move(d1);

        auto d3 = std::make_unique<Derived3<int, double>>();
        d3->i = 9;
        d3->t = 42;
        d3->u = 2.5;

        std::ofstream os("archive.xml");
        cereal::XMLOutputArchive archive(os);

        archive(b);

        // Note: saving as derived type and reading as base type does not work!
        // That might be a problem for using cereal in production
        archive(std::unique_ptr<Base>(std::move(d3)));
    }

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
    }

    return 0;
}
