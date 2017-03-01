import sys

from pysix.sicdUtils import readComplexData, writeXML
from pysix.wrapper import wrap, unwrap

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: ' + sys.argv[0] + ' <NITF or XML pathname> ' +
                '<roundTripped XML pathname')
        sys.exit(1)

    inputPathname = sys.argv[1]
    outputPathname = sys.argv[2]
    sourceData = readComplexData(inputPathname)
    wrappedData = wrap(sourceData)
    unwrappedData = unwrap(wrappedData, sourceData.clone())
    writeXML(unwrappedData, outputPathname)
    roundTrippedData = readComplexData(outputPathname)
    assert(sourceData == roundTrippedData)
    sys.exit(0)

