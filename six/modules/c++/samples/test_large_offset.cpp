/* ==========================================================================
* This file is part of six.sicd-c++
* ==========================================================================
*
* (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <cstring>

#include <io/TempFile.h>
#include <mem/SharedPtr.h>

#include <six/Container.h>
#include <six/NITFReadControl.h>
#include <six/NITFWriteControl.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sidd/DerivedXMLControl.h>

namespace
{
const size_t NUM_ROWS_IN_FIRST_SEGMENT = 47673; // By manual inspection
const size_t ROWS_TO_SKIP = NUM_ROWS_IN_FIRST_SEGMENT - 1000;
const size_t SICD_ELEMENT_SIZE = sizeof(std::complex<sys::Int16_T>);
const size_t SIDD_ELEMENT_SIZE = sizeof(sys::Int16_T);

void createNITF(const std::string& inputPathname,
        const std::string& outputPathname)
{
    six::XMLControlRegistry registry;
    registry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<
                    six::sicd::ComplexXMLControl>());
    registry.addCreator(six::DataType::DERIVED,
            new six::XMLControlCreatorT<
                    six::sidd::DerivedXMLControl>());
    const std::vector<std::string> schemaPaths;
    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&registry);
    reader.load(inputPathname, schemaPaths);

    std::auto_ptr<six::Data> data(reader.getContainer()->getData(0)->clone());
    size_t elementSize;
    if (data->getDataType() == six::DataType::COMPLEX)
    {
        data->setNumRows(52440); // (11GB / 4) ^ (1/2)
        data->setNumCols(52440);
        data->setPixelType(six::PixelType::RE16I_IM16I);
        elementSize = SICD_ELEMENT_SIZE;
    }
    else
    {
        data->setNumRows(74152); // (11GB / 2) ^ (1/2)
        data->setNumCols(74162);
        data->setPixelType(six::PixelType::MONO16I);
        elementSize = SIDD_ELEMENT_SIZE;
    }

    const size_t AREA = data->getNumRows() * data->getNumCols();
    const size_t FIRST_SIZE = elementSize * data->getNumCols() * ROWS_TO_SKIP;
    mem::SharedPtr<six::Container> container(
            new six::Container(data->getDataType()));
    container->addData(data.release());

    six::NITFWriteControl writer;
    writer.setXMLControlRegistry(&registry);
    writer.initialize(container);

    mem::ScopedArray<six::UByte> imageData(new six::UByte[elementSize * AREA]);
    std::memset(imageData.get(), 0, FIRST_SIZE);
    std::memset(imageData.get() + FIRST_SIZE, 1,
            (AREA * elementSize) - FIRST_SIZE);

    writer.save(imageData.get(), outputPathname);
}

bool checkNITF(const std::string& pathname)
{
    six::XMLControlRegistry registry;
    registry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<
                    six::sicd::ComplexXMLControl>());
    registry.addCreator(six::DataType::DERIVED,
            new six::XMLControlCreatorT<
                    six::sidd::DerivedXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&registry);
    const std::vector<std::string> schemaPaths;
    reader.load(pathname, schemaPaths);
    std::auto_ptr<six::Data> data(reader.getContainer()->getData(0)->clone());

    six::Region region;
    region.setStartRow(ROWS_TO_SKIP);
    region.setNumRows(data->getNumRows() - ROWS_TO_SKIP);
    region.setStartCol(0);
    reader.interleaved(region, 0);
    return static_cast<sys::Int8_T>(region.getBuffer()[0]) == 1;
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <Input NITF>\n";
            return 1;
        }
        const io::TempFile temp;
        const std::string nitfPathname(argv[1]);
        createNITF(nitfPathname, temp.pathname());
        if (checkNITF(temp.pathname()))
        {
            std::cout << "Test passed\n";
            return 0;
        }
        std::cerr << "Failed to read expected data\n";
        return 1;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return 1;
}

