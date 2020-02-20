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
import random
import subprocess
import sys
from subprocess import call

import utils

def getSampleSicdXML():
    samples = os.path.join(utils.findSixHome(), 'six', 'modules', 'c++',
                           'six.sicd', 'tests', 'sample_xml')

    return os.path.join(samples, random.choice(os.listdir(samples)))

def runTests(testDir, testName, *args):
    print('Running {}'.format(testName))
    argList = [utils.executableName(os.path.join(testDir, testName))]
    argList.extend(args)
    result = call(argList, stdout=subprocess.PIPE)

    if result == 0:
        print('Passed')
        return True
    print('Failed')
    return False


def runSICDTests():
    # Make sure plugins installed properly
    nitfPluginPath = os.environ['NITF_PLUGIN_PATH_REAL']

    if (not os.path.exists(nitfPluginPath) or
            not any(plugin.startswith('PIAIMB') for plugin in
                    os.listdir(nitfPluginPath))):
        print('Could not find NITF plugins. Please re-install with '
                'the following command.')
        print('python waf install --target=PIAIMB')
        sys.exit(1)
    testDir = os.path.join(utils.installPath(), 'tests', 'six.sicd')

    success = runTests(testDir, 'test_add_additional_des', getSampleSicdXML())
    success = runTests(testDir, 'test_mesh_roundtrip') and success
    success = runTests(testDir, 'test_mesh_polyfit') and success

    # Need it for just this test
    os.environ['NITF_PLUGIN_PATH'] = os.environ['NITF_PLUGIN_PATH_REAL']
    success = runTests(testDir, 'test_read_sicd_with_extra_des',
                       getSampleSicdXML()) and success
    del os.environ['NITF_PLUGIN_PATH']

    return success

def run():
    return runSICDTests()

if __name__ == '__main__':
    run()
