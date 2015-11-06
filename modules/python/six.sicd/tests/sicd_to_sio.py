import numpy as np
import sys
import os
from pysix import six_base, six_sicd
from coda import sio_lite

if __name__ == '__main__':
    if len(sys.argv) >= 3:
        # Set up
        nitfPath = os.path.abspath(sys.argv[1])
        sioPath = nitfPath.rstrip(".nitf") + ".sio"
        schemaPath = os.path.abspath(sys.argv[2])
    else:
        print "Usage: python " + sys.argv[0] + " <NITF path> <Schema path>"
        sys.exit(0)

    schemaPaths = six_base.VectorString()
    schemaPaths.push_back(schemaPath)
    widebandData, complexData = six_sicd.read(nitfPath, schemaPaths)
    sio_lite.write(widebandData, sioPath)

    print "Wrote " + sioPath
