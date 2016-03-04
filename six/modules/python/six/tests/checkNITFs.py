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
import utils

from subprocess import call

class NITFChecker(object):

    def __init__(self, pathfinder, path):
        self.binDir = os.path.join(pathfinder.installPath(), 'bin')
        self.path = path
        self.outPath = self.roundTrippedName()

    def roundTrippedName(self):
        return os.path.basename(self.path.replace('.nitf', '_rt.nitf'))

    def roundTripSix(self):
        return call([utils.executableName(os.path.join(
            self.binDir,'round_trip_six')), self.path, self.outPath]) == 0
    
    def validate(self):
        check_valid_six = utils.executableName(os.path.join(self.binDir,
                'check_valid_six'))
    
        return (call([check_valid_six, self.path]) == 0 and call(
            [check_valid_six, self.outPath])) == 0

    def clean(self):
        os.remove(self.outPath)

    def run(self):
        if not self.roundTripSix():
            return False
        result = self.validate()
        
        if result == True:
            self.clean()
        return result

        
def run(pathfinder):
    regressionDir = os.path.join(pathfinder.findSixHome(), 'regression_files')
    result = True
    for pathname in glob.iglob(os.path.join(regressionDir, '*', '*.nitf')):
        checker = NITFChecker(pathfinder, pathname)
        result = result and checker.run()

    return result
