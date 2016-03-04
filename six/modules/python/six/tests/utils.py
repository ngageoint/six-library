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
import platform
import sys

from subprocess import call


class Pathfinder(object):

    def __init__(self, installDir='install'):
        self.installDir = installDir
        
    def findSixHome(self):
        if 'WORKSPACE' in os.environ:
            return os.environ['WORKSPACE']
        
        return os.getcwd()

    def installPath(self):
        if 'WORKSPACE' in os.environ:
            return self.installDir
        return os.path.join(self.findSixHome(), self.installDir)

    def setPaths(self):
        nitfPluginPath = os.path.join(self.installPath(), 'share', 'nitf',
                                  'plugins')
        sixSchemaPath = os.path.join(self.installPath(), 'conf', 'schema',
                                 'six')

        pythonPath = os.path.join(self.installPath(), 'lib', 'python2.7',
                                      'site-packages')
        if platform.system() == 'Windows':
            pythonPath = os.path.join(self.installPath(), 'lib',
                                  'site-packages')
        
        os.environ['NITF_PLUGIN_PATH'] = nitfPluginPath
        os.environ['SIX_SCHEMA_PATH'] = sixSchemaPath

        # We set the pythonpath with sys.path so it's set for the current script
        # and with os.environ so it's set for child processes
        sys.path.append(pythonPath)
        os.environ['PYTHONPATH'] = pythonPath

def executableName(pathname):

    if platform.system() == 'Windows':
        return pathname + '.exe'
    if pathname.startswith('/'):
        return '.' + pathname
    return './' + pathname
