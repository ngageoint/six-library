import unittest

import numpy

from coda.math_poly import Poly1D, Poly2D
from pysix.wrapper import wrap, unwrap
from pysix.six_sicd import *
from pysix.six_base import *

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

        unwrapped = unwrap(wrappedObject)

        self.assertEqual(unwrapped.dcx, 5)
        self.assertEqual(unwrapped.dcy, 3.4)

    def testUnwrappingPoly1D(self):
        originalObject = ElectricalBoresight()
        originalObject.dcxPoly = Poly1D(1)
        originalObject.dcxPoly[0] = 3
        wrappedObject = wrap(originalObject)
        wrappedObject.dcxPoly = numpy.zeros(0)
        wrappedObject.dcyPoly = numpy.ones(1)

        unwrapped = unwrap(wrappedObject)

        self.assertTrue(unwrapped.dcxPoly.empty())
        self.assertEqual(unwrapped.dcyPoly[0], 1)
        self.assertIsInstance(unwrapped.dcxPoly, Poly1D)

    def testUnwrappingPoly2D(self):
        originalObject = GainAndPhasePolys()
        originalObject.gainPoly = Poly2D(1, 1)
        wrappedObject = wrap(originalObject)
        wrappedObject.gainPoly = numpy.zeros(0)
        wrappedObject.phasePoly = numpy.ones((1, 1))

        unwrapped = unwrap(wrappedObject)

        self.assertIsInstance(unwrapped.gainPoly, Poly2D)
        self.assertTrue(unwrapped.gainPoly.empty())
        self.assertIsInstance(unwrapped.phasePoly, Poly2D)
        self.assertEqual(unwrapped.phasePoly[(0,0)], 1)

    def testCanUnwrapSmartPointer(self):
        originalObject = AntennaParameters()
        originalObject.electricalBoresight = (
                makeScopedCopyableElectricalBoresight())
        wrappedObject = wrap(originalObject)
        wrappedObject.electricalBoresight = None
        wrappedObject.halfPowerBeamwidths = (
                makeScopedCopyableHalfPowerBeamwidths())
        wrappedObject.halfPowerBeamwidths.dcx = 3

        unwrapped = unwrap(wrappedObject)

        self.assertIsNone(unwrapped.electricalBoresight.get())
        self.assertIsNotNone(unwrapped.halfPowerBeamwidths.get())
        self.assertEqual(unwrapped.halfPowerBeamwidths.dcx, 3)

    def testUnwrapList(self):
        originalObject = CollectionInformation()
        originalObject.countryCodes.push_back('element')
        wrappedObject = wrap(originalObject)
        wrappedObject.countryCodes.append('secondElement')

        unwrapped = unwrap(wrappedObject)

        self.assertIn('Vector', type(unwrapped.countryCodes).__name__)
        self.assertEqual(unwrapped.countryCodes[1], 'secondElement')

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

        unwrapped = unwrap(wrappedObject)

        self.assertEqual(unwrapped.geoInfos.size(), 3)
        self.assertIsNotNone(unwrapped.geoInfos[1].get())
        self.assertEqual(unwrapped.geoInfos[1].name, "second name")
        self.assertIsNone(unwrapped.geoInfos[2].get())


class TestMethods(unittest.TestCase):

    def testMethodsUseUpdatedValues(self):
        originalObject = ComplexData()
        originalObject.imageData.numRows = 3
        wrappedObject = wrap(originalObject)
        wrappedObject.imageData.numRows = 5
        self.assertEqual(wrappedObject.getNumRows(), 5)

    def testMethodsCanChangeWrapper(self):
        originalObject = ComplexData()
        wrappedObject = wrap(originalObject)
        wrappedObject.setNumRows(5)
        self.assertEqual(wrappedObject.getNumRows(), 5)

    def testMethodsDeep(self):
        originalObject = ComplexData()
        wrappedObject = wrap(ComplexData())
        wrappedObject.fillDefaultFields()
        originalObject.fillDefaultFields()
        unwrapped = unwrap(wrappedObject)
        self.assertEqual(originalObject, unwrapped)


if __name__ == '__main__':
    unittest.main()

