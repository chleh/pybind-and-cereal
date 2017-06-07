#!/usr/bin/python

import test_pybind as tp

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

### check that error messages are generated
try:
    print(d1.b__COPY_IN)
except Exception as e:
    print("ERROR", e)

### check that error messages are generated
try:
    print(d1.b__MOVE_IN)
except Exception as e:
    print("ERROR", e)

print("new: tmp")
tmp = tp.Derived1()
tmp.s = "tmp"  # Note standard python strings are immutable. String assignment always copies

### check move assignment of std::unique_ptr
d1.b__MOVE_IN = tmp
# d1.b__COPY_IN = tmp  ## tmp is not copyable!

print("tmp after move", tmp)
print("tmp.s after move \"{}\"".format(tmp.s))
print("d1.b", d1.b, d1.b.s)

### unique_ptr holding non-copyable type
d1.ncp__MOVE_IN = tp.NoCopy()
try:
    d1.ncp__MOVE_IN = tp.Derived1()
except TypeError as e:
    print("ERROR", e)
print("d1.ncp", d1.ncp)
d1.ncp = None

print("del d1")
del d1

if False:
    # BOOM! -- Fixed in the current implementation
    # The python interpreter maybe caches the tmp object.
    # This assignmen causes a segfault.
    # Conclusion: Moving around pointers on the C++ side is dangerous!
    #             And may cause hard to find bugs.
    # At the bottom of the problem is that the lifetime of tmp is governed by d1
    # after the assignment above. Python does not know about this.
    # The reference tmp is invalidated by deleting d1.
    # The same can happen if one receives references to C++ objects, which are
    # destroyed subsequently without Python noticing it.
    tmp.s = "500000000000000000000000000000000000000000000000000000000000000000" * 100

    # python still references the "stolen" object :-(
    print("tmp after move", tmp)
    print("tmp.s after move \"{}\"".format(tmp.s))

del tmp

if False:
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


### Testing move-only member
d1 = tp.Derived1()

nc = tp.NoCopy()
nc.s = "hello!"
print("nc.s \"{}\"".format(nc.s))
print("d1.nc.s", d1.nc.s)

print("move assign")
d1.nc__MOVE_IN = nc

### try copying non-copyable type
try:
    thrown = False
    tp.NoCopy(nc)
except TypeError:
    thrown = True
assert thrown
try:
    thrown = False
    tp.Derived1(d1)
except TypeError:
    thrown = True
assert thrown

### copy copyable type
d2 = tp.Derived2()
d2_2 = tp.Derived2(d2)


print("d1.nc.s", d1.nc.s)
print("nc.s \"{}\"".format(nc.s))

try:
    print("d1.nc", d1.nc__MOVE_IN)
except Exception as e:
    print("ERROR", e)
print("d1.nc", d1.nc)


print("### vector test")
vt = tp.VectorTest()
print("before assignment: vt.a", type(vt.a), vt.a)

## move-assign std::vector<int>, that's special,
# because custom holder type of some default type
tmp = tp.all_types["std::vector<int, std::allocator<int> >"]()
# TODO there might be garbage collection issues
tmp2 = tp.make_ref(tmp)
print("tmp2", type(tmp2), tmp2)
vt.a__MOVE_IN2 = tmp2

print("after assignment: vt.a", type(vt.a), vt.a)
vt.a.append(1)
print("after append: vt.a", vt.a)

print(vt.b)
print(vt.get())
vt.set(tp.all_types["std::vector<int, std::allocator<int> >"]([1,2,3]))


### test unique_ptr fcts

d3 = tp.all_types["Derived3<int, int>"]();
res = d3.f()
print("res", res, type(res))

