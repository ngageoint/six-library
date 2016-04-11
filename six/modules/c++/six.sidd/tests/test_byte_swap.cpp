/* =========================================================================
* This file is part of six.sidd-c++
* =========================================================================
*
* (C) Copyright 2004 - 2016, MDA Information Systems LLC
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
#include "six/NITFWriteControl.h"
#include "six/Types.h"

namespace
{
static const std::string OUTPUT_NAME("ByteSwapTest");
static const size_t DATA_LENGTH = 100;
static const size_t DATA_SIZE_IN_BYTES = DATA_LENGTH * sizeof(sys::Int16_T) / sizeof(six::UByte);
static const size_t BYTES_PER_PIXEL = 2;

void generateData(sys::Int16_T* data)
{
    for (size_t ii = 0; ii < DATA_LENGTH; ++ii)
    {
        data[ii] = sys::Int16_T(static_cast<sys::Int16_T>(ii));
    }
}

std::auto_ptr<six::sidd::DerivedData> createData()
{
    std::auto_ptr<six::sidd::DerivedData> derivedData(new six::sidd::DerivedData());
    derivedData->productCreation.reset(new six::sidd::ProductCreation());
    derivedData->productCreation->classification.classification = "C";
    derivedData->measurement.reset(new six::sidd::Measurement(six::ProjectionType::PLANE));
    six::sidd::PlaneProjection* planeProjection =
        (six::sidd::PlaneProjection*) derivedData->measurement->projection.get();
    planeProjection->timeCOAPoly = six::Poly2D(0, 0);
    planeProjection->timeCOAPoly[0][0] = 1;
    planeProjection->productPlane.rowUnitVector = six::Vector3(0.0);
    planeProjection->productPlane.colUnitVector = six::Vector3(0.0);
    derivedData->measurement->arpPoly = six::PolyXYZ(0);
    derivedData->measurement->arpPoly[0] = six::Vector3(0.0);
    derivedData->display.reset(new six::sidd::Display());
    derivedData->display->pixelType = six::PixelType::MONO16I;
    derivedData->setNumRows(10);
    derivedData->setNumCols(40);
    derivedData->geographicAndTarget.reset(new six::sidd::GeographicAndTarget(six::RegionType::GEOGRAPHIC_INFO));

    derivedData->exploitationFeatures.reset(new six::sidd::ExploitationFeatures());
    derivedData->exploitationFeatures->product.resolution.row = 0;
    derivedData->exploitationFeatures->product.resolution.col = 0;
    derivedData->exploitationFeatures->collections.push_back(mem::ScopedCloneablePtr<six::sidd::Collection>());
    derivedData->exploitationFeatures->collections[0].reset(new six::sidd::Collection());

    six::sidd::Collection* parent =
        derivedData->exploitationFeatures->collections[0].get();
    parent->information->resolution.rg = 0;
    parent->information->resolution.az = 0;
    parent->information->collectionDuration = 0;
    parent->information->collectionDateTime = six::DateTime();
    parent->information->radarMode = six::RadarModeType::SPOTLIGHT;
    parent->information->sensorName.clear();
    parent->geometry.reset(new six::sidd::Geometry());

    return derivedData;
}

void write(six::UByte* data, bool useStream, bool byteSwap)
{
    six::Container container(six::DataType::DERIVED);
    container.addData(createData());

    six::NITFWriteControl writer;
    writer.getOptions().setParameter(six::WriteControl::OPT_BYTE_SWAP, static_cast<int>(byteSwap));
    writer.initialize(&container);

    if (useStream)
    {
        io::ByteStream stream;
        stream.write(reinterpret_cast<sys::byte*>(data), DATA_SIZE_IN_BYTES);
        stream.seek(0, io::Seekable::START);
        std::vector<io::InputStream*> streams;
        streams.push_back(&stream);
        writer.save(streams, OUTPUT_NAME);
    }
    else
    {
        writer.save(data, OUTPUT_NAME);
    }
}

void read(const std::string& filename, six::UByte* data)
{

    six::NITFReadControl reader;
    reader.load(filename);
    six::Container* const container = reader.getContainer();
    six::Data* const inData = container->getData(0);

    const size_t numRows = inData->getNumRows();
    const size_t numCols = inData->getNumCols();

    six::Region region;
    region.setStartRow(0);
    region.setStartCol(0);
    region.setNumRows(numRows);
    region.setNumCols(numCols);

    memcpy(data, reader.interleaved(region, 0), DATA_SIZE_IN_BYTES);
    return;
}

bool run(bool useStream = false, bool byteswap = false)
{
    sys::Int16_T imageData[DATA_LENGTH];
    generateData(imageData);

    six::UByte* sourceData = reinterpret_cast<six::UByte*>(&imageData[0]);
    six::UByte testData[DATA_SIZE_IN_BYTES];
    memcpy(testData, sourceData, DATA_SIZE_IN_BYTES);

    if ((!sys::isBigEndianSystem() && !byteswap) || (sys::isBigEndianSystem() && byteswap))
    {
        sys::byteSwap(sourceData, BYTES_PER_PIXEL, DATA_SIZE_IN_BYTES / BYTES_PER_PIXEL);
    }

    write(testData, useStream, byteswap);
    read(OUTPUT_NAME, testData);

    for (size_t ii = 0; ii < DATA_SIZE_IN_BYTES; ++ii)
    {
        if (sourceData[ii] != testData[ii])
        {
            std::cerr << "Data doesn't match. Test failed." << std::endl;
            return false;
        }
    }
    return true;
}
}

int main(int argc, char** argv)
{
    six::XMLControlFactory::getInstance().addCreator(
        six::DataType::DERIVED,
        new six::XMLControlCreatorT<
        six::sidd::DerivedXMLControl>());

    try
    {
        bool success = run(false, false) && run(true, false) &&
            run(false, true) && run(false, false);

        return (success ? 0 : 1);
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
}
