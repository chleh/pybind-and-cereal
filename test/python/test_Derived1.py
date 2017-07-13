#!/usr/bin/python

import unittest
import bindings as tp

class TestDerived1(unittest.TestCase):
    def setUp(self):
        self.d1 = tp.Derived1()

    def test_all_types_in_module(self):
        self.assertTrue("all_types" in dir(tp))

    def test_member_string(self):
        self.d1.s = "d1"
        self.assertEqual("d1", self.d1.s)

    def test_member_int(self):
        self.d1.i = 1
        self.assertEqual(1, self.d1.i)

    def test_member_vector(self):
        self.d1.v.append(1.0)
        self.assertEqual(1, len(self.d1.v))
        self.assertEqual(1.0, self.d1.v[0])

        v = tp.all_types["std::vector<float, std::allocator<float> >"]([1,2,3])
        self.d1.v = v  # copy assignment
        self.assertEqual(3, len(self.d1.v))

        # also checks that d1.v is iterable
        for i_, v_ in enumerate(self.d1.v[:]):
            self.assertEqual(i_ + 1.0, v_)

        # check that the assignment above really copied
        v[0] = 5
        self.assertEqual(1, self.d1.v[0])

    def test_method_string(self):
        self.assertEqual("hello!", self.d1.say_hello())

    # TODO
    # b = d1.get_base()
    # print(d1, b)
    #
    # b.i = 8
    # print("d1.i", d1.i)

    def test_member_of_member(self):
        self.d1.nc.i = 7
        self.assertEqual(7, self.d1.nc.i)

    def test_method_overridden(self):
        self.assertEqual("der1", self.d1.what())

    def test_member_accessor_write_only(self):
        # ...__COPY_IN and ...__MOVE_IN are for write access only
        with self.assertRaises(KeyError):
            a = self.d1.b__COPY_IN
        # print(type(e.exception), e.exception)

        with self.assertRaises(KeyError):
            a = self.d1.b__MOVE_IN

    def test_move_assignment(self):
        tmp = tp.Derived1()
        tmp.s = "tmp"

        self.d1.b__MOVE_IN = tmp
        # d1.b__COPY_IN = tmp  ## tmp is not copyable!

        # print("tmp.s after move \"{}\"".format(tmp.s))
        self.assertEqual("tmp", self.d1.b.s)
        self.assertEqual("", tmp.s)

        ### unique_ptr holding non-copyable type
        self.d1.ncp__MOVE_IN = tp.NoCopy()
        self.assertIsNotNone(self.d1.ncp)

        with self.assertRaises(TypeError):
            # incompatible types
            self.d1.ncp__MOVE_IN = tp.Derived1()

        self.assertIsNotNone(self.d1.ncp)

        # test assignment of None
        self.d1.ncp = None
        self.assertIsNone(self.d1.ncp)


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


if __name__ == '__main__':
    unittest.main()

