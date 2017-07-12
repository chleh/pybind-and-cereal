
import bindings as tp

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

print("d1.nc.s", d1.nc.s)
print("nc.s \"{}\"".format(nc.s))

try:
    print("d1.nc", d1.nc__MOVE_IN)
except Exception as e:
    print("ERROR", e)
print("d1.nc", d1.nc)

