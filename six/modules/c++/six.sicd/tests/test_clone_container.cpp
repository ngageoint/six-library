/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include <import/six.h>
#include <import/six/sicd.h>

using namespace six;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        die_printf("usage: %s sicd", argv[0]);
    }

    std::string inputFile = argv[1];
    try
    {

        XMLControlFactory::getInstance(). addCreator(DataType::COMPLEX,
                                                     new XMLControlCreatorT<
                                                             six::sicd::ComplexXMLControl> ());
        NITFReadControl* reader = new NITFReadControl();
        reader->load(inputFile);

        auto container = reader->getContainer();

        // test the clone...
        Container dolly = *container;

        delete reader;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

