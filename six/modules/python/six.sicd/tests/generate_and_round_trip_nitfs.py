#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

def createNITF(version='1.1.0', alg='PFA', imageType=''):
    sicdVersions = ['0.4.0', '0.4.1', '0.5.0', '1.0.0', '1.0.1', '1.1.0']
    assert version in sicdVersions, "Invalid SICD versions"

    # Find regression file. If this is called from the base six-library
    # dir, we know exactly where it is.
    test_file_name = 'test_create_sicd_xml.py'
    test_file = None
    if os.path.basename(os.getcwd()) == "six-library":
        test_file = os.path.join(os.getcwd(), 'six', 'modules', 'python',
                'six.sicd', 'tests', 'test_create_sicd_xml.py')

    # Otherwise, let's assume the file is in the current directory,
    # or in a dir on the PYTHONPATH
    elif test_file_name in os.listdir('.'):
        test_file = test_file_name

    else:
        raise Exception("Please call test from six-library/")

    return call(['python', test_file, '--includeNITF',
          '--version={0}'.format(version), '--alg={0}'.format(alg),
          '--imageType={0}'.format(imageType)])


def clean(baseName):
    xmlName = baseName + '.xml'
    nitfName = baseName + '.nitf'
    os.remove(xmlName)
    os.remove(xmlName.replace('.xml', '_rt.xml'))
    os.remove(nitfName)
    os.remove(nitfName.replace('.nitf', '_rt.nitf'))

def setPythonPath():
    try:
        import pysix.scene
    except ImportError:
        import platform
        if platform.system() == 'Windows':
            os.environ['PYTHONPATH'] += os.path.join(
                os.getcwd(), "install", "lib", "site-packages")
        elif platform.system() == 'Linux':
            os.environ['PYTHONPATH'] += os.path.join(
                os.getcwd(), "install", "lib", "python2.7", "site-packages")
                                                     
def test_and_clean(version, alg, imageType=''):
    message = "Testing version {0} with {1} algorithm".format(version, alg)
    if imageType != '' :
        message += " and image type {0}".format(imageType)
    print message

    successCode = createNITF(version, alg, imageType)
    if successCode == 2:
        print "Skipping test with invalid arguments"
        return successCode
    elif successCode == 0:
        outputName = "test_create_sicd_{0}({1}){2}".format(version, alg, imageType)
        clean(outputName)
    else:
        print "Test failed"

if __name__ == '__main__':
    setPythonPath()
    import sys
    sicdVersions = ['0.4.0', '0.4.1', '0.5.0', '1.0.0', '1.0.1', '1.1.0']
    formationAlgs = ['PFA', 'RMA', 'RGAZCOMP']
    imageTypes = ['RMAT', 'RMCR', 'INCA']

    successCode = 0
    for version in sicdVersions:
        for alg in formationAlgs:
            if alg == 'RMA':
                for imageType in imageTypes:
                    code = test_and_clean(version, alg, imageType)
                    if code == 1:
                        successCode = 1
            else:
                code = test_and_clean(version, alg)
                if code == 1:
                    successCode = 1
                
    sys.exit(successCode)
