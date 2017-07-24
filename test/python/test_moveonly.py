import unittest
import types_one.types_one_a.types_one_a_a

class TestMoveOnly(unittest.TestCase):
    def setUp(self):
        self.d1 = types_one.types_one_a.types_one_a_a.Derived1()

    def test_move_assign(self):
        nc = types_one.types_one_a.NoCopy()
        nc.s = "hello!"

        self.d1.nc__MOVE_IN = nc
        self.assertEqual("hello!", self.d1.nc.s)

    def test_copy_construct(self):
        nc = types_one.types_one_a.NoCopy()

        with self.assertRaises(TypeError):
            types_one.types_one_a.NoCopy(nc)

        with self.assertRaises(TypeError):
            types_one.types_one_a.types_one_a_a.Derived1(self.d1)

if __name__ == '__main__':
    unittest.main()

