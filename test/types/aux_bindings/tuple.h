/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#pragma once

#include <tuple>

#include "reflect-lib/reflect-macros.h"

namespace aux_bindings
{
struct ContainsTuple
{
    std::tuple<int, double> t;

    REFLECT((ContainsTuple), FIELDS(t), METHODS())
};

}  // namespace pickle_types
