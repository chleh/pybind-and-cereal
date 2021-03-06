import unittest
import types_one

class TestNonDefaultCtor(unittest.TestCase):
    def setUp(self):
        pass

    def test_construction(self):
        ndc = types_one.NonDefaultConstructible(5.0, 4)
        self.assertEqual(5.0, ndc.a)
        self.assertEqual(4, ndc.b)

if __name__ == '__main__':
    unittest.main()

