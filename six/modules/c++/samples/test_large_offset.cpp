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
// By manual inspection
static const size_t NUM_ROWS_IN_FIRST_SICD_SEGMENT = 33710;
static const size_t NUM_ROWS_IN_FIRST_SIDD_SEGMENT = 67420;
static const size_t SICD_ROWS_TO_SKIP = NUM_ROWS_IN_FIRST_SICD_SEGMENT - 1000;
static const size_t SIDD_ROWS_TO_SKIP = NUM_ROWS_IN_FIRST_SIDD_SEGMENT - 1000;

size_t calculateEdgeLength(size_t elementSize)
{
    const float desiredImageSize = 11e9;
    const float elementsPerImage = desiredImageSize / elementSize;
    return static_cast<size_t>(std::sqrt(elementsPerImage));
}

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

    std::unique_ptr<six::Data> data;
    if (datatype == six::DataType::COMPLEX)
    {
        data = six::sicd::Utilities::createFakeComplexData();
        data->setNumRows(calculateEdgeLength(8));
        data->setNumCols(calculateEdgeLength(8));
        data->setPixelType(six::PixelType::RE32F_IM32F);
    }
    else
    {
        data = six::sidd::Utilities::createFakeDerivedData();
        data->setNumRows(calculateEdgeLength(2));
        data->setNumCols(calculateEdgeLength(2));
        data->setPixelType(six::PixelType::MONO16I);
    }

    const size_t elementsInImage = data->getExtent().area();
    const size_t imageSize = elementsInImage * data->getNumBytesPerPixel();

    mem::SharedPtr<six::Container> container(
        new six::Container(data->getDataType()));
    container->addData(data.release());

    six::NITFWriteControl writer;
    writer.setXMLControlRegistry(&registry);
    writer.initialize(container);

    std::unique_ptr<std::byte[]> imageData(new std::byte[imageSize]);
    if (container->getDataType() == six::DataType::COMPLEX)
    {
        std::complex<float>* complexData =
                reinterpret_cast<std::complex<float>* >(imageData.get());

        for (size_t ii = 0; ii < elementsInImage; ++ii)
        {
            complexData[ii] = std::complex<float>(
                    static_cast<float>(ii),
                    static_cast<float>(ii) * -1);
        }
    }
    else if (container->getDataType() == six::DataType::DERIVED)
    {
        uint16_t* derivedData =
                reinterpret_cast<uint16_t*>(imageData.get());
        for (size_t ii = 0; ii < elementsInImage; ++ii)
        {
            derivedData[ii] = static_cast<uint16_t>(ii);
        }
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
    std::unique_ptr<six::Data> data(reader.getContainer()->getData(0)->clone());
    const size_t ROWS_TO_SKIP = data->getDataType() ==
            six::DataType::COMPLEX ? SICD_ROWS_TO_SKIP : SIDD_ROWS_TO_SKIP;

    six::Region region;
    region.setStartRow(ROWS_TO_SKIP);
    region.setNumRows(data->getNumRows() - ROWS_TO_SKIP);
    reader.interleaved(region, 0);
    auto buffer = region.getBuffer();

    const auto extent = data->getExtent();
    const auto elementsPerRow = extent.col;
    const size_t skipSize = ROWS_TO_SKIP * elementsPerRow;
    const size_t imageSize = extent.area();

    if (data->getDataType() == six::DataType::COMPLEX)
    {
        auto complexBuffer = reinterpret_cast<std::complex<float>* >(buffer);
        for (size_t ii = skipSize; ii < imageSize; ++ii)
        {
            const std::complex<float> currentElement =
                    complexBuffer[ii - skipSize];
            if (currentElement.real() != static_cast<float>(ii))
            {
                return false;
            }
            if (currentElement.imag() != -1 * static_cast<float>(ii))
            {
                return false;
            }
        }
    }
    else
    {
        uint16_t* derivedBuffer =
                reinterpret_cast<uint16_t*>(buffer);
        for (size_t ii = skipSize; ii < imageSize; ++ii)
        {
            if (derivedBuffer[ii - skipSize] != static_cast<uint16_t>(ii))
            {
                return false;
            }
        }
    }
    return true;
}

bool runTest(const six::DataType& datatype)
{
    std::cout << "Running offset test for datatype " << datatype << "\n";

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

int main(int, char**)
{
    try
    {
        bool testPassed = runTest(six::DataType::COMPLEX);
        testPassed = runTest(six::DataType::DERIVED) && testPassed;
        return testPassed ? 0 : 1;
    }
    catch (const std::bad_alloc&)
    {
        std::cerr << "Not enough memory available to build test NITF. "
                "Skipping test.\n";
        return 0;
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

