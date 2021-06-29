/* =========================================================================
* This file is part of six-c++
* =========================================================================
*
* (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <std/filesystem>

#include <import/six.h>
#include <import/six/sidd.h>
#include <import/nitf.hpp>

namespace fs = std::filesystem;

namespace
{
void validateArguments(int argc, char** argv)
{
    if (argc != 2)
    {
        std::string message = "Usage: " + fs::path(argv[0]).filename().string()
            + " <SIDD pathname>";
        throw except::Exception(Ctxt(message));
    }
    if (!str::endsWith(argv[1], ".ntf") &&
        !str::endsWith(argv[1], ".nitf"))
    {
        std::string message = "Extension suggests input may not be a SIDD file"
            "\nExpecting '*.nitf'.";
        throw except::Exception(Ctxt(message));
    }
}

struct Buffers final
{
    std::byte* add(size_t numBytes)
    {
        mBuffers.push_back(std::unique_ptr<std::byte[]>(new std::byte[numBytes]));
        return mBuffers.back().get();
    }

    std::vector<std::byte*> get() const
    {
        std::vector<std::byte*> retval;
        for (auto& buffer : mBuffers)
        {
            retval.push_back(buffer.get());
        }
        return retval;
    }

private:
    std::vector<std::unique_ptr<std::byte[]>> mBuffers;
};

std::string doRoundTrip(const std::string& siddPathname)
{
    six::XMLControlRegistry xmlRegistry;

    xmlRegistry.addCreator(six::DataType::DERIVED,
        new six::XMLControlCreatorT<
        six::sidd::DerivedXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    reader.load(siddPathname);
    auto container = reader.getContainer();

    six::Region region;

    Buffers buffers;
    for (size_t ii = 0, imageNum = 0; ii < container->getNumData(); ++ii)
    {
        six::Data* const data = container->getData(ii);

        if (container->getDataType() == six::DataType::COMPLEX ||
            data->getDataType() == six::DataType::DERIVED)
        {
            const auto extent = getExtent(*data);
            const size_t numPixels(extent.area());

            size_t numBytesPerPixel = data->getNumBytesPerPixel();

            std::byte* buffer = buffers.add(numPixels * numBytesPerPixel);

            setDims(region, extent);
            region.setBuffer(buffer);
            reader.interleaved(region, imageNum++);
        }
    }

    six::NITFWriteControl writer;
    writer.initialize(container);
    writer.setXMLControlRegistry(&xmlRegistry);
    writer.save(buffers.get(), "out.nitf");
    return "out.nitf";
}

nitf::LookupTable readLookupTable(const std::string& pathname)
{
    six::XMLControlRegistry xmlRegistry;

    xmlRegistry.addCreator(six::DataType::DERIVED,
        new six::XMLControlCreatorT<
        six::sidd::DerivedXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    reader.load(pathname);
    nitf::Record record = reader.getRecord();

    nitf::List images = record.getImages();
    nitf::ListIterator imageIter = images.begin();

    nitf::ImageSegment segment = (nitf::ImageSegment) * imageIter;
    nitf::ImageSubheader subheader = segment.getSubheader();

    const nitf::LookupTable& localTable = subheader.getBandInfo(0).getLookupTable();
    return nitf::LookupTable(localTable.getTable(), localTable.getTables(),
            localTable.getEntries());
}

bool operator==(const nitf::LookupTable& lhs, const nitf::LookupTable& rhs)
{
    if ((lhs.getTable() == nullptr && rhs.getTable() != nullptr) ||
            (lhs.getTable() != nullptr && rhs.getTable() == nullptr))
    {
        return false;
    }

    bool isEqual = (lhs.getTables() == rhs.getTables() &&
        lhs.getEntries() == rhs.getEntries());

    if (lhs.getTable() == nullptr && rhs.getTable() == nullptr)
    {
        isEqual = isEqual && true;
    }
    else
    {
        isEqual = isEqual &&  std::equal(lhs.getTable(),
            lhs.getTable() + sizeof lhs.getTable() / sizeof *lhs.getTable(),
            rhs.getTable());
    }
    return isEqual;

}

bool operator!=(const nitf::LookupTable& lhs, const nitf::LookupTable& rhs)
{
    return !(lhs == rhs);
}

mem::ScopedCopyablePtr<six::LUT> readLUT(const std::string& pathname)
{
    six::XMLControlRegistry xmlRegistry;

    xmlRegistry.addCreator(six::DataType::DERIVED,
        new six::XMLControlCreatorT<
        six::sidd::DerivedXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    reader.load(pathname);
    auto container = reader.getContainer();
    six::Data* const data = container->getData(0);
    mem::ScopedCopyablePtr<six::LUT> lut = data->getDisplayLUT();
    if (lut.get() == nullptr)
    {
        return mem::ScopedCopyablePtr<six::LUT>();
    }
    else
    {
        return mem::ScopedCopyablePtr<six::LUT>(lut->clone());
    }
}
}

int main(int argc, char** argv)
{
    try
    {
        validateArguments(argc, argv);
        const std::string siddPathname(argv[1]);
        const std::string roundTrippedPathname = doRoundTrip(siddPathname);

        nitf::LookupTable originalTable = readLookupTable(siddPathname);
        nitf::LookupTable roundTrippedTable =
                readLookupTable(roundTrippedPathname);

        if (originalTable != roundTrippedTable)
        {
            std::cerr <<
                "Round-tripped nitf::LookupTable differs from original\n";
            return 1;
        }

        mem::ScopedCopyablePtr<six::LUT> originalLUT = readLUT(siddPathname);
        mem::ScopedCopyablePtr<six::LUT> roundTrippedLUT =
                readLUT(roundTrippedPathname);
        if (!(originalLUT == roundTrippedLUT))
        {
            std::cerr <<
                "Round-tripped six::LUT differs from original\n";
            return 1;
        }

        std::cout << "Round-trip succeeded\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
