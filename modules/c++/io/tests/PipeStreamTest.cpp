/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <import/sys.h>
#include <import/io.h>
#include <import/except.h>

int main(int argc, char **argv)
{
    try
    {
        std::string cmd;
        std::string testMessage("Scream!");

        if (argc > 1)
        {
            for (int ii = 1; ii < argc; ++ii)
            {
                cmd += std::string(argv[ii]) + " ";
            }
        }
        else
        {
            cmd = "echo " + testMessage;
        }

        io::PipeStream ps(cmd, 1);

        io::StandardOutStream stdo;
        size_t ioSize = ps.streamTo(stdo);
        stdo.close();

        // check echo output size vs test message size plus newline
        if (argc == 1 && ioSize != testMessage.size() + 1)
            std::cerr << "Invalid I/O size:  " << ioSize << std::endl;

        if (ps.close() != 0)
        {
            std::cout << "Child Process Successful but Internally Failed!" << 
                std::endl;
        }
        else
        {
            std::cout << "Child Process Successful!" << std::endl;
        }
    }
    catch (const except::Throwable& ex)
    {
        std::cerr << "Caught C++ exception: " << 
            ex.getMessage() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught standard exception from " << 
            ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unnamed Unwanted exception" << std::endl;
    }

    return 0;
}
