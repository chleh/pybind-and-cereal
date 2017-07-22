import unittest
import types_one
import types_one.types_one_b
import types_one.types_one_a
import types_one.types_one_a.types_one_a_a

class TestCopyable(unittest.TestCase):
    def test_copy_construction(self):
        ### copy copyable type
        d2 = types_one.types_one_a.types_one_a_a.Derived2()
        self.assertFalse(d2.b)
        d2.b = True

        d2_2 = types_one.types_one_a.types_one_a_a.Derived2(d2)
        self.assertTrue(d2_2.b)


if __name__ == '__main__':
    unittest.main()

