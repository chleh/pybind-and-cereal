/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#include <iostream>
#include <typeinfo>

using namespace std;

template<typename T>
void f()
{
    cout << "type\n";
}

template<long I> // catches most
void f()
{
    cout << "long " << I << '\n';
}

template<int const* P> // specialization for any pointer type possible
void f()
{
    cout << "int ptr " << P << '\n';
}

template<long const* P> // specialization for any pointer type possible
void f()
{
    cout << "long ptr " << P << '\n';
}

template<void(*F)()>
void f()
{
    cout << "fct " << F << '\n';
}

template<double(*F)()> // specialization for any function pointer type possible
void f()
{
    cout << "fct (double) " << F << '\n';
}

// template<auto I> // C++17
// void f()
// {
//     cout << "auto " << I << '\n';
//     cout << typeid(decltype(I)).name() << '\n';
// }

// template<bool I>
// void f()
// {
//     cout << "bool " << I << '\n';
// }

// template<unsigned int I>
// void f()
// {
//     cout << "uint " << I << '\n';
// }

void g() {}
double h() {}
static constexpr int i = 43;
static constexpr long l = -43;

int main()
{
    f<1>();
    f<false>();
    f<g>();
    f<h>();
    f<&i>();
    f<&l>();
    f<double>();
    decltype(1) a;
    return 0;
}

