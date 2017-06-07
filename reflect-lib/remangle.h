#pragma once

#include <string>

std::string mangle(std::string name);

// demangle typeid(...).name()
std::string demangle(const char* mangled_name);

// demangle result of mangle()
std::string demangle2(std::string name);

inline std::string remangle(const char* mangled_name) {
    return mangle(demangle(mangled_name));
}

