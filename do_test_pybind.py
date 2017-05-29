#!/usr/bin/python

import test_pybind as tp

d1 = tp.Derived1()

d1.i = 1
print("d1.i", d1.i)

print("d1.say_hello()", d1.say_hello())

b = d1.get_base()
print(d1, b)

b.i = 8
print("d1.i", d1.i)

