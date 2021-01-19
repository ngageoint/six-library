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
static const std::string OUTPUT_NAME("GeoTIFFTest");
static const size_t DATA_LENGTH = 100;
static const size_t DATA_SIZE_IN_BYTES = DATA_LENGTH * sizeof(sys::Int16_T) / sizeof(six::UByte);
static const size_t BYTES_PER_PIXEL = sizeof(sys::Int16_T);

void generateData(sys::Int16_T* data)
{
    for (size_t ii = 0; ii < DATA_LENGTH; ++ii)
    {
        data[ii] = static_cast<sys::Int16_T>(ii);
    }
}

std::auto_ptr<six::sidd::DerivedData> createData()
{
    std::auto_ptr<six::sidd::DerivedData> derivedData(new six::sidd::DerivedData());
    derivedData->productCreation.reset(new six::sidd::ProductCreation());
    derivedData->productCreation->classification.classification = "U";
    derivedData->measurement.reset(
            new six::sidd::Measurement(six::ProjectionType::GEOGRAPHIC));
    six::sidd::GeographicProjection* geographicProjection =
            (six::sidd::GeographicProjection*)
            derivedData->measurement->projection.get();
    geographicProjection->timeCOAPoly = six::Poly2D(0, 0);
    geographicProjection->timeCOAPoly[0][0] = 1;
    derivedData->measurement->arpPoly = six::PolyXYZ(0);
    derivedData->measurement->arpPoly[0] = six::Vector3(0.0);
    derivedData->display.reset(new six::sidd::Display());
    derivedData->display->pixelType = six::PixelType::MONO16I;
    derivedData->setNumRows(10);
    derivedData->setNumCols(40);
    derivedData->geographicAndTarget.reset(new six::sidd::GeographicAndTarget());
    derivedData->geographicAndTarget->geographicCoverage.reset(
            new six::sidd::GeographicCoverage(six::RegionType::GEOGRAPHIC_INFO));

    for (size_t ii = 0; ii < 4; ++ii)
    {
        derivedData->geographicAndTarget->geographicCoverage->footprint.
                getCorner(ii).setLat(0);
        derivedData->geographicAndTarget->geographicCoverage->footprint.
                getCorner(ii).setLon(0);
    }

    derivedData->exploitationFeatures.reset(new six::sidd::ExploitationFeatures());
    derivedData->exploitationFeatures->product.resize(1);
    derivedData->exploitationFeatures->product[0].resolution.row = 0;
    derivedData->exploitationFeatures->product[0].resolution.col = 0;
    derivedData->exploitationFeatures->collections.push_back(mem::ScopedCopyablePtr<six::sidd::Collection>());
    derivedData->exploitationFeatures->collections[0].reset(new six::sidd::Collection());

    six::sidd::Collection* parent =
        derivedData->exploitationFeatures->collections[0].get();
    parent->information.resolution.rg = 0;
    parent->information.resolution.az = 0;
    parent->information.collectionDuration = 0;
    parent->information.collectionDateTime = six::DateTime();
    parent->information.radarMode = six::RadarModeType::SPOTLIGHT;
    parent->information.sensorName.clear();
    parent->geometry.reset(new six::sidd::Geometry());

    derivedData->setNumRows(DATA_LENGTH / 10);
    derivedData->setNumCols(DATA_LENGTH / derivedData->getNumRows());

    return derivedData;
}

void write(const sys::Int16_T* data)
{
    mem::SharedPtr<six::Container> container(new six::Container(
            six::DataType::DERIVED));
    container->addData(createData().release());

    six::sidd::GeoTIFFWriteControl writer;
    writer.initialize(container);
    writer.save(reinterpret_cast<const six::UByte*>(data), OUTPUT_NAME);
}

void read(const std::string& filename, mem::ScopedArray<sys::Int16_T>& data)
{
    six::sidd::GeoTIFFReadControl reader;
    reader.load(filename);
    six::Region region;
    reader.interleaved(region, 0, data);
}

bool run()
{
    mem::ScopedArray<sys::Int16_T> imageData(new sys::Int16_T[DATA_LENGTH]);
    generateData(imageData.get());

    write(imageData.get());

    mem::ScopedArray<sys::Int16_T> testData;
    read(OUTPUT_NAME, testData);

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
        six::XMLControlFactory::getInstance().addCreator(
            six::DataType::DERIVED,
            new six::XMLControlCreatorT<
            six::sidd::DerivedXMLControl>());

        if (run())
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
