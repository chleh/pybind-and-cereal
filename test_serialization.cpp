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
        Base b;
        b.i = 1;
        b.d = 2.5;

        std::ofstream os("archive.xml");
        cereal::XMLOutputArchive archive(os);

        archive(b);
    }

    {
        auto d1 = std::make_unique<Derived1>();
        d1->i = 10;
        d1->d = 3.14;
        d1->s = "hello";

        std::unique_ptr<Base> b = std::move(d1);

        std::ofstream os("archive.xml");
        cereal::XMLOutputArchive archive(os);

        archive(b);
    }

    {
        std::ifstream is("archive.xml");
        cereal::XMLInputArchive archive(is);

        std::unique_ptr<Base> b;
        archive(b);

        std::cout << typeid(*b).name() << '\n';

        Derived1* d1;
        // assert(typeid(*b) == typeid(*d1));
        d1 = dynamic_cast<Derived1*>(b.get());
        assert(d1 != nullptr);
        assert(d1->i == 10);
        assert(d1->d == 3.14);
        assert(d1->s == "hello");
    }

    return 0;
}
