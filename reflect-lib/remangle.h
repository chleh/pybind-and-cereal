#pragma once

#include <string>

namespace reflect_lib
{
std::string mangle(std::string name);

// demangle typeid(...).name()
std::string demangle(const char* mangled_name);

// demangle result of mangle()
std::string demangle2(std::string mangled_name);

inline std::string remangle(const char* mangled_name) {
    return mangle(demangle(mangled_name));
}

std::string strip_namespaces(std::string name);

}  // namespace reflect_lib
