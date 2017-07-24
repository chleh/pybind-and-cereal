import unittest
import types_one.types_one_a.types_one_a_a

class TestSmartPtr(unittest.TestCase):
    def setUp(self):
        self.d1 = types_one.types_one_a.types_one_a_a.Derived1()
        self.d2 = types_one.types_one_a.types_one_a_a.Derived1()

    def test_move_nullptr(self):
        self.d1.b__MOVE_IN = types_one.types_one_a.types_one_a_a.Derived1()
        self.assertIsNotNone(self.d1.b)

        self.assertIsNone(self.d2.b)

        # nullptr is moved as nullptr
        self.d1.b__MOVE_IN = self.d2.b
        self.assertIsNone(self.d1.b)

    def test_copy_nullptr(self):
        # TODO test non-copyable non-pointer member
        self.d1.b__MOVE_IN = types_one.types_one_a.types_one_a_a.Derived1()
        self.assertIsNotNone(self.d1.b)

        self.assertIsNone(self.d2.b)

        # nullptr gets copied as nullptr
        self.d1.b__COPY_IN = self.d2.b
        self.assertIsNone(self.d1.b)

    def test_copy_raises(self):
        with self.assertRaises(TypeError):
            self.d1.b__COPY_IN = self.d2

if __name__ == '__main__':
    unittest.main()

