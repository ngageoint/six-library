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

#include <complex>
#include <iostream>
#include <iterator>
#include <import/six/sidd.h>
#include "six/NITFWriteControl.h"
#include "six/Types.h"

namespace
{
static const std::string OUTPUT_NAME("ByteSwapTest");
static const size_t DATA_LENGTH = 100;
static const size_t DATA_SIZE_IN_BYTES = DATA_LENGTH * sizeof(std::complex<float>) / sizeof(six::UByte);

void generateData(std::complex<float>* data)
{
    for (size_t ii = 0; ii < DATA_LENGTH; ++ii)
    {
        data[ii] = std::complex<float>((float)ii, (float)ii);
    }
}

six::sidd::DerivedData* createData()
{
    six::sidd::DerivedData* derivedData = new six::sidd::DerivedData();
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
    derivedData->display->pixelType = six::PixelType::RE32F_IM32F;
    derivedData->setNumRows(10);
    derivedData->setNumCols(10);
    derivedData->geographicAndTarget.reset(new six::sidd::GeographicAndTarget(six::RegionType::GEOGRAPHIC_INFO));
    //derivedData->geographicAndTarget->imageCorners.reset(new six::LatLonCorners());
    //derivedData->geographicAndTarget->geographicCoverage.reset(new six::sidd::GeographicCoverage(six::RegionType::GEOGRAPHIC_INFO));

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
    //parent->geometry->dopplerConeAngle = 45.8;

    derivedData->productCreation.reset(new six::sidd::ProductCreation());
    return derivedData;
}

void write(six::UByte* data, bool useStream, bool byteSwap)
{
    six::Container container(six::DataType::DERIVED);
    container.addData(createData());

    six::NITFWriteControl writer;
    writer.getOptions().setParameter(six::WriteControl::OPT_BYTE_SWAP, (int)byteSwap);
    writer.initialize(&container);

    if (useStream)
    {
        io::ByteStream stream;
        stream.read((sys::byte*)data, 800);
        std::vector<io::InputStream*> streams;
        streams.push_back(&stream);
        writer.save(streams, OUTPUT_NAME);
    }
    else
    {
        std::cerr << "Using buffer" << std::endl;
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
    six::UByte buffer[DATA_SIZE_IN_BYTES];
    region.setBuffer(buffer);
    six::UByte* result = reader.interleaved(region, 0);

    for (size_t ii = 0; ii < DATA_SIZE_IN_BYTES; ++ii)
    {
        data[ii] = result[ii];
    }
}
}

int main(int argc, char** argv)
{
    six::XMLControlFactory::getInstance().addCreator(
        six::DataType::DERIVED,
        new six::XMLControlCreatorT<
        six::sidd::DerivedXMLControl>());

    std::complex<float> sourceComplex[100];
    generateData(sourceComplex);
    six::UByte sourceData[DATA_SIZE_IN_BYTES];
    for (size_t ii = 0; ii < DATA_SIZE_IN_BYTES; ++ii)
    {
        sourceData[ii] = *(((six::UByte*)&sourceComplex[0]) + ii);
    }
    six::UByte testData[DATA_SIZE_IN_BYTES];
    std::copy(std::begin(sourceData), std::end(sourceData), std::begin(testData));

    write(testData, false, false);
    for (size_t ii = 0; ii <DATA_SIZE_IN_BYTES; ++ii)
    {
        testData[ii] = (six::UByte)0;
    }
    std::cerr << "Written" << std::endl;
    read(OUTPUT_NAME, testData);

    for (size_t ii = 0; ii < DATA_SIZE_IN_BYTES; ++ii)
    {
        //if (sourceData[ii] == testData[ii])
        //{
            std::cerr << (int)sourceData[ii] << " " << (int)testData[ii] << std::endl;
            //std::cerr << "Not equal" << std::endl;
        //}
    }

    return 0;
}
