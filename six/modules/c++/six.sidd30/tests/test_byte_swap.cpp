/* =========================================================================
* This file is part of six.sidd30-c++
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

#include <import/six/sidd30.h>
#include "six/NITFWriteControl.h"
#include "six/Types.h"


namespace
{
static const std::string OUTPUT_NAME("ByteSwapTest");
static const size_t DATA_LENGTH = 100;
static const size_t DATA_SIZE_IN_BYTES = DATA_LENGTH * sizeof(int16_t) / sizeof(std::byte);
static const size_t BYTES_PER_PIXEL = sizeof(int16_t);

void generateData(std::unique_ptr<int16_t[]>& data)
{
    for (size_t ii = 0; ii < DATA_LENGTH; ++ii)
    {
        data[ii] = int16_t(static_cast<int16_t>(ii));
    }
}

std::unique_ptr<six::sidd30::DerivedData> createData()
{
    std::unique_ptr<six::sidd30::DerivedData> derivedData(new six::sidd30::DerivedData());
    derivedData->productCreation.reset(new six::sidd30::ProductCreation());
    derivedData->productCreation->classification.classification = "U";
    derivedData->measurement.reset(new six::sidd30::Measurement(six::ProjectionType::PLANE));
    six::sidd30::PlaneProjection* planeProjection =
        (six::sidd30::PlaneProjection*) derivedData->measurement->projection.get();
    planeProjection->timeCOAPoly = six::Poly2D(0, 0);
    planeProjection->timeCOAPoly[0][0] = 1;
    planeProjection->productPlane.rowUnitVector = six::Vector3(0.0);
    planeProjection->productPlane.colUnitVector = six::Vector3(0.0);
    derivedData->measurement->arpPoly = six::PolyXYZ(0);
    derivedData->measurement->arpPoly[0] = six::Vector3(0.0);
    derivedData->display.reset(new six::sidd30::Display());
    derivedData->display->pixelType = six::PixelType::MONO16I;
    derivedData->setNumRows(10);
    derivedData->setNumCols(40);
    derivedData->geographicAndTarget.reset(new six::sidd30::GeographicAndTarget());
    derivedData->geographicAndTarget->geographicCoverage.reset(
            new six::sidd30::GeographicCoverage(six::RegionType::GEOGRAPHIC_INFO));
    // Set image corners
    for (size_t ii = 0; ii < 4; ++ii)
    {
        derivedData->geographicAndTarget->geographicCoverage->footprint.
                getCorner(ii).setLat(0);
        derivedData->geographicAndTarget->geographicCoverage->footprint.
            getCorner(ii).setLon(0);
    }

    // Set image corners
    for (size_t ii = 0; ii < 4; ++ii)
    {
        derivedData->geographicAndTarget->geographicCoverage->footprint.
                getCorner(ii).setLat(0);
        derivedData->geographicAndTarget->geographicCoverage->footprint.
            getCorner(ii).setLon(0);
    }

    derivedData->exploitationFeatures.reset(new six::sidd30::ExploitationFeatures());
    derivedData->exploitationFeatures->product.resize(1);
    derivedData->exploitationFeatures->product[0].resolution.row = 0;
    derivedData->exploitationFeatures->product[0].resolution.col = 0;
    derivedData->exploitationFeatures->collections.push_back(mem::ScopedCopyablePtr<six::sidd30::Collection>());
    derivedData->exploitationFeatures->collections[0].reset(new six::sidd30::Collection());

    six::sidd30::Collection* parent =
        derivedData->exploitationFeatures->collections[0].get();
    parent->information.resolution.rg = 0;
    parent->information.resolution.az = 0;
    parent->information.collectionDuration = 0;
    parent->information.collectionDateTime = six::DateTime();
    parent->information.radarMode = six::RadarModeType::SPOTLIGHT;
    parent->information.sensorName.clear();
    parent->geometry.reset(new six::sidd30::Geometry());

    derivedData->setNumRows(DATA_LENGTH / 10);
    derivedData->setNumCols(DATA_LENGTH / derivedData->getNumRows());

    return derivedData;
}

void write(const int16_t* data, bool useStream, bool byteSwap)
{
    mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::DERIVED));
    container->addData(createData().release());

    six::Options options;
    options.setParameter(six::WriteControl::OPT_BYTE_SWAP, static_cast<int>(byteSwap));
    six::NITFWriteControl writer(options, container);

    if (useStream)
    {
        io::ByteStream stream;
        stream.write(reinterpret_cast<const std::byte*>(data),
            DATA_SIZE_IN_BYTES / sizeof(std::byte));
        stream.seek(0, io::Seekable::START);
        std::vector<io::InputStream*> streams;
        streams.push_back(&stream);
        writer.save(streams, OUTPUT_NAME);
    }
    else
    {
        writer.save(reinterpret_cast<const std::byte*>(data), OUTPUT_NAME);
    }
}

void read(const std::string& filename, int16_t* data)
{
    six::NITFReadControl reader;
    reader.load(filename);

    six::Region region;
    region.setBuffer(reinterpret_cast<std::byte*>(data));
    reader.interleaved(region, 0);
}

bool run(bool useStream = false, bool byteswap = false)
{
    std::unique_ptr<int16_t[]> imageData(new int16_t[DATA_LENGTH]);
    generateData(imageData);

    std::unique_ptr<int16_t[]> testData(new int16_t[DATA_LENGTH]);
    memcpy(testData.get(), imageData.get(), DATA_SIZE_IN_BYTES);

    if ( ((std::endian::native == std::endian::little) && !byteswap) ||
        ((std::endian::native == std::endian::big) && byteswap) )
    {
        sys::byteSwap(reinterpret_cast<std::byte*>(imageData.get()),
                BYTES_PER_PIXEL, DATA_LENGTH);
    }
    write(testData.get(), useStream, byteswap);
    read(OUTPUT_NAME, testData.get());

    if (memcmp(testData.get(), imageData.get(), DATA_SIZE_IN_BYTES) == 0)
    {
        return true;
    }
    else
    {
        std::cerr << "Data doesn't match. Test failed." << std::endl;
        return false;
    }
}
}

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        six::XMLControlFactory::getInstance().addCreator<six::sidd30::DerivedXMLControl>();

        bool success = run(false, false) && run(true, false) &&
            run(false, true) && run(true, true);
        if (success)
        {
            std::cout << "All tests passed." << std::endl;
            return 0;
        }
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
