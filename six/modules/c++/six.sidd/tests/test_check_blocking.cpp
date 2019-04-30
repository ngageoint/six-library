/* =========================================================================
* This file is part of six.sidd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2017, MDA Information Systems LLC
*
* six.sidd-c++ is free software; you can redistribute it and/or modify
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

#include <iostream>
#include <import/six/sidd.h>
#include <cli/ArgumentParser.h>
#include <io/TempFile.h>
#include <math/Utilities.h>
#include <str/Convert.h>
#include "six/NITFWriteControl.h"
#include "six/Types.h"

namespace
{
void generateData(const six::Data& data, mem::ScopedArray<six::UByte>& buffer)
{
    const size_t size = data.getNumRows() * data.getNumCols();
    buffer.reset(new six::UByte[size]);
    for (size_t ii = 0; ii < size; ++ii)
    {
        buffer[ii] = static_cast<six::UByte>(ii % 100);
    }
}

std::auto_ptr<six::Data> read(const std::string& filename)
{
    six::NITFReadControl reader;
    reader.load(filename);
    mem::SharedPtr<const six::Container> container = reader.getContainer();
    std::auto_ptr<six::Data> data(container->getData(0)->clone());
    return data;
}

std::string computeProductSize(const std::string& blockSizeStr,
        size_t imageSideSize,
        size_t numBytesPerPixel)
{
    const size_t blockSize = str::toType<size_t>(blockSizeStr);
    const size_t totalBlockSize = math::square(blockSize);
    const size_t totalImageSize = math::square(imageSideSize) *
            numBytesPerPixel;
    const size_t numBlocksRequired = std::ceil(
            static_cast<double>(totalImageSize) / totalBlockSize);
    return str::toString(numBlocksRequired * totalBlockSize);
}

void writeSingleImage(const six::Data& data, const std::string& pathname,
        const std::string& blockSize, size_t imageSideSize)
{
    std::auto_ptr<six::Data> workingData(data.clone());
    const std::string productSize = computeProductSize(blockSize,
            imageSideSize, workingData->getNumBytesPerPixel());
    workingData->setNumRows(imageSideSize);
    workingData->setNumCols(imageSideSize);

    mem::ScopedArray<six::UByte> buffer;
    generateData(*workingData, buffer);

    mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::DERIVED));
    container->addData(workingData);

    six::BufferList buffers(1);
    buffers[0] = buffer.get();

    six::Options options;
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, productSize);

    six::NITFWriteControl writer(options, container);
    writer.save(buffers, pathname, std::vector<std::string>());

}

void writeTwoImages(const six::Data& data, const std::string& pathname,
        const std::string& blockSize, size_t largeImageSize,
        size_t smallImageSize)
{
    std::auto_ptr<six::Data> firstData(data.clone());
    std::auto_ptr<six::Data> secondData(data.clone());

    // The first image will be blocked, and the second is too small.
    firstData->setNumRows(largeImageSize);
    firstData->setNumCols(largeImageSize);
    secondData->setNumRows(smallImageSize);
    secondData->setNumCols(smallImageSize);

    const std::string productSize = computeProductSize(blockSize,
            largeImageSize, data.getNumBytesPerPixel());

    mem::ScopedArray<six::UByte> firstBuffer;
    mem::ScopedArray<six::UByte> secondBuffer;
    generateData(*firstData, firstBuffer);
    generateData(*secondData, secondBuffer);

    mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::DERIVED));
    container->addData(firstData);
    container->addData(secondData);

    six::BufferList buffers(2);
    buffers[0] = firstBuffer.get();
    buffers[1] = secondBuffer.get();

    six::Options options;
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, productSize);

    six::NITFWriteControl writer(options, container);
    writer.save(buffers, pathname, std::vector<std::string>());
}

void assignBuffer(mem::ScopedArray<six::UByte>& buffer, size_t& bufferSize,
        size_t index, six::NITFReadControl& reader)
{
    six::Region region;
    buffer.reset(reader.interleaved(region, index));
    bufferSize = region.getNumRows() * region.getNumCols();
}

bool compare(const std::string& twoImageSidd,
        const std::string& firstImage,
        const std::string& secondImage)
{
    six::NITFReadControl firstReader;
    firstReader.load(twoImageSidd);

    mem::ScopedArray<six::UByte> firstBuffers[2];
    size_t firstBufferSizes[2];
    assignBuffer(firstBuffers[0], firstBufferSizes[0], 0, firstReader);
    assignBuffer(firstBuffers[1], firstBufferSizes[1], 1, firstReader);

    six::NITFReadControl secondReader;
    secondReader.load(firstImage);
    six::NITFReadControl thirdReader;
    thirdReader.load(secondImage);

    mem::ScopedArray<six::UByte> secondBuffers[2];
    size_t secondBufferSizes[2];
    assignBuffer(secondBuffers[0], secondBufferSizes[0], 0, secondReader);
    assignBuffer(secondBuffers[1], secondBufferSizes[1], 0, thirdReader);

    if (firstBufferSizes[0] != secondBufferSizes[0] ||
        firstBufferSizes[1] != secondBufferSizes[1])
    {
        std::cerr << "Buffer lengths do not match\n";
        return false;
    }

    for (size_t ii = 0; ii < firstBufferSizes[0]; ++ii)
    {
        if (firstBuffers[0][ii] != secondBuffers[0][ii])
        {
            std::cerr << "Buffer mismatch at index " << ii << std::endl;
            return false;
        }
    }

    for (size_t ii = 0; ii < firstBufferSizes[1]; ++ii)
    {
        if (firstBuffers[1][ii] != secondBuffers[1][ii])
        {
            std::cerr << "Buffer mismatch at index " << ii << std::endl;
            return false;
        }
    }
    std::cout << "Images match\n";
    return true;
}
}

int main(int argc, char** argv)
{
    try
    {
        cli::ArgumentParser parser;
        parser.setDescription("This program tests that blocking is being "\
                "set correctly.");
        parser.addArgument("inputSIDD", "Input SIDD", cli::STORE, "sidd",
                "SIDD", 1, 1);
        const std::auto_ptr<cli::Results> options(parser.parse(argc, argv));

        const std::string siddPathname(options->get<std::string>("sidd"));

        six::XMLControlFactory::getInstance().addCreator(
                six::DataType::DERIVED,
                new six::XMLControlCreatorT<
                six::sidd::DerivedXMLControl>());

        std::auto_ptr<six::Data> sidd = read(siddPathname);

        if (sidd->getDataType() != six::DataType::DERIVED)
        {
            throw except::Exception(Ctxt("Exepcted " + siddPathname + " to be"\
                    " DERIVED. Got " + sidd->getDataType().toString()));
        }

        sidd->setPixelType(six::PixelType::MONO8I);
        io::TempFile twoImageSIDD;
        io::TempFile singleImageBlocked;
        io::TempFile singleImageUnblocked;
        writeTwoImages(*sidd, twoImageSIDD.pathname(), "10", 11, 9);
        writeSingleImage(*sidd, singleImageBlocked.pathname(), "10", 11);
        writeSingleImage(*sidd, singleImageUnblocked.pathname(), "10", 9);

        return compare(twoImageSIDD.pathname(),
                singleImageBlocked.pathname(),
                singleImageUnblocked.pathname()) ? 0 : 1;
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

