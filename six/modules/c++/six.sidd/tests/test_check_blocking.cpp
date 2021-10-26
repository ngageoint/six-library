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
#include <vector>

#include <import/six/sidd.h>
#include <cli/ArgumentParser.h>
#include <io/TempFile.h>
#include <math/Utilities.h>
#include <str/Convert.h>
#include "six/NITFWriteControl.h"
#include "six/Types.h"

namespace
{
void generateData(const six::Data& data, std::vector<std::complex<float>>& buffer)
{
    buffer.resize(getExtent(data).area());
    for (size_t ii = 0; ii < buffer.size(); ++ii)
    {
        buffer[ii] = std::complex<float>(ii % 100);
    }
}

std::unique_ptr<six::Data> read(const std::string& filename)
{
    six::NITFReadControl reader;
    reader.load(filename);
    const auto container = reader.getContainer();
    std::unique_ptr<six::Data> data(container->getData(0)->clone());
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
    return std::to_string(numBlocksRequired * totalBlockSize);
}

void writeSingleImage(const six::Data& data, const std::string& pathname,
        const std::string& blockSize, size_t imageSideSize)
{
    std::unique_ptr<six::Data> workingData(data.clone());
    const std::string productSize = computeProductSize(blockSize,
            imageSideSize, workingData->getNumBytesPerPixel());
    workingData->setNumRows(imageSideSize);
    workingData->setNumCols(imageSideSize);

    std::vector<std::complex<float>> buffer;
    generateData(*workingData, buffer);

    mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::DERIVED));
    container->addData(std::move(workingData));

    const six::cxbuffer_list buffers{ buffer };

    six::Options options;
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, productSize);

    six::NITFWriteControl writer(options, container);
    writer.save(buffers, pathname, std::vector<std::filesystem::path>());

}

void writeTwoImages(const six::Data& data, const std::string& pathname,
        const std::string& blockSize, size_t largeImageSize,
        size_t smallImageSize)
{
    std::unique_ptr<six::Data> firstData(data.clone());
    std::unique_ptr<six::Data> secondData(data.clone());

    // The first image will be blocked, and the second is too small.
    firstData->setNumRows(largeImageSize);
    firstData->setNumCols(largeImageSize);
    secondData->setNumRows(smallImageSize);
    secondData->setNumCols(smallImageSize);

    const std::string productSize = computeProductSize(blockSize,
            largeImageSize, data.getNumBytesPerPixel());

    std::vector<std::complex<float>> firstBuffer;
    std::vector<std::complex<float>> secondBuffer;
    generateData(*firstData, firstBuffer);
    generateData(*secondData, secondBuffer);

    mem::SharedPtr<six::Container> container(new six::Container(
        six::DataType::DERIVED));
    container->addData(std::move(firstData));
    container->addData(std::move(secondData));

    const six::cxbuffer_list buffers{ firstBuffer, secondBuffer };

    six::Options options;
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_ROWS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_NUM_COLS_PER_BLOCK, blockSize);
    options.setParameter(
            six::NITFHeaderCreator::OPT_MAX_PRODUCT_SIZE, productSize);

    six::NITFWriteControl writer(options, container);
    writer.save(buffers, pathname, std::vector<std::filesystem::path>());
}

void assignBuffer(std::unique_ptr<six::UByte[]>& buffer, size_t& bufferSize,
        size_t index, six::NITFReadControl& reader)
{
    six::Region region;
    buffer.reset(reader.interleaved(region, index));
    bufferSize = getExtent(region).area();
}

bool compare(const std::string& twoImageSidd,
        const std::string& firstImage,
        const std::string& secondImage)
{
    six::NITFReadControl firstReader;
    firstReader.load(twoImageSidd);

    std::unique_ptr<six::UByte[]> firstBuffers[2];
    size_t firstBufferSizes[2];
    assignBuffer(firstBuffers[0], firstBufferSizes[0], 0, firstReader);
    assignBuffer(firstBuffers[1], firstBufferSizes[1], 1, firstReader);

    six::NITFReadControl secondReader;
    secondReader.load(firstImage);
    six::NITFReadControl thirdReader;
    thirdReader.load(secondImage);

    std::unique_ptr<six::UByte[]> secondBuffers[2];
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
        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));

        const std::string siddPathname(options->get<std::string>("sidd"));

        six::XMLControlFactory::getInstance().addCreator<six::sidd::DerivedXMLControl>();

        std::unique_ptr<six::Data> sidd = read(siddPathname);

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

