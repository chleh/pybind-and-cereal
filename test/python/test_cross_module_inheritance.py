import unittest
import types_one.types_one_b

class TestCrossModule(unittest.TestCase):
    def test_cross_module(self):
        ncd = types_one.types_one_b.NoCopyDerived()

        self.assertEqual(ncd.n(), "NoCopy")
        self.assertEqual(ncd.m(), "NoCopyDerived")


if __name__ == '__main__':
    unittest.main()

