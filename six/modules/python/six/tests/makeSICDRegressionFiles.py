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

import imp
import os
import sys
from subprocess import call
from itertools import product

import utils


def checkArgs(version, alg, imageType):
    if imageType != '' and alg != 'RMA':
        return False
    if alg == 'RGAZCOMP' and not version.startswith('1'):
        return False
    if imageType == 'RMCR' and not version.startswith('1'):
        return False
    return True


def createNITFs(version, alg, imageType, home):
    imp.load_source('test_create_sicd_xml', os.path.join(home, 'six',
            'modules', 'python', 'six.sicd', 'tests',
                'test_create_sicd_xml.py'))

    from test_create_sicd_xml import initData, writeNITF
    if not checkArgs(version, alg, imageType):
        return

    outputName = "sicd_{0}({1}){2}.nitf".format(version, alg, imageType)
    print('Creating file {}.nitf'.format(outputName))

    cmplx = initData(includeNITF=True, version=version, alg=alg,
                     imageType=imageType)
    writeNITF(os.path.join(home, 'regression_files', 'six.sicd', outputName),
              cmplx)

def run():
    sicdVersions = ['0.4.0', '0.4.1', '0.5.0', '1.0.0', '1.0.1', '1.1.0',
                    '1.2.0', '1.2.1']
    formationAlgs = ['PFA', 'RMA', 'RGAZCOMP']
    imageTypes = ['RMAT', 'RMCR', 'INCA']

    home = utils.findSixHome()
    if not os.path.isdir(os.path.join(home, 'regression_files', 'six.sicd')):
        os.makedirs(os.path.join(home, 'regression_files', 'six.sicd'))
    for args in product(sicdVersions, formationAlgs, imageTypes):
        if args[1] != 'RMA':
            args = (args[0], args[1], '')
        createNITFs(args[0], args[1], args[2], home)

