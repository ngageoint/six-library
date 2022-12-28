/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <import/sys.h>
#include <iostream>

using namespace sys;

int main(int, char**)
{
    try
    {
        sys::OS os;
        size_t freePhysMem=0, totalPhysMem=0;
        os.getMemInfo(totalPhysMem, freePhysMem);
        std::cout << "Total system memory: " << totalPhysMem << " MB" << std::endl;
        std::cout << "Free system memory: " << freePhysMem << " MB" << std::endl;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::exception: " << ex.getMessage()
                  << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }

    return 0;
}
