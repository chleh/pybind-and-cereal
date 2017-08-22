import unittest

import pickle

import types_one

class TestPickle(unittest.TestCase):
    def setUp(self):
        ndc = types_one.NonDefaultConstructible(5.0, 7)
        # help(ndc)
        print("get state:", ndc.__getstate__())
        with open("non_default_constructible.pickle", "wb") as fh:
            pickle.dump(ndc, fh, -1)

    def test_unpickle(self):
        with open("non_default_constructible.pickle", "rb") as fh:
            ndc = pickle.load(fh)
        self.assertEqual(5.0, ndc.a)
        self.assertEqual(7, ndc.b)

if __name__ == '__main__':
    unittest.main()

