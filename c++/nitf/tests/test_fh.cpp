/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.hpp>
#include <import/sys.h>
#include <import/except.h>

int main(int argc, char** argv)
{
    try
    {
        std::cout << "Crazy FileHeader test" << std::endl;
        //Sticking a second in breaks the paradigm!!!
        nitf::FileHeader fileHeader;
        std::cout << "Created FileHeader" << std::endl;

        //fileHeader.name = "1";
        nitf::FileHeader two = fileHeader;
        std::cout << "Copied FileHeader" << std::endl;
        //two.name = "2";

        nitf::FileHeader three = two;
        std::cout << "Copied FileHeader copy" << std::endl;
        //three.name = "3";

        nitf::FileHeader alone;
        std::cout << "Created another FileHeader" << std::endl;
        //alone.name = "4";

        // This guy should be different
        nitf::FileHeader myFileHeader;
        std::cout << "Created yet another FileHeader" << std::endl;
        //myFileHeader.name = "5";

        nitf::FileHeader myFileCopy = myFileHeader;
        std::cout << "Copied yet another FileHeader" << std::endl;
        //myFileCopy.name = "6";

        nitf::FileHeader* myNewCopy = new nitf::FileHeader(myFileCopy);
        std::cout << "New-allocated copy of FileHeader" << std::endl;
        //myNewCopy->name = "7";
        delete myNewCopy;
        std::cout << "Deleted new-allocated copy of FileHeader" << std::endl;

        nitf::FileHeader* myNonCopy = new nitf::FileHeader;
        std::cout << "New-allocated a FileHeader" << std::endl;
        //myNonCopy->name = "8";
        delete myNonCopy;
        std::cout << "Deleted new-allocated FileHeader" << std::endl;

        std::cout << "Setting file header" << std::endl;
        fileHeader.getFileHeader().set("NITF");
        std::cout << "Setting file version" << std::endl;
        fileHeader.getFileVersion().set("2.1");

        two = fileHeader;

        std::cout << "Setting file header" << std::endl;
        myFileHeader.getFileHeader().set("FTIN");
        std::cout << "Setting file version" << std::endl;
        myFileHeader.getFileVersion().set("1.2");

        nitf::Extensions ext;

        nitf::HashTable hash;
        ext.setHash(hash);

        std::cout << fileHeader.getFileHeader().toString() << std::endl;
        std::cout << fileHeader.getFileVersion().toString() << std::endl;
        std::cout << two.getFileHeader().toString() << std::endl;
        std::cout << two.getFileVersion().toString() << std::endl;

        std::cout << myFileHeader.getFileHeader().toString() << std::endl;
        std::cout << myFileHeader.getFileVersion().toString() << std::endl;

        return 0;
    }
    catch (except::Throwable& t)
    {
        std::cout << t.getTrace() << std::endl;
    }
}
