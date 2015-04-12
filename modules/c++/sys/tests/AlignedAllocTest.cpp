/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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

#include <iostream>

#include <sys/Conf.h>
#include <sys/Path.h>
#include <except/Exception.h>
#include <str/Convert.h>

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: " << sys::Path::basename(argv[0])
                      << " <# bytes to allocate>\n\n";
            return 1;
        }
        const size_t numBytes(str::toType<size_t>(argv[1]));

        // Allocate an aligned buffer
        void* const ptr = sys::alignedAlloc(numBytes);

        // Confirm it's a multiple of 16
        bool const isAligned(reinterpret_cast<size_t>(ptr) % 16 == 0);

        sys::alignedFree(ptr);

        if (!isAligned)
        {
            std::cerr << "Error: buffer " << ptr
                      << " isn't aligned as expected!\n";
            return 1;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::exception: " << ex.getMessage()
                  << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }

    return 0;
}
