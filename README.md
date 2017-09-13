This library tries to facilitate the creation of both serialization code and
Python bindings for (rather) arbitrary C++ objects.
It provides a simple macro-based reflection mechanism, based on which
serialization code and Python bindings can be generated with only little extra
code written by the user. Check the tests in order to see, what still has to be
done manually.

The main work has already been done by the pybind and cereal projects:
 * https://github.com/pybind/pybind11
 * https://github.com/USCiLab/cereal

