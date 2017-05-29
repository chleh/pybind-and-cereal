#include <iostream>
#include <fstream>
#include <cassert>


#define ADD_MEMBER_ACCESS(var_name, member) , cereal::make_nvp(#member, var_name . member)
#define MEMBER_ACCESS_LIST(var_name, ...) \
    FOR_EACH(ADD_MEMBER_ACCESS, var_name, __VA_ARGS__)




#include <cereal/archives/xml.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/base_class.hpp>

#if 1

template <typename Archive, typename... Ts>
decltype(auto)
call_helper(Archive&& archive, Ts&&... xs)
{
    return archive(std::forward<Ts>(xs)...);
}

#define GEN_SER(CLASS, ...) \
    template<class Archive> \
    void save(Archive & archive, CLASS const& i) \
    { \
        call_helper(archive MEMBER_ACCESS_LIST(i, __VA_ARGS__)); \
    } \
    template<class Archive> \
    void load(Archive & archive, CLASS & i) \
    { \
        call_helper(archive MEMBER_ACCESS_LIST(i, __VA_ARGS__)); \
    }

#define GEN_SER_DERIVED(CLASS, BASE, ...) \
    template<class Archive> \
    void save(Archive & archive, CLASS const& i) \
    { \
        archive(cereal::base_class<BASE>(&i) MEMBER_ACCESS_LIST(i, __VA_ARGS__)); \
    } \
    template<class Archive> \
    void load(Archive & archive, CLASS & i) \
    { \
        archive(cereal::base_class<BASE>(&i) MEMBER_ACCESS_LIST(i, __VA_ARGS__)); \
    } \
    CEREAL_REGISTER_TYPE(CLASS); \
    CEREAL_REGISTER_POLYMORPHIC_RELATION(BASE, CLASS)

#else

#define GEN_SER(...) \
    template<class Archive> \
    void serialize(Archive & archive) \
    { \
        archive(__VA_ARGS__); \
    }

#define GEN_SER_DERIVED(BASE, ...) \
    template<class Archive> \
    void serialize(Archive & archive) \
    { \
        archive(cereal::base_class<BASE>(this), __VA_ARGS__); \
    }

#endif

struct Base
{
    int i;
    double d;

    void say_hello() {}

    virtual ~Base() = default;
};
REFLECT(Base, FIELDS(i, d), METHODS())


struct Derived1 : Base
{
    std::string s;
    Base b;
};
REFLECT_DERIVED(Derived1, Base, FIELDS(s, b), METHODS(say_hello))

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
};
REFLECT_DERIVED(Derived2, Base, FIELDS(b), METHODS())



int main()
{
    {
        Base b;
        b.i = 1;
        b.d = 2.5;

        std::ofstream os("archive.xml");
        cereal::XMLOutputArchive archive(os);

        archive(b);
    }

    {
        std::ifstream is("archive.xml");
        cereal::XMLInputArchive archive(is);

        Base b;
        archive(b);

        std::cout << b.i << " " << b.d << "\n";
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
