/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#pragma once

#include <iostream>
#include <memory>
#include "reflect-lib/reflect-macros.h"

namespace pickle_types
{
struct Empty {
    virtual std::string what() const = 0;

    virtual ~Empty() {
        std::cout << "!!!! destroying Empty " << typeid(*this).name() << std::endl;
    }

    REFLECT((Empty), FIELDS(), METHODS(what))
};

struct DerivedFromEmptyInt : Empty {
    std::string what() const override { return std::to_string(i); }
    int i;

    REFLECT_DERIVED((DerivedFromEmptyInt), (Empty), FIELDS(i), METHODS())
};

struct DerivedFromEmptyString : Empty {
    std::string what() const override { return s; }
    std::string s;

    REFLECT_DERIVED((DerivedFromEmptyString), (Empty), FIELDS(s), METHODS())
};

struct OwnsEmpty {
    std::unique_ptr<Empty> e;

    REFLECT((OwnsEmpty), FIELDS(e), METHODS())
};

struct ContainsVectorOfEmpty {
    ContainsVectorOfEmpty() = default;

    ContainsVectorOfEmpty(ContainsVectorOfEmpty&&) = default;
    // prevent wrong deduction of being copy constructible
    // caused by std::vector always __declaring__ a copy ctor, even if the
    // item type is not copy constructible.
    ContainsVectorOfEmpty(ContainsVectorOfEmpty const&) = delete;

    // std::vector<std::unique_ptr<Empty>> v;
    std::vector<std::shared_ptr<Empty>> v;
    std::vector<std::unique_ptr<Empty>> u;

    REFLECT((ContainsVectorOfEmpty), FIELDS(v, u), METHODS())
};

struct ContainsVectorOfIntString
{
    std::vector<std::string> vs;
    std::vector<int> vi;

    REFLECT((ContainsVectorOfIntString), FIELDS(vs, vi), METHODS())
};

}  // namespace pickle_types
