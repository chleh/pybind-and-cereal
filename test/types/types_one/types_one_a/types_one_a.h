/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#pragma once

#include <memory>
#include "reflect-lib/reflect-macros.h"

#include "test/types/types_one/types_one.h"

namespace types_one
{
namespace types_one_a
{
struct NoCopy
{
    NoCopy() = default;
    NoCopy(NoCopy&&) = default;
    NoCopy(NoCopy const&) = delete;
    NoCopy& operator=(NoCopy&&) = default;
    NoCopy& operator=(NoCopy const&) = delete;

    virtual std::string m() const { return "NoCopy"; }
    virtual std::string n() const { return "NoCopy"; }

    int i = 10;
    std::string s;

    REFLECT((NoCopy), FIELDS(i, s), METHODS(m, n))
};
}  // namespace types_one_a
}  // namespace types_one
