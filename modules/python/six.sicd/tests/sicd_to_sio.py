import numpy as np
import sys
import os
import six_sicd
import sio_lite

if __name__ == '__main__':
    if len(sys.argv) >= 3:
        # Set up
        nitfPath = os.path.abspath(sys.argv[1])
        sioPath = nitfPath.rstrip(".nitf") + ".sio"
        schemaPath = os.path.abspath(sys.argv[2])
    else:
        print "Usage: python " + sys.argv[0] + " <NITF path> <Schema path>"
        sys.exit(0)

    widebandData, complexData = six_sicd.read(nitfPath)
    sio_lite.write(widebandData, sioPath)

    print "Wrote " + sioPath
