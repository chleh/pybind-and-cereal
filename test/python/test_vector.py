import unittest
import bindings as tp

class TestDerived1(unittest.TestCase):
    def setUp(self):
        self.vt = tp.VectorTest()

    def test_move_assign(self):
        tmp = tp.all_types["std::vector<int, std::allocator<int> >"]()
        self.vt.a__MOVE_IN = tmp
        # TODO: move/copy held object if held class is copyable/movable?

    def test_append(self):
        self.vt.b.append(1)
        self.assertEqual(1, len(self.vt.b))
        self.assertEqual(1, self.vt.b[0])

    def test_vector_return(self):
        tmp = self.vt.get()
        self.assertEqual(1, len(tmp))
        self.assertEqual(8, tmp[0])

    def test_vector_arg(self):
        tmp = tp.all_types["std::vector<int, std::allocator<int> >"]([1,2,3])
        self.vt.set(tmp)

        tmp = tp.all_types["std::vector<short, std::allocator<short> >"]([1,2,3])
        self.vt.set_ref(tmp)
        self.assertEqual(7, tmp[0])

if __name__ == '__main__':
    unittest.main()
