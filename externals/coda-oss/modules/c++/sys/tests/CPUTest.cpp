/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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
#include <vector>

#include <import/sys.h>

namespace
{
void printCPUs(const std::string& header, const std::vector<int>& cpus)
{
    std::vector<int>::const_iterator cpu;

    std::cout << header << ": ";
    for (cpu = cpus.begin(); cpu != cpus.end(); ++cpu)
    {
        if (cpu != cpus.begin())
        {
            std::cout << ", ";
        }
        std::cout << *cpu;
    }
    std::cout << std::endl;
}
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        sys::OS os;

        std::cout << "Present number of CPUs: "
                  << os.getNumCPUs() << std::endl;
        std::cout << "Present number of physical CPUs: "
                  << os.getNumPhysicalCPUs() << std::endl;
        std::cout << "Available number of CPUs: "
                  << os.getNumCPUsAvailable() << std::endl;
        std::cout << "Available number of physical CPUs: "
                  << os.getNumPhysicalCPUsAvailable() << std::endl;

        std::vector<int> physicalCPUs;
        std::vector<int> htCPUs;
        os.getAvailableCPUs(physicalCPUs, htCPUs);
        printCPUs("Available physical CPUs", physicalCPUs);
        printCPUs("Available HT CPUs", htCPUs);

    }
    catch (const except::Throwable& t)
    {
        std::cerr << "Caught throwable: " << t.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Caught unnamed exception" << std::endl;
        return 1;
    }
    return 0;
}
