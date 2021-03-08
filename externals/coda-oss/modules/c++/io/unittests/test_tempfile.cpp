/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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

#include <fstream>

#include <sys/OS.h>
#include <io/TempFile.h>
#include "TestCase.h"

namespace
{
TEST_CASE(testTempFileCreation)
{
    const sys::OS os;
    const io::TempFile tempFile;
    // This should just give us a name to a file that doesn't exist yet
    TEST_ASSERT(os.exists(tempFile.pathname()));
}

TEST_CASE(testFileDestroyed)
{
    const sys::OS os;
    std::string pathname;
    {
        const io::TempFile tempFile;
        pathname = tempFile.pathname();
        std::ofstream out(pathname.c_str());
        out << "Test text";
    }
    // File should be destroyed on destruction
    TEST_ASSERT(!os.exists(pathname));
}

}

int main(int, char**)
{
    TEST_CHECK(testTempFileCreation);
    TEST_CHECK(testFileDestroyed);
    return 0;
}


