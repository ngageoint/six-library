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

#include <import/sys.h>
#include <import/str.h>
#include <import/except.h>

int main(int argc, char **argv)
{
    try
    {

        sys::TimeStamp ts;
        std::cout << ts.gmt() << std::endl;
        std::cout << ts.local() << std::endl;

        sys::OS os;
        std::cout << os["PATH"] << std::endl;

        std::string cwd = os.getCurrentWorkingDirectory();

        sys::Path path(cwd);

        std::cout << "Searching directory: " << path << std::endl;

        sys::DirectoryEntry d(path);

        for (sys::DirectoryEntry::Iterator p = d.begin(); p != d.end(); ++p)
        {
            std::cout << "Found file: " << *p << std::endl;
        }
    }
    catch (except::Throwable& t)
    {
        std::cout << t.toString() << std::endl;
    }
    catch (std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Caught unnknown throwable" << std::endl;
    }
    return 0;

}
