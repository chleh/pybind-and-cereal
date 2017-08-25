#!/usr/bin/python

import pickle
import tst_vec

s = tst_vec.S()

print("C++ish")
s.v = tst_vec.vectorInt([ 1, 2, 3 ])
print("list")
s.v = [ 1, 2, 3 ]
print("tuple")
s.v = ( 4, 5 )
print("generator")
def f(i):
    print(i)
    return i*i
gen = (f(i) for i in range(5))
print(type(gen))
s.v = gen

with open("test.pickle", "wb") as fh:
    pickle.dump(s, fh, -1)

with open("test.pickle", "rb") as fh:
    new_s = pickle.load(fh)

for o, n in zip(s.v, new_s.v):
    print(o, n)
    assert o == n

