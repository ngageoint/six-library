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

def createNITF(version='1.1.0'):
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
        raise Exception("Please call test from six-library/ or "
                "six-library/six/modules/python/six.sicd/tests")

    return call(['python', test_file, '--includeNITF',
          '--version={0}'.format(version)])

def clean(nitfName, thorough=False):
    xmlName = nitfName.replace('nitf', 'xml')
    os.remove(xmlName)
    os.remove(xmlName.replace('.xml', '_rt.xml'))
    if thorough:
        os.remove(nitfName)
        os.remove(nitfName.replace('.nitf', '_rt.nitf'))

if __name__ == '__main__':
    import sys
    sicdVersions = ['0.4.0', '0.4.1', '0.5.0', '1.0.0', '1.0.1', '1.1.0']

    successCode = 0
    for version in sicdVersions:
        print "Testing version {0}:".format(version)
        if createNITF(version) == False:
            successCode = 1

        outputName = "test_create_sicd_{0}.nitf".format(version)
        # The output from the script createNITF runs should give us
        # the desired testing output
        if len(sys.argv) > 1 and sys.argv[1] == '--clean':
            clean(createNITF(version), thorough=True)
        else:
            clean(createNITF(version))
    sys.exit(successCode)
