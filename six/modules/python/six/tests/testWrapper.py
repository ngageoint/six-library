import unittest

import numpy

from coda.math_poly import Poly1D, Poly2D
from pysix.wrapper import wrap
from pysix.six_sicd import *



class TestWrapping(unittest.TestCase):

    def testCanWrapPrimitives(self):
        originalObject = HalfPowerBeamwidths()
        wrappedObject = wrap(originalObject)
        self.assertEqual(wrappedObject.dcx, originalObject.dcx)
        self.assertEqual(wrappedObject.dcy, originalObject.dcy)

    def testWrappingEmptyPoly1D(self):
        originalObject = ElectricalBoresight()
        wrappedObject = wrap(originalObject)
        self.assertIsInstance(wrappedObject.dcxPoly, type(numpy.zeros(0)))
        self.assertEqual(len(wrappedObject.dcxPoly), 0)

    def testWrappingFilledPoly1D(self):
        originalObject = ElectricalBoresight()
        originalObject.dcxPoly = Poly1D(1)
        originalObject.dcxPoly[0] = 4

        wrappedObject = wrap(originalObject)
        self.assertEqual(len(wrappedObject.dcxPoly), 2)
        self.assertEqual(wrappedObject.dcxPoly[0], 4)

    def testWrappingEmptyPoly2D(self):
        originalObject = GainAndPhasePolys()
        wrappedObject = wrap(originalObject)
        self.assertIsInstance(wrappedObject.gainPoly, type(numpy.zeros(0)))
        self.assertEqual(len(wrappedObject.gainPoly), 0)

    def testCanWrapEmptySmartPointer(self):
        originalObject = AntennaParameters()
        wrappedObject = wrap(originalObject)
        self.assertEqual(wrappedObject.electricalBoresight, None)

    def testWrappingNestedPoly(self):
        originalObject = AntennaParameters()
        originalObject.electricalBoresight = (
                makeScopedCopyableElectricalBoresight())
        originalObject.electricalBoresight.dcxPoly = Poly1D(1)
        originalObject.electricalBoresight.dcxPoly[0] = 5

        wrappedObject = wrap(originalObject)

        self.assertEqual(wrappedObject.electricalBoresight.dcxPoly[0],  5)
        self.assertIsInstance(wrappedObject.electricalBoresight.dcxPoly,
                type(numpy.zeros(0)))

    def testChangeVectorToList(self):
        originalObject = CollectionInformation()
        originalObject.countryCodes.push_back('element')

        wrappedObject = wrap(originalObject)

        self.assertIsInstance(wrappedObject.countryCodes, list)
        self.assertEqual(len(wrappedObject.countryCodes), 1)
        self.assertEqual(wrappedObject.countryCodes[0], 'element')

    def testWrapVectorOfSmartPointers(self):
        originalObject = GeoData()
        originalObject.geoInfos.resize(3)
        originalObject.geoInfos[0] = makeScopedCloneableGeoInfo()
        originalObject.geoInfos[0].name = "first name"
        originalObject.geoInfos[2] = makeScopedCloneableGeoInfo()
        originalObject.geoInfos[2].name = "third name"

        wrappedObject = wrap(originalObject)

        self.assertEqual(len(wrappedObject.geoInfos), 3)
        self.assertEqual(wrappedObject.geoInfos[0].name, "first name")
        self.assertEqual(wrappedObject.geoInfos[1], None)
        self.assertEqual(wrappedObject.geoInfos[2].name, "third name")


if __name__ == '__main__':
    unittest.main()

