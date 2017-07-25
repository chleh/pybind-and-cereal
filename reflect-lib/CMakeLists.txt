file(GLOB sources LIST_DIRECTORIES false *.cpp *.h)
add_library(reflection ${sources})
target_compile_options(reflection PRIVATE -fPIC)
