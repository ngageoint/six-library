/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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

#include <six/Utilities.h>
#include <six/NITFReadControl.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

namespace
{
void getErrors(const six::sicd::ComplexData& data,
               scene::Errors& errors)
{
    six::getErrors(data.errorStatistics.get(),
                   types::RgAz<double>(data.grid->row->sampleSpacing,
                                       data.grid->col->sampleSpacing),
                   errors);
}
}

namespace six
{
namespace sicd
{
scene::SceneGeometry*
Utilities::getSceneGeometry(const ComplexData* data)
{
    scene::SceneGeometry *geom =
            new scene::SceneGeometry(data->scpcoa->arpVel,
                                     data->scpcoa->arpPos,
                                     data->geoData->scp.ecf,
                                     data->grid->row->unitVector,
                                     data->grid->col->unitVector);

    return geom;
}

scene::ProjectionModel*
Utilities::getProjectionModel(const ComplexData* data,
                              const scene::SceneGeometry* geom)
{
    const six::ComplexImageGridType gridType = data->grid->type;
    const int lookDir = (data->scpcoa->sideOfTrack == 1) ? 1 : -1;

    // Parse error statistics so we can pass these to the projection model as
    // well
    scene::Errors errors;
    ::getErrors(*data, errors);

    switch ((int) gridType)
    {
    case six::ComplexImageGridType::RGAZIM:
        return new scene::RangeAzimProjectionModel(
                                                   data->pfa->polarAnglePoly,
                                                   data->pfa->spatialFrequencyScaleFactorPoly,
                                                   geom->getSlantPlaneZ(),
                                                   data->grid->row->unitVector,
                                                   data->grid->col->unitVector,
                                                   data->geoData->scp.ecf,
                                                   data->position->arpPoly,
                                                   data->grid->timeCOAPoly,
                                                   lookDir,
                                                   errors);

    case six::ComplexImageGridType::RGZERO:
        return new scene::RangeZeroProjectionModel(
                                                   data->rma->inca->timeCAPoly,
                                                   data->rma->inca->dopplerRateScaleFactorPoly,
                                                   data->rma->inca->rangeCA,
                                                   geom->getSlantPlaneZ(),
                                                   data->grid->row->unitVector,
                                                   data->grid->col->unitVector,
                                                   data->geoData->scp.ecf,
                                                   data->position->arpPoly,
                                                   data->grid->timeCOAPoly,
                                                   lookDir,
                                                   errors);
    case six::ComplexImageGridType::XRGYCR:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XRGYCRProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir,
                                                errors);
    case six::ComplexImageGridType::XCTYAT:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XCTYATProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir,
                                                errors);
    case six::ComplexImageGridType::PLANE:
        // Note: This case has not been tested due to a lack of test data
        return new scene::PlaneProjectionModel(geom->getSlantPlaneZ(),
                                               data->grid->row->unitVector,
                                               data->grid->col->unitVector,
                                               data->geoData->scp.ecf,
                                               data->position->arpPoly,
                                               data->grid->timeCOAPoly,
                                               lookDir,
                                               errors);
    default:
        throw except::Exception(Ctxt("Invalid grid type: " +
                gridType.toString()));
    }
}

void six::sicd::Utilities::readSicd(const std::string& sicdPathname,
             const std::vector<std::string>& schemaPaths,
             std::auto_ptr<ComplexData>& complexData,
             std::vector<std::complex<float> >& widebandData)
{
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
            new six::XMLControlCreatorT<
            six::sicd::ComplexXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);
    reader.load(sicdPathname, schemaPaths); 

    getComplexData(reader, complexData);
    getWidebandData(reader, *(complexData.get()), widebandData);

    // This tells the reader that it doesn't
    // own an XMLControlRegistry
    reader.setXMLControlRegistry(NULL); 

}

void six::sicd::Utilities::getComplexData(
        NITFReadControl& reader,
        std::auto_ptr<ComplexData>&  complexData)
{
    six::Data* data = reader.getContainer()->getData(0);
    if(data->getDataType() != six::DataType::COMPLEX)
    {
        throw except::Exception(Ctxt(data->getName() + " is not a SICD"));
    }

    // Container owns data currently so we need to clone it
    complexData.reset(
        (reinterpret_cast<six::sicd::ComplexData*>(data->clone())));

}

namespace 
{
template <typename ValueType>
six::Region buildRegion(size_t firstRow, size_t firstCol, 
                        size_t numRows, size_t numCols,
                        ValueType* buffer)
{
    six::Region retv;
    retv.setStartRow(firstRow);
    retv.setStartCol(firstCol);
    retv.setNumRows(numRows);
    retv.setNumCols(numCols);
    retv.setBuffer(reinterpret_cast<six::UByte*>(buffer));
    return retv;
}
}

