#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2016, MDA Information Systems LLC
#
# six.sidd-python is free software; you can redistribute it and/or modify
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

import filecmp
import os
import platform
import sys
from itertools import product
from subprocess import call

def findTestsDir():
    # Backtrack to six-library dir, in case this gets called
    # from tests/ or somewhere weird.
    while os.path.basename(os.getcwd()) != 'six-library':
        if os.path.dirname(os.getcwd()) == os.getcwd():
            # We'rve probably made it to / if this happens, so the user
            # is calling from somewhere nonsensical or unexpected
            raise Exception("Please run from home directory of six-library")
    
    test_path = os.path.join(os.getcwd(), 'install', 'bin')

    if not os.path.exists(test_path):
        raise IOError('Could not find test directory {0}'.format(test_path))

    return test_path

def getExecutableName(basename):
    if platform.system() == 'Windows':
        basename += '.exe'
    else:
        basename = './' + basename
    return basename

def createNITF(testsDir, args, filename):
    command = getExecutableName(os.path.join(testsDir, 'test_create_sidd_from_mem'))
    code = call([command] + args + [filename])
    return code

def extractXML(testsDir, filename):
    command = getExecutableName(os.path.join(testsDir, 'test_extract_xml'))
    return call([command, filename])

def roundTrip(testsDir, inFile, outFile):
    command = getExecutableName(os.path.join(testsDir, 'round_trip_six'))
    if call([command, '--retainDateTime', inFile, outFile]) != 0:
        return 1
    if extractXML(testsDir, inFile) != 0 or extractXML(testsDir, outFile) != 0:
        raise Exception("Could not extract XML")
    for (original, roundTripped) in zip(createXMLName(inFile), createXMLName(outFile)):
        if filecmp.cmp(original, roundTripped) == False:
            return 1
    return 0

def validate(testsDir, filename):
    command = getExecutableName(os.path.join(testsDir, 'check_valid_six'))
    return call([command, filename])

def createFilenameFromArgs(args):
    base = 'output'
    if 'Color' in args:
        base += '_C'
    elif 'Mono' in args:
        base += '_M'
    if '--smallImage' in args:
        base += '_smI'
    elif '--multipleSegments' in args:
        base += '_mS'
    if '--multipleImages' in args:
        base += '_mIs'
    base += '.nitf'
    return base

def createXMLName(filename):
    base = filename[:-1 * len('.nitf')]
    ret = []
    numImages = 3 if '_mI' in filename else 1
    for idx in range(numImages):
        xmlName = base + '-XML_DATA_CONTENT*.xml'
        ret.append(xmlName.replace('*', str(idx)))
    return ret

def createRoundTrippedName(filename):
    extension = filename.split('.')[-1]
    base = filename[:-1 * (len(extension) + 1)]
    return base + '_rt.{0}'.format(extension)

def clean(filename):
    try:
        os.remove(filename)
        os.remove(createRoundTrippedName(filename))
        for xmlFile in createXMLName(filename) + createXMLName(createRoundTrippedName(filename)):
            os.remove(xmlFile)
    except OSError:
        pass
    
    
if __name__ == '__main__':
    shouldClean = '--clean' in sys.argv
    lutOptions = ['Color', 'Mono', 'None']
    exclusiveOptions = ['--smallImage', '--multipleSegments', '']
    testsDir = findTestsDir()

    for args in product(['--lut'], lutOptions, exclusiveOptions, ['--multipleImages', '']):
        args = [arg for arg in args if arg != '']
        filename = createFilenameFromArgs(args)
        if createNITF(testsDir, list(args), filename) != 0:
            sys.exit('Unable to make .nitf with specification {0}'.format(args))
        if roundTrip(testsDir, filename, createRoundTrippedName(filename)) != 0:
            sys.exit('Error round-tripping file {0}'.format(filename))
        if validate(testsDir, filename) != 0:
            sys.exit('{0} appears to be invalid.'.format(filename))
        if shouldClean:
            clean(filename)

    sys.exit(0)
