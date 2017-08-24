import unittest
import os

import pickle

import types_one.types_one_a
import pickle_types

class TestPickleSimple(unittest.TestCase):
    def setUp(self):
        self.filename = "non_default_constructible.pickle"
        ndc = types_one.NonDefaultConstructible(5.0, 7)
        # help(ndc)
        with open(self.filename, "wb") as fh:
            pickle.dump(ndc, fh, -1)

    def test_unpickle(self):
        with open(self.filename, "rb") as fh:
            ndc = pickle.load(fh)
        os.unlink(self.filename)

        self.assertEqual(5.0, ndc.a)
        self.assertEqual(7, ndc.b)


class TestPickleNoncopyableInstance(unittest.TestCase):
    def setUp(self):
        self.filename = "nocopy.pickle"
        nc = types_one.types_one_a.NoCopy()
        nc.i = 5
        nc.s = "Hello!"

        with open(self.filename, "wb") as fh:
            pickle.dump(nc, fh, -1)

    def test_unpickle(self):
        with open(self.filename, "rb") as fh:
            nc = pickle.load(fh)
        os.unlink(self.filename)

        self.assertEqual(5, nc.i)
        self.assertEqual("Hello!", nc.s)


class TestPickleEmptyBaseClass(unittest.TestCase):
    def setUp(self):
        self.filename = "empty_base.pickle"
        print("-----> ", dir(pickle_types))
        a = pickle_types.DerivedFromEmptyInt()
        a.i = 5
        b = pickle_types.DerivedFromEmptyString()
        b.s = "Hello!"

        with open(self.filename, "wb") as fh:
            pickle.dump((a, b), fh, -1)

    def test_unpickle(self):
        with open(self.filename, "rb") as fh:
            a, b = pickle.load(fh)
        os.unlink(self.filename)

        self.assertEqual(5, a.i)
        self.assertEqual("Hello!", b.s)



if __name__ == '__main__':
    unittest.main()