void six::sicd::Utilities::getWidebandData(
        NITFReadControl& reader,
        const ComplexData& complexData,
        std::complex<float>* buffer
        )
{
    const PixelType pixelType = complexData.getPixelType();
    const size_t imageNumber = 0;
    const size_t startRow = 0;
    const size_t startCol = 0;
    const size_t numRows = complexData.getNumRows();
    const size_t numCols = complexData.getNumCols();

    const size_t requiredBufferBytes = sizeof(std::complex<float>) 
                                            * numRows * numCols;
    

    if(!buffer)
    {
        // for some reason we don't have a buffer

        throw except::Exception(Ctxt("Null buffer provided to getWidebandData"
                    + std::string(" when a ")
                    + str::toString(requiredBufferBytes)
                    + std::string(" byte buffer was expected")));
    }
    
    if(pixelType == PixelType::RE32F_IM32F)
    {
        six::Region region = buildRegion(startRow, startCol,
                numRows, numCols, buffer);
        reader.interleaved(region, imageNumber);
    }
    else if(pixelType == PixelType::RE16I_IM16I)
    {
        const size_t elementsPerRow = numCols*2;
        const size_t rowsAtATime = 
            (32000000 / (elementsPerRow * sizeof(short))) + 1;
        std::vector<short> tempVector(elementsPerRow * rowsAtATime);
        short* const tempBuffer = &tempVector[0];

        for(size_t row = startRow, rowsToRead = rowsAtATime; 
                row < numRows; 
                row += rowsToRead)
        {
            if(row + rowsToRead > numRows)
            {
                rowsToRead = numRows - row;
            }

            six::Region region = buildRegion(row, startCol,
                    rowsToRead, numCols, tempBuffer);
            reader.interleaved(region, imageNumber);

            //Take each Int16 out of the temp buffer and put it into the real buffer as a Float32
            float* bufferPtr = reinterpret_cast<float*>(buffer) + (row * elementsPerRow);
            for(size_t index = 0; index < elementsPerRow * rowsToRead; index++)
            {
                bufferPtr[index] = tempBuffer[index];
            }
        }
    }
    else
    {
        throw except::Exception(Ctxt(complexData.getName() + " has an unknown pixel type"));
    }

}

void Utilities::getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                std::vector<std::complex<float> >& buffer)
{
    size_t requiredNumElements = complexData.getNumCols() * complexData.getNumRows();

    if(0 == requiredNumElements)
    {
        buffer.clear();
        return;
    }

    buffer.resize(requiredNumElements);    
    Utilities::getWidebandData(reader,
            complexData,
            &buffer[0]);
}

//
// Old interface below
//

std::auto_ptr<ComplexData> Utilities::getComplexData(
        const std::string& sicdPathname,
        const std::vector<std::string>& schemaPaths)
{
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
                           new six::XMLControlCreatorT<
                                   six::sicd::ComplexXMLControl>());

    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);
    reader.load(sicdPathname, schemaPaths);

    six::Data* const data = reader.getContainer()->getData(0);
    if (data->getDataType() != six::DataType::COMPLEX)
    {
        throw except::Exception(Ctxt(sicdPathname + " is not a SICD"));
    }

    // Now that we know it's a SICD we can safely cast to a ComplexData
    // We can't just return a pointer to the data though because it's owned by
    // the container which is owned by the reader (and the reader will go out
    // of scope when this function returns), so we need to clone it.
    // Note that you don't have to do this yourself if in your usage the
    // reader stays in scope.
    // TODO: If the container held shared pointers we wouldn't need to do this
    std::auto_ptr<ComplexData> complexData(
            reinterpret_cast<six::sicd::ComplexData*>(data->clone()));
    return complexData;
}

void Utilities::getWidebandData(
        const std::string& sicdPathname,
        const std::vector<std::string>& schemaPaths,
        const ComplexData& complexData,
        std::complex<float>* buffer)
{
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX,
                           new six::XMLControlCreatorT<
                                   six::sicd::ComplexXMLControl>());
    six::NITFReadControl reader;
    reader.setXMLControlRegistry(&xmlRegistry);
    reader.load(sicdPathname);

    const PixelType pixelType = complexData.getPixelType();
    const size_t imageNumber = 0;
    const size_t startRow = 0;
    const size_t startCol = 0;
    const size_t numRows = complexData.getNumRows();
    const size_t numCols = complexData.getNumCols();

    if (pixelType == PixelType::RE32F_IM32F)
    {
        six::Region region = buildRegion(startRow, startCol,
                numRows, numCols, buffer);
        reader.interleaved(region, imageNumber);
    }
    else if (pixelType == PixelType::RE16I_IM16I)
    {
        // One for the real component, one for imaginary of each pixel
        const size_t elementsPerRow = numCols * 2;

        // Get at least 32MB per read
        const size_t rowsAtATime = (32000000 / (elementsPerRow * sizeof(short))) + 1;

        // Allocate temp buffer
        std::vector<short> tempVector(elementsPerRow * rowsAtATime);
        short* const tempBuffer = &tempVector[0];

        for (size_t row = startRow, rowsToRead = rowsAtATime; row < numRows; row += rowsToRead)
        {
            // If we would read beyond the input buffer, don't
            if (row + rowsToRead > numRows)
            {
                rowsToRead = numRows - row;
            }

            // Read into the temp buffer
            six::Region region = buildRegion(row, startCol,
                    rowsToRead, numCols, tempBuffer);
            reader.interleaved(region, imageNumber);

            //Take each Int16 out of the temp buffer and put it into the real buffer as a Float32
            float* bufferPtr = reinterpret_cast<float*>(buffer) + (row * elementsPerRow);
            for(size_t index = 0; index < elementsPerRow * rowsToRead; index++)
            {
                bufferPtr[index] = tempBuffer[index];
            }
        }
    }
    else
    {
        throw except::Exception(Ctxt(sicdPathname + " has an unknown pixel type"));
    }
}

}
}

