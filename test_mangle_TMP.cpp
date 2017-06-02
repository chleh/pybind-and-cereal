#include <algorithm>
#include <iostream>
#include <typeinfo>

using namespace std;

template<typename A>
struct S
{
    A f(A const&, A*const*) { return A{}; }
    A g;
};

template<typename A, typename B>
struct some____struct
{
};

template<int I, int J, int K>
struct S2
{
};


template<typename T, T I>
struct IntConst
{
};

template<typename T>
struct Name
{
    static std::string name() { return "UNDEFINEDNAME"; }
};


template<typename... Ts>
std::string arg_list()
{
    std::string names[] = { Name<Ts>::name()... };
    std::string res;
    bool first = true;
    for (auto& n : names) {
        if (!first) res += ", "; else first = false;
        res += n;
    }
    return res;
}


template<typename... Ts>
std::string tpl_name_list()
{
    return '<' + arg_list<Ts...>() + '>';
}


// TODO more types...
template<>
struct Name<int> { static std::string name() { return "int"; } };

template<>
struct Name<double> { static std::string name() { return "double"; } };


template<typename T, T I>
struct Name<IntConst<T, I>>
{
    static std::string name() { return std::to_string(I); }
};


// TODO: non-type tpl params...
// Maybe bind them by turning them into std::integral_constant (or IntConst)
// explicitly.


// Member fct ptr
template<typename Res, typename Class, typename... Args>
struct Name<Res (Class::*)(Args...)>
{
    static std::string name() {
        return Name<Res>::name()
            + " (" + Name<Class>::name()
            + "::*)(" + arg_list<Args...>() + ")";
    }
};

// Member ptr
template<typename Res, typename Class>
struct Name<Res (Class::*)>
{
    static std::string name() {
        return Name<Res>::name()
            + ' ' + Name<Class>::name()
            + "::*";
    }
};

// fct ptr
template<typename Res, typename... Args>
struct Name<Res(Args...)>
{
    static std::string name() {
        return Name<Res>::name()
            + " (" + arg_list<Args...>() + ")";
    }
};

template<typename T>
struct Name<T const>
{
    static std::string name() { return Name<T>::name() + " const"; }
};

template<typename T>
struct Name<T*>
{
    static std::string name() { return Name<T>::name() + "*"; }
};

template<typename T>
struct Name<T&>
{
    static std::string name() { return Name<T>::name() + "&"; }
};

template<typename T>
struct Name<T&&>
{
    static std::string name() { return Name<T>::name() + "&&"; }
};



template<typename A, typename B>
struct Name<some____struct<A, B>>
{
    static std::string name() { return "some____struct" + tpl_name_list<A, B>(); }
};

template<typename A>
struct Name<S<A>>
{
    static std::string name() { return "S" + tpl_name_list<A>(); }
};

template<int I, int J, int K>
struct Name<S2<I, J, K>>
{
    // static std::string name() { return "S2<" + std::to_string(I) + '>'; }
    static std::string name() { return "S2" + tpl_name_list<IntConst<int, I>, IntConst<int, J>, IntConst<int, K>>(); }
};



std::string mangle(std::string s)
{
    auto replace_str = [](std::string& s, char const c, std::string const& repl) {
        for (auto pos = s.find(c); pos != s.npos; pos = s.find(c, pos + repl.size())) {
            s.replace(pos, 1, repl);
        }
    };
    // TODO: ", ", "&&", "::", " const"
    replace_str(s, '_', "__");
    replace_str(s, ' ', "_S");
    replace_str(s, ',', "_C");
    replace_str(s, '<', "_L");
    replace_str(s, '>', "_R");
    replace_str(s, '(', "_l");
    replace_str(s, ')', "_r");
    replace_str(s, '&', "_A");
    replace_str(s, '*', "_P");
    replace_str(s, ':', "_c");
    return s;
}

template <typename T>
struct Mangle
{
    using type = T;
    static std::string name() {
        return Name<T>::name();
    }
    static std::string mangled_name() {
        return mangle(name());
    }
};



int h(double) { return 0; }


int main()
{
    cout << Mangle<some____struct<int, double>>::name() << '\n';
    cout << Mangle<some____struct<int, double>>::mangled_name() << '\n';

    cout << Mangle<some____struct<const int&&, decltype(&S<double>::f)>>::name() << '\n';
    cout << Mangle<some____struct<const int&&, decltype(&S<double>::f)>>::mangled_name() << '\n';
    // cout << typeid(Mangle<some____struct<const int&&, decltype(&S<double>::f)>>::type).name() << '\n';

    cout << Mangle<some____struct<decltype(&S<int>::g), double>>::name() << '\n';
    cout << Mangle<some____struct<decltype(&S<int>::g), double>>::mangled_name() << '\n';
    cout << typeid(Mangle<some____struct<decltype(&S<int>::g), double>>::type).name() << '\n';

    cout << Mangle<decltype(h)>::name() << '\n';
    cout << Mangle<decltype(h)>::mangled_name() << '\n';
    cout << typeid(Mangle<decltype(h)>::type).name() << '\n';

    cout << Mangle<S2<5,3,2>>::name() << '\n';
    cout << Mangle<S2<5,3,2>>::mangled_name() << '\n';
    cout << typeid(Mangle<S2<5,3,2>>::type).name() << '\n';
    return 0;
}
