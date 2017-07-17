import unittest
import bindings as tp

class TestCopyable(unittest.TestCase):
    def test_copy_construction(self):
        ### copy copyable type
        d2 = tp.Derived2()
        self.assertFalse(d2.b)
        d2.b = True

        d2_2 = tp.Derived2(d2)
        self.assertTrue(d2_2.b)


if __name__ == '__main__':
    unittest.main()

