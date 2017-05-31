#!/usr/bin/python

import test_pybind as tp

d1 = tp.Derived1()
d1.s = "d1"

d1.i = 1
print("d1.i", d1.i)
d1.v.append(1.0)
print("d1.v", d1.v)

d1.v = tp.std__vector__f([1,2,3])
print("d1.v", d1.v)

print("d1.say_hello()", d1.say_hello())

b = d1.get_base()
print(d1, b)

b.i = 8
print("d1.i", d1.i)

d1.nc.i = 7
print("d1.nc.i", d1.nc.i)

print("d1.what()", d1.what())

tmp = tp.Derived1()
tmp.s = "tmp"
d1.b = tmp

d1a = tp.Derived1()
d1a.s = "d1a"

if False:
    d1a.b = tmp  # BOOM! (on deletion)

if d1.b is not None:
    print("d1.b", d1.b, d1.b.i)
else:
    print("d1.b is None")

d1.b = None
# d1.b = 1
print(tmp, tmp.s)
del tmp

print("##### cut #####")

# tmp = tp.Derived1()
# tmp.s = "tmp 2nd"

# d1.b = tmp
