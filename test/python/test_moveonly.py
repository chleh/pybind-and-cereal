import unittest
import types_one as tp

class TestMoveOnly(unittest.TestCase):
    def setUp(self):
        self.d1 = tp.Derived1()

    def test_move_assign(self):
        nc = tp.NoCopy()
        nc.s = "hello!"

        self.d1.nc__MOVE_IN = nc
        self.assertEqual("hello!", self.d1.nc.s)

    def test_copy_construct(self):
        nc = tp.NoCopy()

        with self.assertRaises(TypeError):
            tp.NoCopy(nc)

        with self.assertRaises(TypeError):
            tp.Derived1(self.d1)

if __name__ == '__main__':
    unittest.main()

