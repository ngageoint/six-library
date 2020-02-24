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

import glob
import os
import re
import utils

from subprocess import call

binDir = os.path.join(utils.installPath(), 'bin')

def extractVersionNumber(pathname):
    versionPattern = '^\d\.\d\.\d$'

    candidateVersions = os.path.normpath(pathname).split(os.path.sep)
    for candidate in candidateVersions:
        if re.match(versionPattern, candidate):
            return candidate

    raise Exception('Unable to find version in path ' + pathanem)


def roundTrippedName(pathname):
    return os.path.basename(pathname.replace('.nitf', '_rt.nitf'))

def roundTripSix(pathname):
    return call([utils.executableName(os.path.join(binDir, 'round_trip_six')),
              '--version', extractVersionNumber(pathname),
              pathname,
              roundTrippedName(pathname)]) == 0

def validate(pathname):
    check_valid_six = utils.executableName(os.path.join(binDir,
            'check_valid_six'))

    return (call([check_valid_six, pathname],
                stdout=open(os.devnull, 'w')) == 0 and
            call([check_valid_six, roundTrippedName(pathname)],
                stdout=open(os.devnull, 'w'))) == 0


def run():
    regressionDir = os.path.join(utils.findSixHome(), 'regression_files')
    result = True
    for pathname in glob.iglob(os.path.join(regressionDir, '*', '*', '*.nitf')):
        print('Checking {0}'.format(os.path.basename(pathname)))
        result = result and roundTripSix(pathname) and validate(pathname)
        if result == False:
            print('Failed {0}'.format(os.path.basename(pathname)))
            break

    # Clean up
    for pathname in glob.iglob(os.path.join(os.getcwd(), '*.nitf')):
        os.remove(pathname)

    return result

if __name__ == '__main__':
    run()
