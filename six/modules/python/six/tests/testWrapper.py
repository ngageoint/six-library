import unittest
from pysix.wrapper import wrap

class PrimitiveWrapper(object):
    def __init__(self):
        self.string = "member"
        self.integer = 3
        self.double = 3.14

    def toString(self):
        return self.string + '|' + str(self.integer) + '|' + str(self.double)

    def setString(self, val):
        self.string = val

class TestWrapping(unittest.TestCase):

    def testCanWrapPrimitives(self):
        originalObject = PrimitiveWrapper()
        wrappedObject = wrap(originalObject)
        self.assertEqual(wrappedObject.string, originalObject.string)
        self.assertEqual(wrappedObject.integer, originalObject.integer)
        self.assertEqual(wrappedObject.double, originalObject.double)

    def testCanUnwrapPrimitives(self):
        originalObject = PrimitiveWrapper()
        wrappedObject = wrap(originalObject)
        wrappedObject.string = "new member"
        self.assertIn("new member", wrappedObject.toString())
        self.assertEqual(wrappedObject.toString(), originalObject.toString())

    def testSideEffectsAreApplied(self):
        originalObject = PrimitiveWrapper()
        wrappedObject = wrap(originalObject)
        wrappedObject.setString("new member")
        self.assertEqual(wrappedObject.string, "new member")


if __name__ == '__main__':
    unittest.main()

