
# add_library(reflection SHARED remangle.cpp)
add_library(reflection remangle.cpp)
target_compile_options(reflection PRIVATE -fPIC)
