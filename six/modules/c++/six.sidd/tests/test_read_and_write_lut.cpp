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
#include <import/six.h>
#include <import/six/sidd.h>
#include <import/nitf.hpp>

namespace
{
void validateArguments(int argc, char** argv)
{
    if (argc != 2)
    {
        std::string message = "Usage: " + sys::Path::basename(argv[0])
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

class Buffers
{
public:
    Buffers()
    {
    }

    ~Buffers()
    {
        for (size_t ii = 0; ii < mBuffers.size(); ++ii)
        {
            delete[] mBuffers[ii];
        }
    }

    sys::ubyte* add(size_t numBytes)
    {
        mem::ScopedArray<sys::ubyte> buffer(new sys::ubyte[numBytes]);
        mBuffers.push_back(buffer.get());
        return buffer.release();
    }

    std::vector<sys::ubyte*> get() const
    {
        return mBuffers;
    }

private:
    std::vector<sys::ubyte*> mBuffers;
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
    six::Container* container = reader.getContainer();

    six::Region region;
    region.setStartRow(0);
    region.setStartCol(0);

    Buffers buffers;
    for (size_t ii = 0, imageNum = 0; ii < container->getNumData(); ++ii)
    {
        six::Data* const data = container->getData(ii);

        if (container->getDataType() == six::DataType::COMPLEX ||
            data->getDataType() == six::DataType::DERIVED)
        {
            const types::RowCol<size_t> extent(data->getNumRows(),
                data->getNumCols());
            const size_t numPixels(extent.row * extent.col);

            size_t numBytesPerPixel = data->getNumBytesPerPixel();
            size_t offset = 0;

            sys::ubyte* buffer =
                buffers.add(numPixels * numBytesPerPixel);

            region.setNumRows(extent.row);
            region.setNumCols(extent.col);
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
    return subheader.getBandInfo(0).getLookupTable();
}

bool operator==(const nitf::LookupTable& lhs, const nitf::LookupTable& rhs)
{
    std::cerr << "Called\n";
    if (lhs.getTable() == NULL)
    {
        std::cerr << "Null Table\n";
    }
    else
    {
        std::cerr << "Table okay\n";
    }
    return (lhs.getTables() == rhs.getTables() &&
        lhs.getEntries() == rhs.getEntries() &&
        std::equal(lhs.getTable(),
            lhs.getTable() + sizeof lhs.getTable() / sizeof *lhs.getTable(),
            rhs.getTable()));
}

bool operator!=(const nitf::LookupTable& lhs, const nitf::LookupTable& rhs)
{
    return !(lhs == rhs);
}

bool operator==(const six::LUT& lhs, const nitf::LookupTable& rhs)
{
    return (lhs.numEntries == rhs.getEntries() &&
        lhs.elementSize == rhs.getTables() &&
        std::equal(rhs.getTable(),
            rhs.getTable() + sizeof rhs.getTable() / sizeof *rhs.getTable(),
            lhs.getTable()));
}

bool operator!=(const six::LUT& lhs, const nitf::LookupTable& rhs)
{
    return !(lhs == rhs);
}

bool operator==(const nitf::LookupTable& lhs, const six::LUT& rhs)
{
    return rhs == lhs;
}

bool operator!=(const nitf::LookupTable& lhs, const six::LUT& rhs)
{
    return !(lhs == rhs);
}

six::LUT* readLUT(const std::string& pathname)
{
    six::XMLControlRegistry xmlRegistry;

    xmlRegistry.addCreator(six::DataType::DERIVED,
        new six::XMLControlCreatorT<
        six::sidd::DerivedXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);

    reader.load(pathname);
    six::Container* container = reader.getContainer();
    six::Data* const data = container->getData(0);
    return data->getDisplayLUT();
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

        six::LUT* originalLUT = readLUT(siddPathname);
        six::LUT* roundTrippedLUT = readLUT(roundTrippedPathname);

        if (!(*originalLUT == *roundTrippedLUT))
        {
            std::cerr <<
                "Round-tripped six::LUT differs from original\n";
            return 1;
        }

        return 0;
    }
    catch (const except::Exception& e)
    {
        std::cerr << e.getMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
