import unittest

from coda import coda_types
from coda.coda_types import pickle

class TestCodaTypes(unittest.TestCase):

    def test_RowCol_pickle(self):
        x = coda_types.RowColDouble(1.2, 3.4)
        y = pickle.loads(pickle.dumps(x))
        self.assertEqual(x, y)

        x = coda_types.RowColInt(1, 2)
        y = pickle.loads(pickle.dumps(x))
        self.assertEqual(x, y)

        x = coda_types.RowColSizeT(1, 2)
        y = pickle.loads(pickle.dumps(x))
        self.assertEqual(x, y)

    def test_RgAz_pickle(self):
        x = coda_types.RgAzDouble(1.2, 3.4)
        y = pickle.loads(pickle.dumps(x))
        self.assertEqual(x, y)

    def test_vector_pickle(self):
        x = coda_types.VectorSizeT(tuple(range(10)))
        y = pickle.loads(pickle.dumps(x))
        self.assertEqual(tuple(x), tuple(y))

        x = coda_types.VectorRowColInt(tuple(coda_types.RowColInt(i+1,i+2)
                                             for i in range(10)))
        y = pickle.loads(pickle.dumps(x))
        self.assertEqual(tuple(x), tuple(y))


if __name__ == '__main__':
    unittest.main()
