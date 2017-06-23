#!/user/bin/env python

#
# =========================================================================
# This file is part of six-python
# =========================================================================
#
# (C) Copyright 2004 - 2016, MDA Information Systems LLC
#
# six-python is free software; you can redistribute it and/or modify
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


import numpy
import sys
import traceback
from datetime import datetime
from pysix.six_base import DateTime


def convertToPython():
    original = DateTime()
    converted = original.toPythonDateTime()
    try:
        assert(original.getYear() == converted.year)
        assert(original.getMonth() == converted.month)
        assert(original.getDayOfMonth() == converted.day)
        assert(original.getHour() == converted.hour)
        assert(original.getMinute() == converted.minute)
        assert(int(original.getSecond()) == converted.second)
        microseconds = (original.getSecond() - int(original.getSecond())) * 1e6
        print('{} vs {}'.format(microseconds, converted.microsecond))
        assert(numpy.isclose(microseconds, converted.microsecond, rtol=1e-4))
    except AssertionError:
        traceback.print_exc()
        return False
    return True


def convertFromPython():
    original = datetime.now()
    converted = DateTime.fromPythonDateTime(original)
    try:
        assert(converted.getYear() == original.year)
        assert(converted.getMonth() == original.month)
        assert(converted.getDayOfMonth() == original.day)
        assert(converted.getHour() == original.hour)
        assert(converted.getMinute() == original.minute)
        assert(int(converted.getSecond()) == original.second)
        microsecond = (converted.getSecond() - int(converted.getSecond())) * 1e6
        print('{} vs {}'.format(microsecond, original.microsecond))
        assert(numpy.isclose(microsecond, original.microsecond, rtol=1e-4))
    except AssertionError:
        traceback.print_exc()
        return False
    return True


def run():
    if convertToPython() and convertFromPython():
        print("Tests passed")
        sys.exit(0)
    sys.exit(1)


if __name__ == '__main__':
    run()

