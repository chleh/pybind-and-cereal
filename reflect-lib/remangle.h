#pragma once

#include <string>

std::string mangle(std::string name);
std::string demangle(const char* mangled_name);

inline std::string remangle(const char* mangled_name) {
    return mangle(demangle(mangled_name));
}

