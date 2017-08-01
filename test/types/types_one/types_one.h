#pragma once

#include <memory>
#include <vector>

#include "reflect-lib/reflect-macros.h"

namespace types_one
{
struct Base
{
    int i;
    double d;
    std::vector<float> v;

    std::string say_hello() { return "hello!"; }
    virtual std::string what() = 0;

    virtual ~Base() = default;

    REFLECT((Base), FIELDS(i, d, v), METHODS(say_hello, what))
};

// used to test inheritance from C++ classes in python
inline std::string say_what(Base& b)
{
    return b.what();
}

struct VectorTest
{
    std::unique_ptr<std::vector<int>> a;
    std::vector<long> b;
    std::vector<unsigned> get() const { return { 8 }; }
    void set(std::vector<int>) {}
    void set_ref(std::vector<short>& tmp) {
        if (tmp.size() != 0) tmp[0] = 7;
    }

    REFLECT((VectorTest), FIELDS(a, b), METHODS(get, set, set_ref))
};

class NonDefaultConstructible
{
    explicit NonDefaultConstructible(int a_, int b_) : a(a_), b(b_) {}

private:
     const double a;
     const int b;

public:
     REFLECT((NonDefaultConstructible), FIELDS(a, b), METHODS())
};

}  // namespace types_one
