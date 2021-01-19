/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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
#include <stdexcept>

#include <sys/Path.h>
#include <except/Exception.h>
#include <logging/Setup.h>
#include <cphd03/CPHDReader.h>

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        const std::string progname(fs::path(argv[0]).filename());
        if (argc != 2)
        {
            std::cerr << "Usage: " << progname << " <CPHD pathname>\n\n";
            return 1;
        }
        const std::string pathname(argv[1]);

        // Set up a logger to log to the console
        mem::SharedPtr<logging::Logger> logger(logging::setupLogger(progname));

        // There are lots of ways we could print this information
        // For now, just use the default ways each object knows to print itself
        static const size_t NUM_THREADS = 1;
        const cphd03::CPHDReader reader(pathname, NUM_THREADS, logger);

        std::cout << reader.getFileHeader() << "\n"
                  << reader.getMetadata() << "\n"
                  << reader.getVBM() << "\n";

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}
