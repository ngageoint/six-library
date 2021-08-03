/* =========================================================================
 * This file is part of numpyutils-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * math-c++ is free software; you can redistribute it and/or modify
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

#include <TestCase.h>
#include <config/compiler_extensions.h>
#include <numpyutils/numpyutils.h>

namespace
{
TEST_CASE(testGetNumElements)
{
    // NOTE: This test leaks memory.
    // The Python C API naturally assume you're running in a Python extension,
    // so the "right" way to manage memory involves making sure the Python
    // runtime as the right information about e.g. reference counting and
    // ownership.
    // We are just running these functions standalone to verify functionality,
    // so we don't have anyone to do the cleanup work.
    std::vector<int> data(50);
    PyObject* pyArrayObject = numpyutils::toNumpyArray(5, 10, NPY_INT, data.data());
    TEST_ASSERT_EQ(50, numpyutils::getNumElements(pyArrayObject));

    PyObject* nonArrayObject = PyLong_FromLong(123L);
    TEST_THROWS(numpyutils::getNumElements(nonArrayObject));
}
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(testGetNumElements);
    // wreaks havoc from the bowels of <numpy/arrayobject.h>
    CODA_OSS_mark_symbol_unused(_import_array);
    return 0;
}
