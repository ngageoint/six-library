/* =========================================================================
* This file is part of six-c++
* =========================================================================
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

#include <iostream>
#include <iterator>
#include <import/six/sidd.h>
#include <import/six/sicd.h>
#include <import/cli.h>
#include "six/NITFWriteControl.h"
#include "six/Types.h"

namespace
{
void generateData(const six::Data& data, mem::ScopedArray<six::UByte>& buffer)
{
    const size_t size = data.getNumRows() * data.getNumCols();
    buffer.reset(new six::UByte[size]);
    memset(buffer.get(), 1, size);
}

std::auto_ptr<six::Data> read(const std::string& filename)
{
    six::NITFReadControl reader;
    reader.load(filename);
    mem::SharedPtr<const six::Container> container = reader.getContainer();
    std::auto_ptr<six::Data> data(container->getData(0)->clone());
    return data;
}

void write(const six::Data& data, const std::string& pathname,
        const std::string& blockSize)

{
    std::auto_ptr<six::Data> firstData(data.clone());
    std::auto_ptr<six::Data> secondData(data.clone());

    firstData->setNumRows(11);
    firstData->setNumCols(11);
    secondData->setNumRows(9);
    secondData->setNumCols(9);

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

    six::NITFWriteControl writer;
    writer.getOptions().setParameter(
            six::NITFWriteControl::OPT_NUM_ROWS_PER_BLOCK, blockSize);
    writer.getOptions().setParameter(
            six::NITFWriteControl::OPT_NUM_COLS_PER_BLOCK, blockSize);
    writer.getOptions().setParameter(
            six::NITFWriteControl::OPT_MAX_PRODUCT_SIZE, "110");
    writer.initialize(container);
    writer.save(buffers, pathname, std::vector<std::string>());
}

void assignBuffer(std::pair<mem::ScopedArray<six::UByte>, size_t>& buffer,
        size_t index, six::NITFReadControl& reader)
{
    six::Region region;
    region.setStartRow(0);
    region.setStartCol(0);
    buffer.first.reset(reader.interleaved(region, index));
    buffer.second = region.getNumRows() * region.getNumCols();
}

bool compare(const std::string& lhs, const std::string& rhs)
{
    six::NITFReadControl firstReader;
    firstReader.load(lhs);

    std::pair<mem::ScopedArray<six::UByte>, size_t> firstBuffers[2];
    assignBuffer(firstBuffers[0], 0, firstReader);
    assignBuffer(firstBuffers[1], 1, firstReader);

    six::NITFReadControl secondReader;
    secondReader.load(rhs);

    std::pair<mem::ScopedArray<six::UByte>, size_t> secondBuffers[2];
    assignBuffer(secondBuffers[0], 0, secondReader);
    assignBuffer(secondBuffers[1], 1, secondReader);

    if (firstBuffers[0].second != secondBuffers[0].second ||
        firstBuffers[1].second != secondBuffers[1].second)
    {
        std::cerr << "Buffer lengths do not match\n";
        return false;
    }

    for (size_t ii = 0; ii < firstBuffers[0].second; ++ii)
    {
        if (firstBuffers[0].first[ii] != secondBuffers[0].first[ii])
        {
            std::cerr << "Buffer mismatch at index " << ii << std::endl;
            return false;
        }
    }

    for (size_t ii = 0; ii < firstBuffers[1].second; ++ii)
    {
        if (firstBuffers[1].first[ii] != secondBuffers[1].first[ii])
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
                six::DataType::COMPLEX,
                new six::XMLControlCreatorT<
                six::sicd::ComplexXMLControl>());
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
        write(*sidd, "firstTest.nitf", "10");
        write(*sidd, "secondTest.nitf", "15");
        compare("firstTest.nitf", "secondTest.nitf");
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
