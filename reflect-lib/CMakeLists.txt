# Copyright (c) 2017 Christoph Lehmann c_lehmann@posteo.de
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

file(GLOB sources LIST_DIRECTORIES false *.cpp *.h)
add_library(reflection ${sources})
target_compile_options(reflection PRIVATE -fPIC)
