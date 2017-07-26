import unittest
import types_one.types_one_c


class Derived(types_one.Base):
    def what(self):
        return "derived in python"

# non-default-constructible base and trampoline class
class Derived2(types_one.types_one_c.Base):
    def __init__(self, x):
        # this is necessary, otherwise seg fault!
        super(Derived2, self).__init__(x)

    def what(self):
        return "also derived in python"


class TestInheritance(unittest.TestCase):
    def setUp(self):
        pass

    def test_overridden_method(self):
        d = Derived()
        self.assertEqual("derived in python", types_one.say_what(d))

    def test_overridden_method2(self):
        d = Derived2("")
        self.assertEqual("also derived in python", types_one.types_one_c.say_what(d))

if __name__ == '__main__':
    unittest.main()

