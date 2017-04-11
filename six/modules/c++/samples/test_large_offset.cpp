/* ==========================================================================
* This file is part of six-c++
* ==========================================================================
*
* (C) Copyright 2004 - 2017, MDA Information Systems LLC
*
* six-c++ is free software; you can redistribute it and/or modify
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
#include <six/sicd/Utilities.h>
#include <six/sidd/DerivedXMLControl.h>
#include <six/sidd/Utilities.h>

namespace
{
static const size_t NUM_ROWS_IN_FIRST_SEGMENT = 47673; // By manual inspection
static const size_t ROWS_TO_SKIP = NUM_ROWS_IN_FIRST_SEGMENT - 1000;

void createNITF(const std::string& outputPathname,
        const six::DataType& datatype)
{
    six::XMLControlRegistry registry;
    registry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<
                    six::sicd::ComplexXMLControl>());
    registry.addCreator(six::DataType::DERIVED,
            new six::XMLControlCreatorT<
                    six::sidd::DerivedXMLControl>());
    const std::vector<std::string> schemaPaths;

    std::auto_ptr<six::Data> data;
    if (datatype == six::DataType::COMPLEX)
    {
        data = six::sicd::Utilities::createFakeComplexData();
        data->setNumRows(52440); // (11GB / 4) ^ (1/2)
        data->setNumCols(52440);
        data->setPixelType(six::PixelType::RE16I_IM16I);
    }
    else
    {
        data = six::sidd::Utilities::createFakeDerivedData();
        data->setNumRows(74152); // (11GB / 2) ^ (1/2)
        data->setNumCols(74162);
        data->setPixelType(six::PixelType::MONO16I);
    }

    const size_t bytesPerRow = data->getNumCols() * data->getNumBytesPerPixel();
    const size_t imageSize = data->getNumRows() * bytesPerRow;

    mem::SharedPtr<six::Container> container(
            new six::Container(data->getDataType()));
    container->addData(data.release());

    six::NITFWriteControl writer;
    writer.setXMLControlRegistry(&registry);
    writer.initialize(container);

    mem::ScopedArray<six::UByte> imageData(new six::UByte[imageSize]);
    for (size_t ii = 0; ii < imageSize; ++ii)
    {
        imageData[ii] = static_cast<six::UByte>(ii);
    }
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
    six::UByte* buffer = region.getBuffer();

    const size_t bytesPerRow = data->getNumCols() * data->getNumBytesPerPixel();
    const size_t skipSize = ROWS_TO_SKIP * bytesPerRow;
    const size_t imageSize = data->getNumRows() * bytesPerRow;

    for (size_t ii = skipSize; ii < imageSize; ++ii)
    {
        if (buffer[ii - skipSize] != static_cast<six::UByte>(ii))
        {
            return false;
        }
    }
    return true;
}

bool runTest(const six::DataType& datatype)
{
    std::cerr << "Running offset test for datatype " << datatype.toString()
              << "\n";

    const io::TempFile temp;
    createNITF(temp.pathname(), datatype);
    if (checkNITF(temp.pathname()))
    {
        std::cout << "Test passed\n";
        return true;
    }
    std::cerr << "Test failed\n";
    return false;
}
}

int main(int argc, char** argv)
{
    try
    {
        bool testPassed = runTest(six::DataType::COMPLEX);
        testPassed = runTest(six::DataType::DERIVED);
        return testPassed ? 0 : 1;
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

