import unittest
import bindings as tp

class TestUniquePtr(unittest.TestCase):
    def test_unique_ptr_return(self):
        d3 = tp.all_types["Derived3<int, int>"]();
        res = d3.f()
        self.assertIs(tp.Derived1, type(res))

if __name__ == '__main__':
    unittest.main()

