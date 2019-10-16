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
#include <cassert>
#include <iostream>
#include <fstream>
#include <streambuf>

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        sys::OS os;
        sys::Path p("this/that");
        p.makeDirectory(true);
        os.createSymlink(p.getAbsolutePath(), "symlink");

        assert(os.exists("symlink"));
        assert(os.exists("this/that"));

        std::string output = "Test message! Testing 1, 2, 3; testing 1, 2, 3?";

        std::ofstream fout;
        fout.open("this/that/test.txt");
        fout << output;
        fout.close();

        std::ifstream fin;
        fin.open("symlink/test.txt");
        std::string input;
        std::getline(fin, input);
        fin.close();

        assert(input == output);

        os.remove("this");
        os.removeSymlink("symlink");

        assert(!os.exists("symlink"));
        assert(!os.exists("this/that"));

        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::exception: " << ex.getMessage()
                  << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
    }
    return 1;
}
