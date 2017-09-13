/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include <algorithm>
#include <iostream>
#include <typeinfo>

#include "reflect-lib/util-macros.h"

using namespace std;

template<typename A, typename B>
struct some____struct
{
};

#define IDENTITY2(TOK1, TOK2) TOK1 TOK2

#define STR(SEP, TOK) , SEP , #TOK

#define GET_NAME3(...) \
    APPLY(IGNORE_HEAD2, 0 FOR_EACH(STR, ", ", __VA_ARGS__))

#define GET_NAME2(...) \
    FOR_EACH(IDENTITY2, , GET_NAME3(__VA_ARGS__))

#define GET_NAME(TYPE) \
    GET_NAME2(EXPAND(TYPE))


#define MANGLE_META2(TYPE, ...) \
    template <> \
    struct Mangle<__VA_ARGS__> { \
        using type = __VA_ARGS__; \
        static constexpr const char* name() { \
            return GET_NAME(TYPE); \
        } \
        static std::string mangled_name() { \
            return mangle(name()); \
        } \
    }

#define MANGLE_META(TYPE) \
    MANGLE_META2(TYPE, EXPAND(TYPE))

std::string mangle(std::string s)
{
    auto replace_str = [](std::string& s, char const c, std::string const& repl) {
        for (auto pos = s.find(c); pos != s.npos; pos = s.find(c, pos + repl.size())) {
            s.replace(pos, 1, repl);
        }
    };

    replace_str(s, ' ', "");
    replace_str(s, '_', "__");
    replace_str(s, ',', "_C");
    replace_str(s, '<', "_L");
    replace_str(s, '>', "_R");
    return s;
}

template <typename T>
struct Mangle;

MANGLE_META((some____struct<int, double>));

int main()
{
    cout << Mangle<some____struct<int, double>>::name() << '\n';
    cout << Mangle<some____struct<int, double>>::mangled_name() << '\n';
    cout << typeid(Mangle<some____struct<int, double>>::type).name() << '\n';
    return 0;
}
