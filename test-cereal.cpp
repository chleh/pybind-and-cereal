#include <iostream>
#include <fstream>
#include <cassert>
#include <typeinfo>

#include "reflect-macros.h"

#include "cereal.h"

struct Base
{
    int i;
    double d;

    void say_hello() {}

    virtual ~Base() = default;

    REFLECT(Base, FIELDS(i, d), METHODS(say_hello))
};


struct Derived1 : Base
{
    std::string s;
    Base b;

    REFLECT_DERIVED(Derived1, Base, FIELDS(s, b), METHODS())
};
REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(Derived1, Base)

/*
REFLECT_DERIVED(Some::NameSpace, template<typename T, typename S>, Derived1, <T, S>,
        ((int, double), (int, std::string)),
        Base, (s, b), (say_hello))

REFLECT_DERIVED(DISABLE_AUTOGEN, Derived1<T, S>, Base, (s, b), (say_hello))
REFLECT_DERIVED_DISABLE_AUTOGEN(Derived1<T, S>, Base, (s, b), (say_hello))
REFLECT_DERIVED(Some::NameSpace, Derived1, Base, (s, b), (say_hello))

REFLECT_DERIVED(Some::NameSpace, Derived1, Base, (FIELDS s, b), (METHODS say_hello))
*/


struct Derived2 : Base
{
    bool b;

    REFLECT_DERIVED(Derived2, Base, FIELDS(b), METHODS())
};
// REGISTER_POLYMORPHIC_TYPE_FOR_SERIALIZATION(Derived2, Base)



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
