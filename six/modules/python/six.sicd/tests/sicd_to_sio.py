import numpy as np
import sys
import os
from pysix import six_base, six_sicd, sicdUtils
from coda import sio_lite

if __name__ == '__main__':
    if len(sys.argv) >= 3:
        # Set up
        nitfPath = os.path.abspath(sys.argv[1])
        sioPath = nitfPath.rstrip(".nitf") + ".sio"
        schemaPath = os.path.abspath(sys.argv[2])
    else:
        print("Usage: python " + sys.argv[0] + " <NITF path> <Schema path> [startRow numRows startCol numCols]")
        sys.exit(0)

    schemaPaths = [schemaPath]

    if len(sys.argv) == 7:
        sfl = int(sys.argv[3])
        snl = int(sys.argv[4])
        sfe = int(sys.argv[5])
        sne = int(sys.argv[6])
        widebandData, complexData = sicdUtils.readNITF(nitfPath, sfl, snl, sfe,
                sne, schemaPaths)
    else:
        widebandData, complexData = sicdUtils.readNITF(nitfPath,
                schemaPaths=schemaPaths)

    sio_lite.write(widebandData, sioPath)

    print("Wrote " + sioPath)

