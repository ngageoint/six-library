/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

        if (argc > 1)
            cmd = std::string(argv[1]);
        else
            cmd = "echo Scream!";

        io::PipeStream ps(cmd);

        io::StandardOutStream stdo;
        ps.streamTo(stdo);
        stdo.writeln("");
        stdo.close();

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
