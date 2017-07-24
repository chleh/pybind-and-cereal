import unittest
import types_one.types_one_a.types_one_a_a

class TestSplitBindings(unittest.TestCase):
    def test_new_instance(self):
        d1 = types_one.types_one_a.types_one_a_a.Derived1()
        d2 = types_one.types_one_a.types_one_a_a.Derived2()
        self.assertIsNotNone(d1)
        self.assertIsNotNone(d2)

if __name__ == '__main__':
    unittest.main()

