import unittest
import types_one.types_one_b

class TestUniquePtr(unittest.TestCase):
    def test_unique_ptr_return(self):
        d3 = types_one.types_one_b.all_types["Derived3<int, int>"]();
        res = d3.f()

        self.assertIs(types_one.types_one_b.all_types["Derived3<int, int>"], type(res))
        # TODO revert to the old code below?
        # self.assertIs(tp.Derived1, type(res))

if __name__ == '__main__':
    unittest.main()

