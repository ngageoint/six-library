/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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

#include <string>
#include <vector>
#include <zip/ZipOutputStream.h>
#include <io/FileInputStream.h>

namespace
{
void usage(const std::string& progname)
{
    std::cerr << "Usage: " << progname
        << " -i <file #1> <zipPath #1> <file#2> <zipPath #2> ..."
        << " [-o Output pathname]";
}
}

int main(int argc, char** argv)
{
    try
    {
        std::string outputPathname;
        std::vector<std::string> inputPathnames;
        std::vector<std::string> zipPathnames;

        int index = 1;
        while (index < argc)
        {
            if (!::strcmp(argv[index], "-i"))
            {
                ++index;
                while (index < argc - 1)
                {
                    if ((::strlen(argv[index]) > 0 && argv[index][0] == '-') ||
                            (::strlen(argv[index + 1]) > 0 && 
                                    argv[index + 1][0] == '-'))
                    {
                        break;
                    }
                    inputPathnames.push_back(argv[index++]);
                    zipPathnames.push_back(argv[index++]);
                }
            }
            else if (!::strcmp(argv[index], "-o") && index + 1 < argc)
            {
                outputPathname = argv[index + 1];
                index += 2;
            }
            else
            {
                usage(argv[0]);
                return 1;
            }
        }

        zip::ZipOutputStream output(outputPathname);
        for (size_t ii = 0; ii < inputPathnames.size(); ++ii)
        {
            output.write(inputPathnames[ii], zipPathnames[ii]);
        }
        output.close();
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
    catch (...)
    {
        std::cerr << "Caught unknown exception\n";
        return 1;
    }
}
