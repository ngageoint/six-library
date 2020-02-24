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
from coda.coda_types import VectorString
from coda.xml_lite import *
from pysix.six_sicd import *


def writeXML(complexData, pathname):
    ''' Write complexData to pathname as XML. '''
    schemaPaths = VectorString()
    schemaPaths.push_back( os.environ['SIX_SCHEMA_PATH'] )
    xmlControl = ComplexXMLControl()
    print('pathname: {}'.format(pathname))
    print(type(pathname))
    outputStream = FileOutputStream(pathname)
    try:
        xml = xmlControl.toXML(complexData, schemaPaths)
        root = xml.getRootElement()
        root.prettyPrint(outputStream)
    finally:
        outputStream.close()

