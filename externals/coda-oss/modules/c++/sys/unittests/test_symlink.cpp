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
//#include <import/io.h>
#include <iostream>
#include <fstream>
#include <streambuf>

#include "TestCase.h"

namespace
{

TEST_CASE(testCreateSymlink)
{
    try
    {
        sys::OS os;
        sys::Path p("this/that");
        p.makeDirectory(true);
        os.createSymlink(p.getAbsolutePath(), "symlink");

        TEST_ASSERT(os.exists("symlink"));
        TEST_ASSERT(os.exists("this/that"));

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

        TEST_ASSERT_EQ(input, output);

        os.remove("this");
        os.removeSymlink("symlink");

        TEST_ASSERT(!os.exists("symlink"));
        TEST_ASSERT(!os.exists("this/that"));
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        TEST_ASSERT(false);
    }
    catch (const except::Exception& ex)
    {
        std::cerr << "Caught except::exception: " << ex.getMessage()
                  << std::endl;
        TEST_ASSERT(false);
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        TEST_ASSERT(false);
    }
}

}


int main(int argc, char** argv)
{
    TEST_CHECK(testCreateSymlink);

    return 0;
}
