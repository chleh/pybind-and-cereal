import unittest
import types_one.types_one_b

class TestMethodArgsReturns(unittest.TestCase):
    def test_unique_ptr_return(self):
        d3 = types_one.types_one_b.all_types["Derived3<int, int>"]();
        res = d3.f()

        self.assertIs(types_one.types_one_b.all_types["Derived3<int, int>"], type(res))
        # TODO revert to the old code below?
        # self.assertIs(tp.Derived1, type(res))

    def test_nocopy_arg(self):
        d3 = types_one.types_one_b.all_types["Derived3<int, int>"]();
        nc = types_one.types_one_a.NoCopy()
        nc.i = 42

        res = d3.nocopy_method_arg(nc)
        self.assertEqual(nc.i, res)

    def test_unique_ptr_ref_arg(self):
        d3 = types_one.types_one_b.all_types["Derived3<int, int>"]();
        nc = types_one.types_one_a.NoCopy()
        nc.i = 42

        res = d3.get_int_from_unique_ptr(nc)
        self.assertEqual(2*nc.i, res)

    def test_unique_ptr_misc_args(self):
        nc = types_one.types_one_a.NoCopy()
        nc.i = 42

        ref = types_one.types_one_b.move_to_unique_ptr(nc)

        self.assertEqual(2*nc.i, types_one.types_one_b.i_up_cr(ref))
        self.assertEqual(2*nc.i, types_one.types_one_b.i_up_r(ref))
        self.assertEqual(2*nc.i, types_one.types_one_b.i_up_rr(ref))
        self.assertEqual(2*nc.i, types_one.types_one_b.i_up(ref))

if __name__ == '__main__':
    unittest.main()

