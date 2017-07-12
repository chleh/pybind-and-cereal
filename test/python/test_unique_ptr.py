import bindings as tp

### test unique_ptr fcts

d3 = tp.all_types["Derived3<int, int>"]();
res = d3.f()
print("res", res, type(res))

