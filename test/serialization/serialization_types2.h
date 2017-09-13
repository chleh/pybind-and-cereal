/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#pragma once

#include "reflect-lib/cereal.h"

#include "test/types/types_one/types_one_b/types_one_b.h"

REGISTER_DERIVED_TYPE_FOR_SERIALIZATION(
    types_one::types_one_b::Derived3<std::string, double>)
