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
import shutil
import sys

from glob import glob
from subprocess import call

def findSixHome():
    if 'WORKSPACE' in os.environ:
        return os.environ['WORKSPACE']

    currentPath = os.getcwd()
    while os.path.basename(currentPath) != 'six-library':
        parent = os.path.abspath(os.path.join(currentPath, os.pardir))
        if parent == currentPath:
            raise IOError(os.getcwd())
            #raise IOError('Please run this program from within six-library dir')
        currentPath = parent

    return currentPath


def installPath():
    home = findSixHome()
    children = ['remove_foss.csh', 'README.md', 'six', 'wscript',
                'sync_externals.csh', 'externals', '.git', 'processFiles.py',
                'docs', 'waf', '.gitignore', 'LICENSE']

    for child in os.listdir(home):
        fullChildPath = os.path.join(home, child)
        if child not in children and os.path.isdir(fullChildPath):
            subdirs = os.listdir(fullChildPath)
            if 'tests' in subdirs and 'bin' in subdirs:
                return fullChildPath


def findPythonPath():
    if platform.system() == 'Linux':
        return glob(os.path.join(installPath(), 'lib', 'python*',
                                      'site-packages'))[0]
    elif platform.system() == 'Windows':
        return os.path.join(installPath(), 'lib',
                                'site-packages')
    return ''


def ensureInitFiles(pythonPath):
    for child in os.listdir(pythonPath):
        childPath = os.path.join(pythonPath, child)
        if '__init__.py' not in os.listdir(childPath):
            open(os.path.join(childPath, '__init__.py'), 'w').close()



def setPaths():
    nitfPluginPath = os.path.join(installPath(), 'share', 'nitf',
                                  'plugins')
    sixSchemaPath = os.path.join(installPath(), 'conf', 'schema',
                                 'six')

    # Want to save this value off but only actually want it set for one test
    # For the rest of them, want to make sure we don't need NITF_PLUGIN_PATH
    # set since we're linking XML_DATA_CONTENT in statically
    os.environ['NITF_PLUGIN_PATH_REAL'] = nitfPluginPath
    os.environ['SIX_SCHEMA_PATH'] = sixSchemaPath

    pythonPath = findPythonPath()

    # Sometimes, the __init__.py files don't get created properly
    if pythonPath != '':
        ensureInitFiles(pythonPath)

    # We set the pythonpath with sys.path so it's set for the current script
    # and with os.environ so it's set for child processes
    sys.path.append(pythonPath)
    os.environ['PYTHONPATH'] = pythonPath

def executableName(pathname):

    if platform.system() == 'Windows':
        if pathname.endswith('.exe'):
            return pathname
        return pathname + '.exe'
    return pathname

