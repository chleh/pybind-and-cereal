import bindings as tp

print("### vector test")
vt = tp.VectorTest()
print("before assignment: vt.a", type(vt.a), vt.a)

## move-assign std::vector<int>
tmp = tp.all_types["std::vector<int, std::allocator<int> >"]()
vt.a__MOVE_IN = tmp

print("after assignment: vt.a", type(vt.a), vt.a)
vt.a.append(1)
print("after append: vt.a", vt.a)

print(vt.b)
print(vt.get())
vt.set(tp.all_types["std::vector<int, std::allocator<int> >"]([1,2,3]))
vt.set_ref(tp.all_types["std::vector<short, std::allocator<short> >"]([1,2,3]))
