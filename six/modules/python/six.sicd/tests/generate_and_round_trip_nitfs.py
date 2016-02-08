#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2016, MDA Information Systems LLC
#
# six.sicd-python is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; If not,
# see <http://www.gnu.org/licenses/>.
#

import os
from subprocess import call

def clean(baseName):
    xmlName = baseName + '.xml'
    nitfName = baseName + '.nitf'
    os.remove(xmlName)
    os.remove(xmlName.replace('.xml', '_rt.xml'))
    os.remove(nitfName)
    os.remove(nitfName.replace('.nitf', '_rt.nitf'))

def checkArgs(version, alg, imageType):
    if imageType != '' and alg != 'RMA':
        return False
    if alg == 'RGAZCOMP' and not version.startswith('1'):
        return False
    if imageType == 'RMCR' and not version.startswith('1'):
        return False
    return True
    
def test_and_clean(version, alg, imageType=''):
    from test_create_sicd_xml import initData, doRoundTrip
    if not checkArgs(version, alg, imageType):
        return 2
    
    message = "Testing version {0} with {1} algorithm".format(version, alg)
    if imageType != '' :
        message += " and image type {0}".format(imageType)
    print message

    outputName = "test_create_sicd_{0}({1}){2}".format(version, alg, imageType)
    successCode = doRoundTrip(initData(includeNITF=True, version=version,
                                       alg=alg, imageType=imageType),
                              includeNITF=True, outputFilename=outputName) 
        
    if successCode == 0:
        clean(outputName)
    else:
        print "Test failed"

def checkPythonPath():
    import platform
    try:
        import pysix.scene
    except ImportError:
        pythonpath = ''
        if platform.system() == 'Windows':
            pythonpath = os.path.join('install', 'lib', 'site-packages')
        else:
            pythonpath = os.path.join('install', 'lib', 'python-2.7',
                                      'site-packages')
        raise ImportError('Please set PYTHONPATH to six-library{0}{1}'.format(
                os.sep, pythonpath))
    
if __name__ == '__main__':
    from itertools import product
    import sys

    checkPythonPath()

    sicdVersions = ['0.4.0', '0.4.1', '0.5.0', '1.0.0', '1.0.1', '1.1.0']
    formationAlgs = ['PFA', 'RMA', 'RGAZCOMP']
    imageTypes = ['RMAT', 'RMCR', 'INCA']

    successCode = 0
    for args in product(sicdVersions, formationAlgs, imageTypes):
        code = test_and_clean(args[0], args[1], args[2])
        if code == 1:
            successCode = 1

                
    sys.exit(successCode)
