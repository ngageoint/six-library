#!/usr/bin/env python

#
# =========================================================================
# This file is part of six-c++
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
import subprocess

script = os.path.join(os.getcwd(), 'externals', 'coda-oss',
                      'build', 'scripts', 'makeEnums.py')

inputFile = os.path.join(os.path.split(__file__)[0], 'enums.txt')
outDir = os.path.join(os.path.split(__file__)[0], os.pardir, 'include', 'six')

assert os.path.exists(script)
assert os.path.exists(inputFile)
assert os.path.exists(outDir)

target = os.path.join(outDir, 'Enums.h')
if os.path.exists(target):
    os.remove(target)
    
with open(target, 'w') as f:
    f.write("""/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __SIX_ENUMS_H__
#define __SIX_ENUMS_H__

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/scene.h>

// -----------------------------------------------------------------------------
// This file is auto-generated - please do NOT edit directly
// -----------------------------------------------------------------------------

namespace six
{

const int NOT_SET_VALUE = 2147483647; //std::numeric_limits<int>::max()

""")
    f.flush()
    subprocess.call(['python', script, inputFile], stdout=f, shell=True)
    f.write("""}

#endif
""")

