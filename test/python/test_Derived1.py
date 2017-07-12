#!/usr/bin/python

import bindings as tp

print(dir(tp))
print("all_types", tp.all_types)

d1 = tp.Derived1()
d1.s = "d1"

d1.i = 1
print("d1.i", d1.i)
d1.v.append(1.0)
print("d1.v", d1.v)

# d1.v = tp.std__vector__f([1,2,3])
# print("d1.v", d1.v)
d1.v = tp.all_types["std::vector<float, std::allocator<float> >"]([1,2,3])
print("d1.v", d1.v)

print("d1.say_hello()", d1.say_hello())

# b = d1.get_base()
# print(d1, b)

# b.i = 8
# print("d1.i", d1.i)

d1.nc.i = 7
print("d1.nc.i", d1.nc.i)

print("d1.what()", d1.what())
