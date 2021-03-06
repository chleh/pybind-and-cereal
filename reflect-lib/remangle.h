/* Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
 *
 * All rights reserved. Use of this source code is governed by a
 * BSD-style license that can be found in the LICENSE file.
 */

#pragma once

#include <string>

namespace reflect_lib
{
std::string mangle(std::string name);

// demangle typeid(...).name()
std::string demangle(const char* mangled_name);

template <typename T>
std::string demangle()
{
    return demangle(typeid(T).name());
}

// demangle result of mangle()
std::string demangle2(std::string mangled_name);

inline std::string remangle(const char* mangled_name)
{
    return mangle(demangle(mangled_name));
}

std::string strip_namespaces(std::string name);

std::string strip_outermost_namespace(std::string name);

bool is_scoped(std::string const& name);

std::string get_namespaces(std::string const& name);

}  // namespace reflect_lib
