#!/usr/bin/env python

#
# =========================================================================
# This file is part of six.sicd-python
# =========================================================================
#
# (C) Copyright 2004 - 2017, MDA Information Systems LLC
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


import sys
from pysix.six_sicd import readRecord


def show_record(sicdPathname):
    record = readRecord(sicdPathname)
    print('Record members: {}'.format(dir(record)))
    print('Number of image segments: {}'.format(record.getNumImages()))
    print('Number of DE segments: {}'.format(record.getNumDataExtensions()))
    print('Version: {}'.format(record.getVersion()))

    if record.getNumImages() > 0:
        imageSubheader = record.getImageSegment(0).getSubheader()
        print('ImageSubheader members: {}'.format(dir(imageSubheader)))

    if record.getNumDataExtensions() > 0:
        deSubheader = record.getDataExtension(0).getSubheader()
        print('DESubheader members: {}'.format(dir(deSubheader)))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: {0} <SICD file>'.format(sys.argv[0]))
        sys.exit(-1)

    show_record(sys.argv[1])

