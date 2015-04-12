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
#include <fstream>
#include <iomanip>

using namespace sys;

int main(int argc, char **argv)
{
    try
    {
        sys::OS os;
        std::string fileName = os.getCurrentWorkingDirectory()
                + os.getDelimiter() + ".." + os.getDelimiter() + "blah.txt";

        std::cout << "File Name as string: " << fileName << std::endl;

        Path parentPath(os.getCurrentWorkingDirectory(), "..");
        Path filePath(parentPath, "blah.txt");
        std::cout << "File Name as path: " << filePath.getPath() << std::endl;

        Path::StringPair parts = Path::splitPath(fileName);
        std::cout << "1) " << parts.first << " --- " << parts.second
                << std::endl;

        parts = filePath.split();
        std::cout << "2) " << parts.first << " --- " << parts.second
                << std::endl;
        parts = Path::splitDrive("c:/junk.txt");
        std::cout << "1) " << parts.first << " --- " << parts.second
                << std::endl;
        filePath = Path("c:/junk.txt");
        parts = filePath.splitDrive();
        std::cout << "2) " << parts.first << " --- " << parts.second
                << std::endl;
        filePath = Path("/data/nitf///data/vendor1.ntf");

        std::string base =
                Path::basename("/data/nitf///data/vendor1.ntf", true);
        std::cout << "1) " << base << std::endl;

        base = filePath.getBasePath(true);
        std::cout << "2) " << base << std::endl;

        parts = Path::splitPath("/data.txt");
        std::cout << parts.first << " --- " << parts.second << std::endl;

        parts = Path::splitExt(fileName);
        std::cout << parts.first << " --- " << parts.second << std::endl;

        parts = Path::splitExt(Path::splitPath(fileName).second);
        std::cout << parts.first << " --- " << parts.second << std::endl;

        std::cout << Path::normalizePath(fileName) << std::endl;
        std::cout << Path::normalizePath("c:/data/nitf/data/vendor1.ntf")
                << std::endl;
        std::cout << Path("/data/nitf///data/vendor1.ntf").normalize()
                << std::endl;

        std::cout << Path::normalizePath("/data/nitf///data/../vendor1.ntf")
                << std::endl;

        std::cout
                << Path::normalizePath(
                                       "../data/../../..//./nitf///data/../vendor1.ntf")
                << std::endl;

        std::cout
                << Path::normalizePath(
                                       "data/junk/tzellman/../../../../../..///./nitf///data/../vendor1.ntf")
                << std::endl;

        std::cout << "1) " << Path("/data/junk/", "test.txt").getPath()
                << std::endl;
        std::cout << "2) " << Path::joinPaths("/data/junk/", "///test.txt")
                << std::endl;
        std::cout << "3) " << Path("/data/junk/").join("/test.txt").getPath()
                << std::endl;
        std::cout << "4) " << Path::joinPaths("/data/junk", "test.txt")
                << std::endl;
        std::cout
                << Path(
                        "data/junk/tzellman/../../../../../..///./nitf///data/../vendor1.ntf").getAbsolutePath()
                << std::endl;
        std::cout
                << Path::normalizePath(
                                       "data/junk/tzellman/../../../../../..///./nitf///data/../vendor1.ntf")
                << std::endl;
        std::cout
                << Path::absolutePath(
                                      "c:/data/junk/tzellman/../../../../../..///./nitf///data/../vendor1.ntf")
                << std::endl;
        std::cout << Path::normalizePath("c:/../../../junk.txt") << std::endl;

        std::cout << Path::absolutePath("/home/tzellman/dev/") << std::endl;

        const std::string currentDirWithDelimiter =
            os.getCurrentWorkingDirectory() + os.getDelimiter();
        parts = Path::splitPath(currentDirWithDelimiter);
        std::cout << "1) " << parts.first << " --- " << parts.second
                        << std::endl;

        std::cout << Path::normalizePath(currentDirWithDelimiter)
                  << std::endl;

        std::cout << Path::absolutePath(currentDirWithDelimiter)
                  << std::endl;

        std::cout << Path::joinPaths(parts.first + os.getDelimiter(),
                                     parts.second + os.getDelimiter())
                  << std::endl;
    }
    catch (const except::Throwable& t)
    {
        std::cerr << "Caught throwable: " << t.toString() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Caught std::exception: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "Caught unnamed exception" << std::endl;
    }
    return 0;
}
