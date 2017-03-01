import unittest

import numpy

from coda.math_poly import Poly1D, Poly2D
from pysix.wrapper import wrap, unwrap
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


class TestUnwrapping(unittest.TestCase):

    def testCanUnwrapPrimitives(self):
        originalObject = HalfPowerBeamwidths()
        wrappedObject = wrap(originalObject)
        wrappedObject.dcx = 5.0
        wrappedObject.dcy = 3.4

        unwrap(wrappedObject, originalObject)

        self.assertEqual(originalObject.dcx, 5)
        self.assertEqual(originalObject.dcy, 3.4)

    def testUnwrappingPoly1D(self):
        originalObject = ElectricalBoresight()
        originalObject.dcxPoly = Poly1D(1)
        originalObject.dcxPoly[0] = 3
        wrappedObject = wrap(originalObject)
        wrappedObject.dcxPoly = numpy.zeros(0)
        wrappedObject.dcyPoly = numpy.ones(1)

        unwrap(wrappedObject, originalObject)

        self.assertTrue(originalObject.dcxPoly.empty())
        self.assertEqual(originalObject.dcyPoly[0], 1)
        self.assertIsInstance(originalObject.dcxPoly, Poly1D)

    def testUnwrappingPoly2D(self):
        originalObject = GainAndPhasePolys()
        originalObject.gainPoly = Poly2D(1, 1)
        wrappedObject = wrap(originalObject)
        wrappedObject.gainPoly = numpy.zeros(0)
        wrappedObject.phasePoly = numpy.ones((1, 1))

        unwrap(wrappedObject, originalObject)

        self.assertIsInstance(originalObject.gainPoly, Poly2D)
        self.assertTrue(originalObject.gainPoly.empty())
        self.assertIsInstance(originalObject.phasePoly, Poly2D)
        self.assertEqual(originalObject.phasePoly[(0,0)], 1)

    def testCanUnwrapSmartPointer(self):
        originalObject = AntennaParameters()
        originalObject.electricalBoresight = (
                makeScopedCopyableElectricalBoresight())
        wrappedObject = wrap(originalObject)
        wrappedObject.electricalBoresight = None
        wrappedObject.halfPowerBeamwidths = (
                makeScopedCopyableHalfPowerBeamwidths())
        wrappedObject.halfPowerBeamwidths.dcx = 3

        unwrap(wrappedObject, originalObject)

        self.assertIsNone(originalObject.electricalBoresight.get())
        self.assertIsNotNone(originalObject.halfPowerBeamwidths.get())
        self.assertEqual(originalObject.halfPowerBeamwidths.dcx, 3)

    def testUnwrapList(self):
        originalObject = CollectionInformation()
        originalObject.countryCodes.push_back('element')
        wrappedObject = wrap(originalObject)
        wrappedObject.countryCodes.append('secondElement')

        unwrap(wrappedObject, originalObject)

        self.assertIn('Vector', type(originalObject.countryCodes).__name__)
        self.assertEqual(originalObject.countryCodes[1], 'secondElement')

    def testUnwrapVectorOfSmartPointers(self):
        originalObject = GeoData()
        originalObject.geoInfos.resize(3)
        originalObject.geoInfos[0] = makeScopedCloneableGeoInfo()
        originalObject.geoInfos[0].name = "first name"
        originalObject.geoInfos[2] = makeScopedCloneableGeoInfo()
        originalObject.geoInfos[2].name = "third name"
        wrappedObject = wrap(originalObject)
        wrappedObject.geoInfos[2] = None
        wrappedObject.geoInfos[1] = makeScopedCloneableGeoInfo()
        wrappedObject.geoInfos[1].name = "second name"

        unwrap(wrappedObject, originalObject)

        self.assertEqual(originalObject.geoInfos.size(), 3)
        self.assertEqual(wrappedObject.geoInfos[1].name, "second name")
        self.assertIsNone(wrappedObject.geoInfos[2])

if __name__ == '__main__':
    unittest.main()

