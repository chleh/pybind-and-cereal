import unittest
import types_one


class Derived(types_one.Base):
    def what(self):
        return "derived in python"


class TestInheritance(unittest.TestCase):
    def setUp(self):
        pass

    def test_overridden_method(self):
        d = Derived()
        self.assertEqual("derived in python", types_one.say_what(d))

if __name__ == '__main__':
    unittest.main()

