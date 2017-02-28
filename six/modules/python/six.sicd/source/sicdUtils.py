'''
 * =========================================================================
 * This file is part of six.sicd-python
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * six.sicd-python is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
'''

import os
from coda.coda_io import FileOutputStream
from coda.xml_lite import *
from pysix.six_sicd import *


def writeXML(complexData, pathname):
    ''' Write complexData to pathname as XML. '''
    schemaPaths = VectorString()
    schemaPaths.push_back( os.environ['SIX_SCHEMA_PATH'] )
    xmlControl = ComplexXMLControl()
    outputStream = FileOutputStream(pathname)
    try:
        xml = xmlControl.toXML(complexData, schemaPaths)
        root = xml.getRootElement()
        root.prettyPrint(outputStream)
    finally:
        outputStream.close()

def readNITF(inputPathname, startRow=0, numRows=-1, startCol=0, numCols=-1,
        schemaPaths=VectorString()):

    complexData = SixSicdUtilities.getComplexData(inputPathname, schemaPaths)

    if numRows == -1:
        numRows = complexData.getNumRows()
    if numCols == -1:
        numCols = complexData.getNumCols()

    widebandData = np.empty(shape = (numRows, numCols), dtype='complex64')
    widebandBuffer, _ = widebandData.__array_interface__['data']

    getWidebandRegion(inputPathname, schemaPaths, complexData, startRow,
            numRows, startCol, numCols, widebandBuffer)

    return widebandData, complexData


