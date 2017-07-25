import unittest
import types_one.types_one_a.types_one_a_a

class TestDerived1(unittest.TestCase):
    def setUp(self):
        self.d1 = types_one.types_one_a.types_one_a_a.Derived1()

    def test_all_types_in_module(self):
        self.assertTrue("all_types" in dir(types_one.types_one_a.types_one_a_a))

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

        # TODO only one all_types dict for all modules
        v = types_one.all_types[
                "std::vector<float, std::allocator<float> >"]([1,2,3])
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

    def test_member_of_member(self):
        self.d1.nc.i = 7
        self.assertEqual(7, self.d1.nc.i)

    def test_method_overridden(self):
        self.assertEqual("der1", self.d1.what())

    def test_member_accessor_write_only(self):
        # ...__COPY_IN and ...__MOVE_IN are for write access only
        with self.assertRaises(KeyError):
            a = self.d1.b__COPY_IN

        with self.assertRaises(KeyError):
            a = self.d1.b__MOVE_IN

    def test_move_assignment(self):
        tmp = types_one.types_one_a.types_one_a_a.Derived1()
        tmp.s = "tmp"

        self.d1.b__MOVE_IN = tmp

        self.assertEqual("tmp", self.d1.b.s)
        self.assertEqual("", tmp.s)

        ### unique_ptr holding non-copyable type
        self.d1.ncp__MOVE_IN = types_one.types_one_a.NoCopy()
        self.assertIsNotNone(self.d1.ncp)

        with self.assertRaises(TypeError):
            # incompatible types
            self.d1.ncp__MOVE_IN = types_one.types_one_a.types_one_a_a.Derived1()

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


if __name__ == '__main__':
    unittest.main()

