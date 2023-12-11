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
#include "six/sicd/Utilities.h"

#include <math.h>
#include <assert.h>

#include <map>
#include <string>
#include <functional>
#include <memory>
#include <algorithm>
#include <iterator>
#include <utility>
#include <stdexcept>
#include <numeric>
#include <std/numbers>
#include <tuple>

#include <except/Exception.h>
#include <io/StringStream.h>
#include <math/Utilities.h>
#include <math/poly/Fit.h>
#include <mem/ScopedAlignedArray.h>
#include <gsl/gsl.h>
#include <str/Manip.h>
#include <str/Encoding.h>
#include <sys/Conf.h>
#include <sys/Span.h>
#include <types/RowCol.h>
#include <units/Angles.h>
#include <sys/Path.h>

#include <six/NITFReadControl.h>
#include <six/sicd/SICDWriteControl.h>
#include <six/Utilities.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/SICDMesh.h>
#include <six/sicd/AreaPlaneUtility.h>
#include <six/sicd/GeoLocator.h>
#include <six/sicd/ImageData.h>
#include <six/sicd/NITFReadComplexXMLControl.h>
#include <six/sicd/DataParser.h>
#include <six/sicd/Exports.h>

namespace fs = std::filesystem;

namespace
{
void getErrors(const six::sicd::ComplexData& data, scene::Errors& errors)
{
    six::getErrors(data.errorStatistics.get(),
                   types::RgAz<double>(data.grid->row->sampleSpacing,
                                       data.grid->col->sampleSpacing),
                   errors);
}

template <typename ValueType>
six::Region buildRegion(const types::RowCol<size_t>& offset,
                        const types::RowCol<size_t>& extent,
                        ValueType* buffer)
{
    six::Region retv;
    setOffset(retv, offset);
    setDims(retv, extent);
    void* buffer_ = buffer;
    retv.setBuffer(static_cast<std::byte*>(buffer_));
    return retv;
}
}

static auto toComplex_(double A, uint8_t phase)
{
    // The phase values should be read in (values 0 to 255) and converted to float by doing:
    // P = (1 / 256) * input_value
    const double P = (1.0 / 256.0) * phase;

    // To convert the amplitude and phase values to complex float (i.e. real and imaginary):
    // S = A * cos(2 * pi * P) + j * A * sin(2 * pi * P)
    const auto angle = units::Radians<double>{ 2 * std::numbers::pi * P };
    double sin_angle, cos_angle;
    SinCos(angle, sin_angle, cos_angle);
    six::zfloat S(gsl::narrow_cast<float>(A * cos_angle), gsl::narrow_cast<float>(A * sin_angle));
    return S;
}
six::zfloat six::sicd::Utilities::toComplex(uint8_t amplitude, uint8_t phase)
{   
    // A = input_amplitude(i.e. 0 to 255)
    const double A = amplitude;
    return toComplex_(A, phase);
}
six::zfloat six::sicd::Utilities::toComplex(uint8_t amplitude, uint8_t phase, const six::AmplitudeTable& amplitudeTable)
{
    const double A = amplitudeTable.index(amplitude);
    return toComplex_(A, phase);
}
six::zfloat six::sicd::Utilities::toComplex(uint8_t amplitude, uint8_t phase, const six::AmplitudeTable* pAmplitudeTable)
{
    if (pAmplitudeTable != nullptr)
    {
        return toComplex(amplitude, phase, *pAmplitudeTable);
    }
    else
    {
        return toComplex(amplitude, phase);
    }
}

static auto iota_0_256_()
{
    static_assert(sizeof(size_t) > sizeof(uint8_t), "size_t can't hold UINT8_MAX!");

    std::vector<uint8_t> retval;
    retval.reserve(UINT8_MAX + 1);
    for (size_t i = 0; i <= UINT8_MAX; i++) // Be careful with indexing so that we don't wrap-around in the loop.
    {
        retval.push_back(gsl::narrow<uint8_t>(i));
    }
    assert(retval.size() == UINT8_MAX + 1);
    return retval;
}
std::vector<uint8_t> six::sicd::Utilities::iota_0_256()
{
    static const auto retval = iota_0_256_();
    return retval;
}

namespace
{
// Reads in ~32 MB of rows at a time, converts to complex<float>, and keeps
// going until reads everything
template<typename T, typename TProcess>
static void SICDreader(six::NITFReadControl& reader, size_t imageNumber,
    const types::RowCol<size_t>& offset, const types::RowCol<size_t>& extent, size_t elementsPerRow,
    TProcess process)
{
    // Get at least 32MB per read
    const size_t rowsAtATime = (32000000 / (elementsPerRow * sizeof(T))) + 1;

    // Allocate temp buffer
    std::vector<T> tempVector(elementsPerRow * rowsAtATime);

    const size_t endRow = offset.row + extent.row;
    for (size_t row = offset.row, rowsToRead = rowsAtATime; row < endRow;
        row += rowsToRead)
    {
        // If we would read beyond the input buffer, don't
        if (row + rowsToRead > endRow)
        {
            rowsToRead = endRow - row;
        }

        // Read into the temp buffer
        const types::RowCol<size_t> swathOffset(row, offset.col);
        const types::RowCol<size_t> swathExtent(rowsToRead, extent.col);
        six::Region region = buildRegion(swathOffset, swathExtent, tempVector.data());
        reader.interleaved(region, imageNumber);

        process(elementsPerRow, row, rowsToRead, tempVector);
    }
}

// Reads in ~32 MB of rows at a time, converts to complex<float>, and keeps
// going until reads everything
template<typename T>
class SICD_readerAndConverter final
{
    void process(size_t elementsPerRow, size_t row, size_t rowsToRead, const std::vector<int16_t>& tempVector) const
    {
        process_RE16I_IM16I(elementsPerRow, row, rowsToRead, tempVector);
    }
    void process_RE16I_IM16I(size_t elementsPerRow, size_t row, size_t rowsToRead, const std::vector<int16_t>& tempVector) const
    {
        // Take each Int16 out of the temp buffer and put it into the real buffer as a Float32
        void* const pBuffer = buffer;
        float* const bufferPtr = static_cast<float*>(pBuffer) + ((row - offset.row) * elementsPerRow);
        for (size_t index = 0; index < elementsPerRow * rowsToRead; index++)
        {
            bufferPtr[index] = tempVector[index];
        }
    }

    void process(size_t elementsPerRow, size_t row, size_t rowsToRead, const std::vector<uint8_t>& tempVector) const
    {
        process_AMP8I_PHS8I(elementsPerRow, row, rowsToRead, tempVector);
    }
    void process_AMP8I_PHS8I(size_t elementsPerRow, size_t row, size_t rowsToRead, const std::vector<uint8_t>& tempVector) const
    {
        auto bufferPtr = buffer + ((row - offset.row) * (elementsPerRow / 2));

        // There's type mangling going on here.
        // We're taking std::vector<uint8_t> and saying it's packed with std::pair<uint8_t, uint8_t>.
        static_assert(sizeof(uint8_t) * 2 == sizeof(six::AMP8I_PHS8I_t), "expected packed layout in pair");
        auto packed = reinterpret_cast<const six::AMP8I_PHS8I_t*>(tempVector.data());

        // Reuse image data's conversion to complex.
        size_t count = (elementsPerRow * rowsToRead) / 2;
        auto const input = sys::make_span(packed, count);
        auto const output = sys::make_span(bufferPtr, input.size());
        six::sicd::ImageData::toComplex(lookup, input, output);
    }
    const types::RowCol<size_t>& offset;
    six::zfloat* buffer;
    const six::Amp8iPhs8iLookup_t& lookup;
    
public:
    SICD_readerAndConverter(six::NITFReadControl& reader, size_t imageNumber,
			    const types::RowCol<size_t>& offset, const types::RowCol<size_t>& extent,
                size_t elementsPerRow,
			    six::zfloat* buffer,  const six::AmplitudeTable* pAmplitudeTable = nullptr)
      : offset(offset), buffer(buffer), lookup(six::sicd::ImageData::getLookup(pAmplitudeTable))
    {
        SICDreader<T>(reader, imageNumber, offset, extent, elementsPerRow,
            [&](size_t elementsPerRow, size_t row, size_t rowsToRead, const std::vector<T>& tempVector)
            {
                process(elementsPerRow, row, rowsToRead, tempVector);
            });
    }
    SICD_readerAndConverter(const SICD_readerAndConverter&) = delete;
    SICD_readerAndConverter& operator=(const SICD_readerAndConverter&) = delete;
    SICD_readerAndConverter& operator=(SICD_readerAndConverter&&) = delete;
};

six::Poly2D getXYtoRowColTransform(double center,
                                   double sampleSpacing,
                                   bool rowTransform)
{
    const double coeffs[2] = {-sampleSpacing * center, sampleSpacing};
    const auto orderX = static_cast<size_t>(rowTransform ? 1 : 0);
    const auto orderY = static_cast<size_t>(rowTransform ? 0 : 1);
    return six::Poly2D(orderX, orderY, coeffs);
}

static std::map<std::string, size_t> getAdditionalDesMap(const six::NITFReadControl& reader)
{
    std::map<std::string, size_t> nameToDesIndex;
    const nitf::List des = reader.getRecord().getDataExtensions();
    nitf::ListIterator desIter = des.begin();

    for (size_t ii = 0; desIter != des.end(); ++desIter, ++ii)
    {
        // Skip the first DES. The SICD specification document
        // indicates that the first DES of the NITF 2.1 product is to
        // be populated by the XML metadata.
        if (ii == 0)
        {
            continue;
        }

        const nitf::DESegment segment(*desIter);
        const auto typeID = segment.getSubheader().typeID();
        nameToDesIndex[typeID] = ii;
    }
    return nameToDesIndex;
}

static void getDesBuffer(const six::NITFReadControl& reader,
                  size_t desIndex,
                  mem::ScopedAlignedArray<std::byte>& buffer)
{
    nitf::List des = reader.getRecord().getDataExtensions();

    if (desIndex >= des.getSize())
    {
        throw except::Exception(Ctxt("DES index out of range."));
    }

    // Pull out the DE segment and its reader
    const nitf::DESegment segment(des[desIndex]);
    const auto subheader = segment.getSubheader();
    nitf::SegmentReader deReader = reader.getReader().newDEReader(static_cast<int>(desIndex));

    // Read the DE segment buffer
    const size_t bufferSize = subheader.getDataLength();
    buffer.reset(bufferSize);
    deReader.read(buffer.get(), bufferSize);
}

template <typename MeshTypeT>
std::unique_ptr<MeshTypeT> extractMesh(const std::string& meshID,
                                     size_t desIndex,
                                     const six::NITFReadControl& reader)
{
    // Extract the mesh
    std::unique_ptr<MeshTypeT> mesh(new MeshTypeT(meshID));
    mem::ScopedAlignedArray<std::byte> buffer;
    getDesBuffer(reader, desIndex, buffer);

    const std::byte* bufferData = buffer.get();
    mesh->deserialize(bufferData);

    return mesh;
}
}

namespace six
{
namespace sicd
{
scene::SceneGeometry* Utilities::getSceneGeometry(const ComplexData* data)
{
    scene::SceneGeometry* geom =
            new scene::SceneGeometry(data->scpcoa->arpVel,
                                     data->scpcoa->arpPos,
                                     data->geoData->scp.ecf,
                                     data->grid->row->unitVector,
                                     data->grid->col->unitVector);

    return geom;
}

scene::ProjectionModel* Utilities::getProjectionModel(
        const ComplexData* data, const scene::SceneGeometry* geom)
{
    const six::ComplexImageGridType gridType = data->grid->type;
    const int lookDir = (data->scpcoa->sideOfTrack == 1) ? 1 : -1;

    // Parse error statistics so we can pass these to the projection model as
    // well
    scene::Errors errors;
    ::getErrors(*data, errors);

    switch ((int)gridType)
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
        throw except::Exception(
                Ctxt("Invalid grid type: " + gridType.toString()));
    }
}

template<typename TSceneGeometryPtr, typename TProjectionModelPtr>
static void getModelComponents_(const ComplexData& complexData,
    TSceneGeometryPtr& geometry,
    TProjectionModelPtr& projectionModel,
    AreaPlane& areaPlane)
{
    geometry.reset(Utilities::getSceneGeometry(&complexData));
    projectionModel.reset(Utilities::getProjectionModel(&complexData, geometry.get()));

    if (AreaPlaneUtility::hasAreaPlane(complexData))
    {
        areaPlane = *complexData.radarCollection->area->plane;
    }
    else
    {
        AreaPlaneUtility::deriveAreaPlane(complexData, areaPlane);
    }
}
void Utilities::getModelComponents(const ComplexData& complexData,
        std::unique_ptr<scene::SceneGeometry>& geometry,
        std::unique_ptr<scene::ProjectionModel>& projectionModel,
        AreaPlane& areaPlane)
{
    getModelComponents_(complexData, geometry, projectionModel, areaPlane);
}

std::unique_ptr<scene::ProjectionPolynomialFitter> Utilities::getPolynomialFitter(const ComplexData& complexData,
        size_t numPoints1D,
        bool sampleWithinValidDataPolygon)
{
    std::unique_ptr<scene::SceneGeometry> geometry;
    std::unique_ptr<scene::ProjectionModel> projectionModel;
    AreaPlane areaPlane;

    Utilities::getModelComponents(complexData,
                                  geometry,
                                  projectionModel,
                                  areaPlane);

    const RowColDouble sampleSpacing(areaPlane.xDirection->spacing,
                                     areaPlane.yDirection->spacing);
    const scene::PlanarGridECEFTransform ecefTransform(
            sampleSpacing,
            areaPlane.referencePoint.rowCol,
            areaPlane.xDirection->unitVector,
            areaPlane.yDirection->unitVector,
            areaPlane.referencePoint.ecef);

    types::RowCol<size_t> offset;
    types::RowCol<size_t> extent;
    complexData.getOutputPlaneOffsetAndExtent(areaPlane, offset, extent);

    if (!sampleWithinValidDataPolygon)
    {
        return std::unique_ptr<scene::ProjectionPolynomialFitter>(
                new scene::ProjectionPolynomialFitter(*projectionModel,
                                                      ecefTransform,
                                                      offset,
                                                      extent,
                                                      numPoints1D));
    }

    // Get the size of the output plane image.
    const types::RowCol<size_t> fullExtent(areaPlane.xDirection->elements,
                                     areaPlane.yDirection->elements);

    // Get the valid data polygon in the output plane.
    std::vector<types::RowCol<double>> polygon;
    Utilities::projectValidDataPolygonToOutputPlane(complexData, polygon);

    return std::unique_ptr<scene::ProjectionPolynomialFitter>(
            new scene::ProjectionPolynomialFitter(*projectionModel,
                                                  ecefTransform,
                                                  fullExtent,
                                                  offset,
                                                  extent,
                                                  polygon,
                                                  numPoints1D));
}

void Utilities::getValidDataPolygon(
        const ComplexData& sicdData,
        const scene::ProjectionModel& projection,
        std::vector<types::RowCol<double>>& validData)
{
    validData.clear();

    const std::vector<six::RowColInt>& origValidData =
            sicdData.imageData->validData;

    if (!origValidData.empty())
    {
        // Life is easy - we're just casting
        validData.resize(origValidData.size());
        for (size_t ii = 0; ii < origValidData.size(); ++ii)
        {
            validData[ii] = origValidData[ii];
        }
    }
    else if (sicdData.radarCollection->area.get() &&
             sicdData.radarCollection->area->plane.get())
    {
        // We have an output plane defined, so project the four output plane
        // corners back to the slant plane and that should approximate the
        // valid data polygon

        // Output plane corners
        types::RowCol<size_t> opOffset;
        types::RowCol<size_t> opDims;
        sicdData.getOutputPlaneOffsetAndExtent(opOffset, opDims);

        const types::RowCol<double> lastOPPixel(opOffset + opDims - 1);

        std::vector<types::RowCol<double>> opCorners(4);
        opCorners[0] = opOffset;
        opCorners[1] =
                types::RowCol<double>(static_cast<double>(opOffset.row),
                                      static_cast<double>(lastOPPixel.col));
        opCorners[2] =
                types::RowCol<double>(static_cast<double>(lastOPPixel.row),
                                      static_cast<double>(opOffset.col));
        opCorners[3] = lastOPPixel;

        const six::sicd::AreaPlane& areaPlane =
                *sicdData.radarCollection->area->plane;

        const six::sicd::AreaDirectionParameters& x(*areaPlane.xDirection);
        const six::sicd::AreaDirectionParameters& y(*areaPlane.yDirection);

        // Normalize the output plane unit vectors just in case
        six::Vector3 opRowUnitVector = x.unitVector;
        six::Vector3 opColUnitVector = y.unitVector;
        opRowUnitVector.normalize();
        opColUnitVector.normalize();

        const scene::PlanarGridECEFTransform gridTransform(
                types::RowCol<double>(x.spacing, y.spacing),
                areaPlane.getAdjustedReferencePoint(),
                opRowUnitVector,
                opColUnitVector,
                areaPlane.referencePoint.ecef);

        const types::RowCol<double> spSCP(sicdData.imageData->scpPixel);
        const types::RowCol<double> spOrigOffset(
                static_cast<double>(sicdData.imageData->firstRow),
                static_cast<double>(sicdData.imageData->firstCol));

        const types::RowCol<double> spOffset(spSCP.row - spOrigOffset.row,
                                             spSCP.col - spOrigOffset.col);

        const six::sicd::Grid& grid(*sicdData.grid);
        const types::RowCol<double> spSampleSpacing(grid.row->sampleSpacing,
                                                    grid.col->sampleSpacing);

        validData.resize(opCorners.size());
        bool spCornerIsInBounds = false;
        for (size_t ii = 0; ii < opCorners.size(); ++ii)
        {
            // Convert OP pixel to ECEF
            const scene::Vector3 sPos =
                    gridTransform.rowColToECEF(opCorners[ii]);

            // Convert this to a continuous surface point in the slant plane
            double timeCOA(0.0);
            const types::RowCol<double> imagePt =
                    projection.sceneToImage(sPos, &timeCOA);

            // Convert this to pixels
            // Need to offset by both the SCP and account for an AOI SICD
            types::RowCol<double>& spCorner(validData[ii]);
            spCorner = (imagePt / spSampleSpacing + spOffset);

            if (spCorner.row >= 0 &&
                spCorner.row < static_cast<double>(sicdData.imageData->numRows) &&
                spCorner.col >= 0 && spCorner.col < static_cast<double>(sicdData.imageData->numCols))
            {
                spCornerIsInBounds = true;
            }
        }

        // If all of the points are out of bounds, the whole image is valid,
        // so don't bother to keep this
        if (!spCornerIsInBounds)
        {
            validData.clear();
        }
    }

    // NOTE: If we don't have an output plane defined, do nothing.  Note that
    //       usually if an output plane isn't defined in the SICD, we define
    //       one ourselves by projecting the slant plane corners into the
    //       output plane.  So projecting those back into the slant plane
    //       isn't going to tell us anything new...
}

template<typename TComplexDataPtr>
static void readSicd_(const std::string& sicdPathname,
                         const std::vector<std::string>& schemaPaths,
                         TComplexDataPtr& complexData,
                         std::vector<six::zfloat>& widebandData)
{
    six::sicd::NITFReadComplexXMLControl reader;
    reader.load(sicdPathname, &schemaPaths);

    // For SICD, there's only one image (container->size() == 1)
    if (reader.getContainer()->size() != 1)
    {
        throw std::invalid_argument(sicdPathname + " is not a SICD; it contains more than one image.");
    }

    auto complexData_ = reader.getComplexData();
    complexData.reset(complexData_.release());
    widebandData = reader.getWidebandData(*(complexData.get()));

    // This tells the reader that it doesn't
    // own an XMLControlRegistry
    reader.setXMLControlRegistry();
}
void Utilities::readSicd(const std::string& sicdPathname,
                         const std::vector<std::string>& schemaPaths,
                         std::unique_ptr<ComplexData>& complexData,
                         std::vector<six::zfloat>& widebandData)
{
    readSicd_(sicdPathname, schemaPaths, complexData, widebandData);
}
void Utilities::readSicd(const fs::path& sicdPathname,
                         const std::vector<fs::path>& schemaPaths,
                         std::unique_ptr<ComplexData>& complexData,
                        std::vector<six::zfloat>& widebandData)
{
    const auto schemaPaths_ = sys::convertPaths(schemaPaths);
    readSicd(sicdPathname.string(), schemaPaths_, complexData, widebandData);
}
ComplexImageResult Utilities::readSicd(const fs::path& sicdPathname, const std::vector<fs::path>& schemaPaths)
{
    ComplexImageResult retval;
    readSicd(sicdPathname, schemaPaths, retval.pComplexData, retval.widebandData);
    return retval;
}

template<typename TComplexDataPtr, typename TNoiseMeshPtr, typename TScalarMeshPtr>
static void readSicd_(const std::string& sicdPathname,
                         const std::vector<std::string>& schemaPaths,
                         size_t orderX,
                         size_t orderY,
                         TComplexDataPtr& complexData,
                         std::vector<six::zfloat>& widebandData,
                         six::Poly2D& outputRowColToSlantRow,
                         six::Poly2D& outputRowColToSlantCol,
                         TNoiseMeshPtr& noiseMesh,
                         TScalarMeshPtr& scalarMesh)
{
    six::sicd::NITFReadComplexXMLControl reader;
    reader.load(sicdPathname, &schemaPaths);

    auto complexData_ = reader.getComplexData();
    complexData.reset(complexData_.release());
    widebandData = reader.getWidebandData(*complexData);
    Utilities::getProjectionPolys(reader.NITFReadControl(),
                       orderX,
                       orderY,
                       complexData,
                       outputRowColToSlantRow,
                       outputRowColToSlantCol);

    std::unique_ptr<NoiseMesh> noiseMesh_;
    std::unique_ptr<ScalarMesh> scalarMesh_;
    reader.getMeshes(noiseMesh_, scalarMesh_);
    noiseMesh.reset(noiseMesh_.release());
    scalarMesh.reset(scalarMesh_.release());

    reader.setXMLControlRegistry();
}

void Utilities::readSicd(const std::string& sicdPathname,
                         const std::vector<std::string>& schemaPaths,
                         size_t orderX,
                         size_t orderY,
                         std::unique_ptr<ComplexData>& complexData,
                         std::vector<six::zfloat>& widebandData,
                         six::Poly2D& outputRowColToSlantRow,
                         six::Poly2D& outputRowColToSlantCol,
                         std::unique_ptr<NoiseMesh>& noiseMesh,
                         std::unique_ptr<ScalarMesh>& scalarMesh)
{
    readSicd_(sicdPathname, schemaPaths, orderX, orderY, complexData, widebandData,
        outputRowColToSlantRow, outputRowColToSlantCol, noiseMesh, scalarMesh);
}

std::unique_ptr<ComplexData> Utilities::getComplexData(NITFReadControl& reader)
{
    const six::Data* data = reader.getContainer()->getData(0);

    if (data->getDataType() != six::DataType::COMPLEX)
    {
        throw except::Exception(Ctxt(data->getName() + " is not a SICD"));
    }

    // Now that we know it's a SICD we can safely cast to a ComplexData
    // We can't just return a pointer to the data though because it's owned by
    // the container which is owned by the reader (and the reader will go out
    // of scope when this function returns), so we need to clone it.
    // Note that you don't have to do this yourself if in your usage the
    // reader stays in scope.
    // TODO: If the container held shared pointers we wouldn't need to do this
    std::unique_ptr<ComplexData> complexData(
        static_cast<ComplexData*>(data->clone()));
    return complexData;
}

std::unique_ptr<ComplexData> Utilities::getComplexData(
        const std::string& pathname,
        const std::vector<std::string>& schemaPaths)
{
    std::string extension = fs::path(pathname).extension().string();
    str::lower(extension);

    if (extension == ".xml")
    {
        logging::NullLogger log;
        return parseDataFromFile(pathname, schemaPaths, log);
    }

    else
    {
        six::sicd::NITFReadComplexXMLControl reader;
        reader.load(pathname, &schemaPaths);

        auto pComplexData = reader.getComplexData();
        return std::unique_ptr<ComplexData>(pComplexData.release());
    }
}

void Utilities::getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                const types::RowCol<size_t>& offset,
                                const types::RowCol<size_t>& extent,
                                six::zfloat* buffer)
{
    const PixelType pixelType = complexData.getPixelType();
    constexpr size_t imageNumber = 0;

    const size_t requiredBufferBytes =
            sizeof(six::zfloat) * extent.area();

    if (buffer == nullptr)
    {
        // for some reason we don't have a buffer

        throw except::Exception(Ctxt("Null buffer provided to getWidebandData" +
                                     std::string(" when a ") +
                                     std::to_string(requiredBufferBytes) +
                                     " byte buffer was expected"));
    }

    if (pixelType == PixelType::RE32F_IM32F)
    {
        six::Region region = buildRegion(offset, extent, buffer);
        reader.interleaved(region, imageNumber);
    }
    else if (pixelType == PixelType::RE16I_IM16I)
    {

        // Each pixel is stored as a pair of numbers that represent the real and imaginary 
        // components. Each component is stored in a 16-bit signed integer in 2's 
        // complement format (2 bytes per component, 4 bytes per pixel). 
        const size_t elementsPerRow = extent.col * (1 + 1); // "real and imaginary"
        const SICD_readerAndConverter<int16_t> readerAndConverter(reader, imageNumber, offset, extent, elementsPerRow, buffer);
    }
    else if (pixelType == PixelType::AMP8I_PHS8I)
    {
        const auto pAmplitudeTable = complexData.imageData->amplitudeTable.get();

        // Each pixel is stored as a pair of numbers that represent the amplitude and phase
        // components. Each component is stored in an 8-bit unsigned integer (1 byte per 
        // component, 2 bytes per pixel). 
        const size_t elementsPerRow = extent.col * (1 + 1); // "amplitude and phase components."
        const SICD_readerAndConverter<uint8_t> readerAndConverter(reader, imageNumber, offset, extent, elementsPerRow, buffer, pAmplitudeTable);
    }
    else
    {
        throw except::Exception(
                Ctxt(complexData.getName() + " has an unknown pixel type"));
    }
}
void Utilities::getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                six::zfloat* buffer)
{
    const types::RowCol<size_t> offset(0, 0);
    const auto extent = getExtent(complexData);
    getWidebandData(reader, complexData, offset, extent, buffer);
}

void Utilities::getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                const types::RowCol<size_t>& offset,
                                const types::RowCol<size_t>& extent,
                                std::vector<six::zfloat>& buffer)
{
    const size_t requiredNumElements = extent.area();
    buffer.resize(requiredNumElements);

    if (requiredNumElements > 0)
    {
        getWidebandData(reader, complexData, offset, extent, buffer.data());
    }
}

void Utilities::getWidebandData(NITFReadControl& reader,
                                const ComplexData& complexData,
                                std::vector<six::zfloat>& buffer)
{
    const types::RowCol<size_t> offset{};
    const auto extent = getExtent(complexData);
    getWidebandData(reader, complexData, offset, extent, buffer);
}

void Utilities::getWidebandData(const std::string& sicdPathname,
                                const std::vector<std::string>& /*schemaPaths*/,
                                const ComplexData& complexData,
                                const types::RowCol<size_t>& offset,
                                const types::RowCol<size_t>& extent,
                                six::zfloat* buffer)
{
    six::sicd::NITFReadComplexXMLControl reader;
    reader.load(sicdPathname);
    reader.getWidebandData(complexData, offset, extent, buffer);
}

void Utilities::getWidebandData(const std::string& sicdPathname,
                                const std::vector<std::string>& schemaPaths,
                                const ComplexData& complexData,
                                six::zfloat* buffer)
{
    const types::RowCol<size_t> offset{};
    const auto extent = getExtent(complexData);
    getWidebandData(sicdPathname, schemaPaths, complexData, offset, extent, buffer);
}

template<> SIX_SICD_API
void Utilities::getRawData(NITFReadControl& reader,
    const ComplexData& complexData,
    const types::RowCol<size_t>& offset,
    const types::RowCol<size_t>& extent,
    std::vector<six::zfloat>& buffer)
{
    const auto pixelType = complexData.getPixelType();
    if (pixelType != PixelType::RE32F_IM32F)
    {
        throw except::Exception(Ctxt(complexData.getName() + " has an unexpected pixel type"));
    }

    getWidebandData(reader, complexData, offset, extent, buffer);
}

template<> SIX_SICD_API
void Utilities::getRawData(NITFReadControl& reader,
    const ComplexData& complexData,
    const types::RowCol<size_t>& offset,
    const types::RowCol<size_t>& extent,
    std::vector<int16_t>& buffer)
{
    const auto pixelType = complexData.getPixelType();
    if (pixelType != PixelType::RE16I_IM16I)
    {
        throw except::Exception(Ctxt(complexData.getName() + " has an unexpected pixel type"));
    }

    constexpr size_t imageNumber = 0;

    // Each pixel is stored as a pair of numbers that represent the real and imaginary 
    // components. Each component is stored in a 16-bit signed integer in 2's 
    // complement format (2 bytes per component, 4 bytes per pixel). 
    const size_t elementsPerRow = extent.col * (1 + 1); // "real and imaginary"
    SICDreader<int16_t>(reader, imageNumber, offset, extent, elementsPerRow,
        [&](size_t /*elementsPerRow*/, size_t /*row*/, size_t /*rowsToRead*/, const std::vector<int16_t>& tempVector)
        {
            buffer.insert(buffer.end(), tempVector.begin(), tempVector.end());
        });
}

template<> SIX_SICD_API
void Utilities::getRawData(NITFReadControl& reader,
    const ComplexData& complexData,
    const types::RowCol<size_t>& offset,
    const types::RowCol<size_t>& extent,
    std::vector<AMP8I_PHS8I_t>& buffer)
{
    const auto pixelType = complexData.getPixelType();
    if (pixelType != PixelType::AMP8I_PHS8I)
    {
        throw except::Exception(Ctxt(complexData.getName() + " has an unexpected pixel type"));
    }

    constexpr size_t imageNumber = 0;

    //const auto pAmplitudeTable = complexData.imageData->amplitudeTable.get();

    // Each pixel is stored as a pair of numbers that represent the amplitude and phase
    // components. Each component is stored in an 8-bit unsigned integer (1 byte per 
    // component, 2 bytes per pixel). 
    SICDreader<AMP8I_PHS8I_t>(reader, imageNumber, offset, extent, extent.col,
        [&](size_t elementsPerRow, size_t /*row*/, size_t rowsToRead, const std::vector<AMP8I_PHS8I_t>& tempVector)
        {
            for (size_t index = 0; index < elementsPerRow * rowsToRead; index++)
            {
                // "For amplitude and phase components, the amplitude component is  stored first."                
                buffer.push_back(tempVector[index]);
            }
        });
}

Vector3 Utilities::getGroundPlaneNormal(const ComplexData& data)
{
    Vector3 groundPlaneNormal{};

    if (data.radarCollection->area.get() &&
        data.radarCollection->area->plane.get())
    {
        const AreaPlane& areaPlane = *data.radarCollection->area->plane;
        groundPlaneNormal =
                math::linear::cross(areaPlane.xDirection->unitVector,
                                    areaPlane.yDirection->unitVector);
    }
    else
    {
        groundPlaneNormal = data.geoData->scp.ecf;
    }

    return groundPlaneNormal.unit();
}

Poly1D Utilities::nPoly(PolyXYZ poly, size_t idx)
{
    if (idx >= 3)
    {
        throw except::Exception(Ctxt("Idx should be < 3"));
    }

    std::vector<double> coefs;
    for (size_t ii = 0; ii < poly.size(); ++ii)
    {
        coefs.push_back(poly[ii][idx]);
    }
    Poly1D ret(coefs);
    return ret;
}

bool Utilities::isClockwise(const std::vector<RowColInt>& vertices,
                            bool isUpPositive)
{
    if (vertices.size() < 3)
    {
        throw except::Exception(
                Ctxt("Polygons must have at least three points"));
    }

    // If the signed area is positive, and y values are ascending,
    // then it is clockwise
    ptrdiff_t area = 0;
    for (size_t ii = 0; ii < vertices.size(); ++ii)
    {
        const ptrdiff_t x1 = static_cast<ptrdiff_t>(vertices[ii].col);
        const ptrdiff_t y1 = static_cast<ptrdiff_t>(vertices[ii].row);

        const size_t nextIndex = (ii == vertices.size() - 1) ? 0 : ii + 1;

        const ptrdiff_t x2 =
                static_cast<ptrdiff_t>(vertices[nextIndex].col);
        const ptrdiff_t y2 =
                static_cast<ptrdiff_t>(vertices[nextIndex].row);

        area += (x1 * y2 - x2 * y1);
    }
    if (isUpPositive)
    {
        area *= -1;
    }
    return (area > 0);
}

std::unique_ptr<ComplexData> Utilities::parseData(
        ::io::InputStream& xmlStream,
        const std::vector<std::string>& schemaPaths,
        logging::Logger& log)
{
    XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator<ComplexXMLControl>();

    auto pData = six::parseData(xmlRegistry, xmlStream, schemaPaths, log);
    return std::unique_ptr<ComplexData>(static_cast<ComplexData*>(pData.release()));
}
std::unique_ptr<ComplexData> Utilities::parseData(::io::InputStream& xmlStream,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger& log)
{
    DataParser parser(pSchemaPaths, &log);
    parser.preserveCharacterData(false); // existing behavior
    return parser.fromXML(xmlStream);
}

std::unique_ptr<ComplexData> Utilities::parseDataFromFile(
        const std::string& pathname,
        const std::vector<std::string>& schemaPaths,
        logging::Logger& log)
{
    io::FileInputStream inStream(pathname);
    return parseData(inStream, schemaPaths, log);
}
std::unique_ptr<ComplexData> Utilities::parseDataFromFile(const std::filesystem::path& pathname,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLogger)
{
    DataParser parser(pSchemaPaths, pLogger);
    parser.preserveCharacterData(false); // existing behavior
    return parser.fromXML(pathname);
}

std::unique_ptr<ComplexData> Utilities::parseDataFromString(
        const std::string& xmlStr_,
        const std::vector<std::string>& schemaPaths_,
        logging::Logger& log)
{
    const auto xmlStr = str::u8FromNative(xmlStr_);
    const auto schemaPaths = sys::convertPaths(schemaPaths_);
    return parseDataFromString(xmlStr, &schemaPaths, &log);
}
std::unique_ptr<ComplexData> Utilities::parseDataFromString(const std::u8string& xmlStr,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLogger)
{
    DataParser parser(pSchemaPaths, pLogger);
    parser.preserveCharacterData(false); // existing behavior
    return parser.fromXML(xmlStr);
}

std::u8string Utilities::toXMLString(const ComplexData& data,
                                   const std::vector<std::string>& schemaPaths_,
                                   logging::Logger* logger)
{
    const auto schemaPaths = sys::convertPaths(schemaPaths_);
    return toXMLString(data, &schemaPaths, logger);
 }
std::string Utilities::toXMLString_(const ComplexData& data,
    const std::vector<std::string>& schemaPaths,
    logging::Logger* logger)
{
    const auto result = toXMLString(data, schemaPaths, logger);
    return str::to_native(result);
}
std::u8string Utilities::toXMLString(const ComplexData& data,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLogger)
{
    const DataParser dataParser(pSchemaPaths, pLogger);
    return dataParser.toXML(data);
}

static void update_for_SICD_130(ComplexData& data)
{
    data.errorStatistics.reset(new ErrorStatistics());

    // Use a OTHER.*:OTHER.* DualPolarizationType type to test more code
    constexpr auto dualPolarizationType = "OTHER_TxRcvPolarizationProc:OTHER_TxRcvPolarizationProc"; // match sicd130.xml
    data.imageFormation->txRcvPolarizationProc = six::toType<DualPolarizationType>(dualPolarizationType);
}

static std::unique_ptr<ComplexData> createFakeComplexData_(const std::string& strVersion, 
    PixelType pixelType, bool makeAmplitudeTable, const types::RowCol<size_t>* pDims)
{
    std::unique_ptr<ComplexData> data(std::make_unique<six::sicd::ComplexData>());
    if (strVersion == "1.3.0")
    {
        data->setVersion(strVersion);
    }

    data->position->arpPoly = six::PolyXYZ(5);
    data->position->arpPoly[0][0] = 4.45303008e6;
    data->position->arpPoly[1][0] = 5.75153322e3;
    data->position->arpPoly[2][0] = -2.7014602e0;
    data->position->arpPoly[3][0] = -1.2323143e-3;
    data->position->arpPoly[4][0] = 2.76797758e-7;
    data->position->arpPoly[5][0] = 8.57355543e-11;

    data->position->arpPoly[0][1] = 1.49876146e6;
    data->position->arpPoly[1][1] = 4.95848815e1;
    data->position->arpPoly[2][1] = -1.3299011e0;
    data->position->arpPoly[3][1] = 1.20353768e-4;
    data->position->arpPoly[4][1] = 1.79750748e-7;
    data->position->arpPoly[5][1] = -1.8053533e-11;

    data->grid->timeCOAPoly = six::Poly2D(2, 2);
    data->grid->timeCOAPoly[0][0] = 1.9789488e0;
    data->grid->timeCOAPoly[0][1] = 2.6269628e-4;
    data->grid->timeCOAPoly[0][2] = 6.1316813e-16;

    data->grid->timeCOAPoly[1][0] = -8.953947e-9;
    data->grid->timeCOAPoly[1][1] = -9.8252154e-13;
    data->grid->timeCOAPoly[1][2] = -9.6216278e-24;

    data->grid->timeCOAPoly[0][0] = 3.3500568e-17;
    data->grid->timeCOAPoly[0][1] = 3.6743435e-21;
    data->grid->timeCOAPoly[0][2] = -3.95088507e-28;

    data->geoData->scp.llh.setLat(4.785840035e1);
    data->geoData->scp.llh.setLon(1.213899391e1);
    data->geoData->scp.llh.setAlt(4.880295281e2);

    data->geoData->scp.ecf[0] = 4.19186033e6;
    data->geoData->scp.ecf[1] = 9.01641404e5;
    data->geoData->scp.ecf[2] = 4.70668874e6;

    data->geoData->imageCorners.getCorner(0).setLat(22.0493529164731);
    data->geoData->imageCorners.getCorner(0).setLon(-159.744920871161);

    data->geoData->imageCorners.getCorner(1).setLat(21.999989220946);
    data->geoData->imageCorners.getCorner(1).setLon(-159.75130136289);

    data->geoData->imageCorners.getCorner(2).setLat(22.0103386157109);
    data->geoData->imageCorners.getCorner(2).setLon(-159.818163639552);

    data->geoData->imageCorners.getCorner(3).setLat(22.0593288745824);
    data->geoData->imageCorners.getCorner(3).setLon(-159.809407055003);

    // Don't have data for this. Just putting something in to prevent
    // a segfault.
    // Later, we can switch on ImageFormation type
    data->pfa.reset(new six::sicd::PFA());
    data->pfa->polarAnglePoly = six::Poly1D(1);
    data->pfa->spatialFrequencyScaleFactorPoly = six::Poly1D(1);

    data->collectionInformation->radarMode = six::RadarModeType::SPOTLIGHT;

    data->setPixelType(pixelType);
    data->imageData->validData = std::vector<six::RowColInt>(8);
    data->imageData->validData[0] = six::RowColInt(0, 0);
    data->imageData->validData[1] = six::RowColInt(0, 6163);
    data->imageData->validData[2] = six::RowColInt(2760, 6163);
    data->imageData->validData[3] = six::RowColInt(7902, 6163);
    data->imageData->validData[4] = six::RowColInt(11623, 6163);
    data->imageData->validData[5] = six::RowColInt(11790, 6163);
    data->imageData->validData[6] = six::RowColInt(11790, 0);
    data->imageData->validData[7] = six::RowColInt(1028, 0);

    if (makeAmplitudeTable)
    {
        data->imageData->amplitudeTable.reset(new AmplitudeTable());
        auto& amplitudeTable = *(data->imageData->amplitudeTable.get());
        for (size_t i = 0; i < amplitudeTable.size(); i++)
        {
            amplitudeTable.index(i) = static_cast<double>(i);
        }
    }

    if (pDims != nullptr)
    {
        setExtent(*data, *pDims);
    }

    // The fields below here aren't really used,
    // just need to be filled with something so things are valid
    data->imageCreation.reset(new ImageCreation());
    data->grid->row->unitVector[0] = 0;
    data->grid->row->unitVector[1] = 0;
    data->grid->row->unitVector[2] = 0;
    data->grid->row->sampleSpacing = 0;
    data->grid->row->impulseResponseWidth = 0;
    data->grid->row->impulseResponseBandwidth = 0;
    data->grid->row->kCenter = 0;
    data->grid->row->deltaK1 = 0;
    data->grid->row->deltaK2 = 0;

    data->grid->col->unitVector[0] = 0;
    data->grid->col->unitVector[1] = 0;
    data->grid->col->unitVector[2] = 0;
    data->grid->col->sampleSpacing = 0;
    data->grid->col->impulseResponseWidth = 0;
    data->grid->col->impulseResponseBandwidth = 0;
    data->grid->col->kCenter = 0;
    data->grid->col->deltaK1 = 0;
    data->grid->col->deltaK2 = 0;

    data->timeline->collectDuration = 0;
    data->radarCollection->txFrequencyMin = 0;
    data->radarCollection->txFrequencyMax = 0;
    data->radarCollection->txPolarization = PolarizationSequenceType::V;
    data->radarCollection->rcvChannels.resize(1);
    data->radarCollection->rcvChannels[0].reset(new ChannelParameters());
    data->radarCollection->rcvChannels[0]->txRcvPolarization =
            DualPolarizationType::V_V;

    data->imageFormation->rcvChannelProcessed.reset(new RcvChannelProcessed());
    data->imageFormation->rcvChannelProcessed->numChannelsProcessed = 1;
    data->imageFormation->rcvChannelProcessed->prfScaleFactor = 0;
    data->imageFormation->rcvChannelProcessed->channelIndex.push_back(0);
    data->imageFormation->txRcvPolarizationProc = DualPolarizationType::V_V;
    data->imageFormation->tStartProc = 0;
    data->imageFormation->tEndProc = 0;
    data->imageFormation->txFrequencyProcMin = 0;
    data->imageFormation->txFrequencyProcMax = 0;

    data->scpcoa->scpTime = 0;
    data->fillDerivedFields();

    data->pfa->krg1 = 0;
    data->pfa->krg2 = 0;
    data->pfa->kaz1 = 0;
    data->pfa->kaz2 = 0;

    data->collectionInformation.reset(new CollectionInformation());
    data->collectionInformation->setClassificationLevel("UNCLASSIFIED");
    data->collectionInformation->radarMode = six::RadarModeType::SPOTLIGHT;

    auto& parameters = data->collectionInformation->parameters;
    six::Parameter param;
    param.setName("TestParameter");
    param.setValue("setValue() for TestParameter");
    parameters.push_back(param);

    // By default, XML parsing in CODA-OSS trims whitespace, see preserveCharacterData
    param.setName("TestParameterEmpty");
    param.setValue("");
    parameters.push_back(param);
    param.setName("TestParameterThreeSpaces");
    param.setValue("   ");
    parameters.push_back(param);

    if (!strVersion.empty()) // TODO: better check for version; this avoid changing any existing test code
    {
        update_for_SICD_130(*data);
    }

    return data;
}
std::unique_ptr<ComplexData> Utilities::createFakeComplexData(const std::string& strVersion,
    PixelType pixelType, bool makeAmplitudeTable, const types::RowCol<size_t>* pDims)
{
    if ((strVersion == "1.2.1") || (strVersion == "1.3.0"))
    {
        return createFakeComplexData_(strVersion, pixelType, makeAmplitudeTable, pDims);
    }
    throw std::invalid_argument("strVersion = '" + strVersion + "' is not supported.");
}
std::unique_ptr<ComplexData> Utilities::createFakeComplexData(const types::RowCol<size_t>* pDims)
{
    auto result = createFakeComplexData_("" /*strVersion*/,
        PixelType::RE32F_IM32F, false /*makeAmplitudeTable*/, pDims);
    return std::unique_ptr<ComplexData>(result.release());
}

std::unique_ptr<NoiseMesh> Utilities::getNoiseMesh(const NITFReadControl& reader)
{
    const std::map<std::string, size_t> nameToDesIndex =
            getAdditionalDesMap(reader);

    std::map<std::string, size_t>::const_iterator it =
            nameToDesIndex.find(SICDMeshes::NOISE_MESH_ID);

    // Slant and output plane mesh IDs must be present in the DES
    if (it == nameToDesIndex.end())
    {
        throw except::Exception(Ctxt("Noise mesh information not present"));
    }

    // Extract the noise mesh
    return extractMesh<NoiseMesh>(SICDMeshes::NOISE_MESH_ID,
                                  it->second,
                                  reader);
}

std::unique_ptr<ScalarMesh> Utilities::getScalarMesh(const NITFReadControl& reader)
{
    const std::map<std::string, size_t> nameToDesIndex =
            getAdditionalDesMap(reader);

    std::map<std::string, size_t>::const_iterator it =
            nameToDesIndex.find(SICDMeshes::SCALAR_MESH_ID);

    // Scalar mesh is optional - return null if the ID is not present in the DES
    if (it == nameToDesIndex.end())
    {
        return std::unique_ptr<ScalarMesh>();
    }

    // Extract the scalar mesh
    return extractMesh<ScalarMesh>(SICDMeshes::SCALAR_MESH_ID,
                                   it->second,
                                   reader);
}

template<typename TComplexDataPtr>
static void getProjectionPolys_(const NITFReadControl& reader,
                                   size_t orderX,
                                   size_t orderY,
                                   TComplexDataPtr& complexData,
                                   six::Poly2D& outputRowColToSlantRow,
                                   six::Poly2D& outputRowColToSlantCol)
{
    const std::map<std::string, size_t> nameToDesIndex =
            getAdditionalDesMap(reader);

    // Slant and output plane mesh IDs must be present in the DES
    if (nameToDesIndex.find(SICDMeshes::SLANT_PLANE_MESH_ID) ==
        nameToDesIndex.end())
    {
        throw except::Exception(
                Ctxt("Slant plane mesh information not present"));
    }

    if (nameToDesIndex.find(SICDMeshes::OUTPUT_PLANE_MESH_ID) ==
        nameToDesIndex.end())
    {
        throw except::Exception(
                Ctxt("Output plane mesh information not present"));
    }

    // SICD metadata must have the AreaPlane populated. Otherwise, the
    // projection mesh information is nonsense.
    if (!AreaPlaneUtility::hasAreaPlane(*complexData))
    {
        throw except::Exception(Ctxt("AreaPlane information must be populated "
                                     "to use projection mesh"));
    }

    // Extract the slant plane mesh buffer and deserialize
    std::unique_ptr<PlanarCoordinateMesh> slantMesh =
            extractMesh<PlanarCoordinateMesh>(
                    SICDMeshes::SLANT_PLANE_MESH_ID,
                    nameToDesIndex.at(SICDMeshes::SLANT_PLANE_MESH_ID),
                    reader);

    // Extract the output plane mesh buffer and deserialize
    std::unique_ptr<PlanarCoordinateMesh> outputMesh =
            extractMesh<PlanarCoordinateMesh>(
                    SICDMeshes::OUTPUT_PLANE_MESH_ID,
                    nameToDesIndex.at(SICDMeshes::OUTPUT_PLANE_MESH_ID),
                    reader);

    const types::RowCol<double> outputSampleSpacing(
            complexData->radarCollection->area->plane->xDirection->spacing,
            complexData->radarCollection->area->plane->yDirection->spacing);

    types::RowCol<size_t> outputPlaneOffset;
    types::RowCol<size_t> outputPlaneExtent;
    complexData->getOutputPlaneOffsetAndExtent(outputPlaneOffset,
                                               outputPlaneExtent);

    const types::RowCol<double> outputCenter(
            0.5 * (static_cast<double>(outputPlaneExtent.row) - 1.0),
            0.5 * (static_cast<double>(outputPlaneExtent.col) - 1.0));

    const types::RowCol<double> slantSampleSpacing(
            complexData->grid->row->sampleSpacing,
            complexData->grid->col->sampleSpacing);

    const types::RowCol<double> slantCenter(
            static_cast<double>(complexData->imageData->scpPixel.row),
            static_cast<double>(complexData->imageData->scpPixel.col));

    six::Poly2D outputXYToSlantX;
    six::Poly2D outputXYToSlantY;
    six::Poly2D slantXYToOutputX;
    six::Poly2D slantXYToOutputY;
    Utilities::fitXYProjectionPolys(*outputMesh,
                         *slantMesh,
                         orderX,
                         orderY,
                         outputXYToSlantX,
                         outputXYToSlantY,
                         slantXYToOutputX,
                         slantXYToOutputY);

    Utilities::transformXYProjectionPolys(outputXYToSlantX,
                               outputXYToSlantY,
                               slantSampleSpacing,
                               outputSampleSpacing,
                               slantCenter,
                               outputCenter,
                               outputRowColToSlantRow,
                               outputRowColToSlantCol);
}
void Utilities::getProjectionPolys(const NITFReadControl& reader,
                                   size_t orderX,
                                   size_t orderY,
                                   std::unique_ptr<ComplexData>& complexData,
                                   six::Poly2D& outputRowColToSlantRow,
                                   six::Poly2D& outputRowColToSlantCol)
{
    getProjectionPolys_(reader, orderX, orderY, complexData, outputRowColToSlantRow, outputRowColToSlantCol);
}

six::Poly2D Utilities::transformXYPolyToRowColPoly(
        const six::Poly2D& polyXY,
        const types::RowCol<double>& outSampleSpacing,
        const types::RowCol<double>& outCenter,
        double polyScaleFactor,
        double polyShift)
{
    const six::Poly2D xToRowTransform =
            getXYtoRowColTransform(outCenter.row, outSampleSpacing.row, true);

    const six::Poly2D yToColTransform =
            getXYtoRowColTransform(outCenter.col, outSampleSpacing.col, false);

    // Transform the output to input polynomials to
    // take row, column inputs
    six::Poly2D polyRowCol =
            polyXY.transformInput(xToRowTransform, yToColTransform);

    polyRowCol *= polyScaleFactor;
    polyRowCol[0][0] += polyShift;

    return polyRowCol;
}

void Utilities::transformXYProjectionPolys(
        const six::Poly2D& outputXYToSlantX,
        const six::Poly2D& outputXYToSlantY,
        const types::RowCol<double>& slantSampleSpacing,
        const types::RowCol<double>& outputSampleSpacing,
        const types::RowCol<double>& slantCenter,
        const types::RowCol<double>& outputCenter,
        six::Poly2D& outputRowColToSlantRow,
        six::Poly2D& outputRowColToSlantCol)
{
    outputRowColToSlantRow =
            transformXYPolyToRowColPoly(outputXYToSlantX,
                                        outputSampleSpacing,
                                        outputCenter,
                                        1.0 / slantSampleSpacing.row,
                                        slantCenter.row);
    outputRowColToSlantCol =
            transformXYPolyToRowColPoly(outputXYToSlantY,
                                        outputSampleSpacing,
                                        outputCenter,
                                        1.0 / slantSampleSpacing.col,
                                        slantCenter.col);
}

void Utilities::fitXYProjectionPolys(
        const six::sicd::PlanarCoordinateMesh& outputMesh,
        const six::sicd::PlanarCoordinateMesh& slantMesh,
        size_t orderX,
        size_t orderY,
        six::Poly2D& outputXYToSlantX,
        six::Poly2D& outputXYToSlantY,
        six::Poly2D& slantXYToOutputX,
        six::Poly2D& slantXYToOutputY)
{
    // Meshes must be the same size.
    const types::RowCol<size_t> slantMeshDims = slantMesh.getMeshDims();
    const types::RowCol<size_t> outputMeshDims = outputMesh.getMeshDims();
    if (slantMeshDims.row != outputMeshDims.row ||
        slantMeshDims.col != outputMeshDims.col)
    {
        throw except::Exception(Ctxt("Mesh dimensions do not match"));
    }

    const types::RowCol<size_t>& dims = slantMeshDims;
    math::linear::Matrix2D<double> outputX(dims.row,
                                           dims.col,
                                           outputMesh.getX().data());
    math::linear::Matrix2D<double> outputY(dims.row,
                                           dims.col,
                                           outputMesh.getY().data());
    math::linear::Matrix2D<double> slantX(dims.row,
                                          dims.col,
                                          slantMesh.getX().data());
    math::linear::Matrix2D<double> slantY(dims.row,
                                          dims.col,
                                          slantMesh.getY().data());

    outputXYToSlantX =
            math::poly::fit(outputX, outputY, slantX, orderX, orderY);
    outputXYToSlantY =
            math::poly::fit(outputX, outputY, slantY, orderX, orderY);
    slantXYToOutputX = math::poly::fit(slantX, slantY, outputX, orderX, orderY);
    slantXYToOutputY = math::poly::fit(slantX, slantY, outputY, orderX, orderY);
}

void Utilities::projectPixelsToOutputPlane(
        const six::sicd::ComplexData& complexData,
        const std::vector<types::RowCol<double>>& spPixels,
        std::vector<types::RowCol<double>>& opPixels)
{
    std::unique_ptr<scene::SceneGeometry> geometry;
    std::unique_ptr<scene::ProjectionModel> projectionModel;
    AreaPlane areaPlane;

    Utilities::getModelComponents(complexData,
                                  geometry,
                                  projectionModel,
                                  areaPlane);

    const types::RowCol<double> opSampleSpacing(areaPlane.xDirection->spacing,
                                                areaPlane.yDirection->spacing);

    const types::RowCol<double> opCenterPixel(
            static_cast<double>(areaPlane.xDirection->elements / 2 + 1),
            static_cast<double>(areaPlane.yDirection->elements / 2 + 1));

    const six::Vector3 opORPECEF = areaPlane.referencePoint.ecef;
    const six::Vector3 opZ = Utilities::getGroundPlaneNormal(complexData);

    // Project slant plane pixels to output plane pixels.
    opPixels.resize(spPixels.size());
    for (size_t ii = 0; ii < spPixels.size(); ++ii)
    {
        const types::RowCol<double> spXY(
                complexData.pixelToImagePoint(spPixels[ii]));

        // Convert to output plane ECEF.
        const six::Vector3 opECEF =
                projectionModel->imageToScene(spXY, opORPECEF, opZ);

        // Convert ECEF to output distance to the output plane ORP.
        const six::Vector3 diffECEF = opECEF - opORPECEF;
        const double opX = diffECEF.dot(areaPlane.xDirection->unitVector);
        const double opY = diffECEF.dot(areaPlane.yDirection->unitVector);

        // Convert XY to pixels.
        opPixels[ii] = types::RowCol<double>(opX / opSampleSpacing.row +
                                                     opCenterPixel.row,
                                             opY / opSampleSpacing.col +
                                                     opCenterPixel.col);
    }
}

void Utilities::projectValidDataPolygonToOutputPlane(
        const six::sicd::ComplexData& complexData,
        std::vector<types::RowCol<double>>& opPixels)
{
    // If we don't have a valid data polygon, then the entire SICD is valid.
    std::vector<six::RowColInt> validData = complexData.imageData->validData;
    if (validData.size() == 0)
    {
        // Get dimensions of SICD.
        const types::RowCol<ptrdiff_t> extent(getExtent(complexData));
        const auto numRows = extent.row;
        const auto numCols = extent.col;

        validData.push_back(six::RowColInt(0, 0));
        validData.push_back(six::RowColInt(0, numCols - 1));
        validData.push_back(six::RowColInt(numRows - 1, numCols - 1));
        validData.push_back(six::RowColInt(numRows - 1, 0));
    }

    // Convert to double coordinates.
    std::vector<types::RowCol<double>> spPixels(validData.size());
    for (size_t ii = 0; ii < validData.size(); ++ii)
    {
        spPixels[ii] = validData[ii];
    }

    // Project to the output plane.
    projectPixelsToOutputPlane(complexData, spPixels, opPixels);
}

void Utilities::projectPixelsToSlantPlane(
        const six::sicd::ComplexData& complexData,
        const std::vector<types::RowCol<double>>& opPixels,
        std::vector<types::RowCol<double>>& spPixels)
{
    std::unique_ptr<scene::SceneGeometry> geometry;
    std::unique_ptr<scene::ProjectionModel> projectionModel;
    AreaPlane areaPlane;

    Utilities::getModelComponents(complexData,
                                  geometry,
                                  projectionModel,
                                  areaPlane);

    const types::RowCol<double> opSampleSpacing(areaPlane.xDirection->spacing,
                                                areaPlane.yDirection->spacing);
    const scene::PlanarGridECEFTransform ecefTransform(
            opSampleSpacing,
            areaPlane.referencePoint.rowCol,
            areaPlane.xDirection->unitVector,
            areaPlane.yDirection->unitVector,
            areaPlane.referencePoint.ecef);
    const types::RowCol<double> spSampleSpacing(
            complexData.grid->row->sampleSpacing,
            complexData.grid->col->sampleSpacing);
    const types::RowCol<double> spOrigOffset(
            static_cast<double>(complexData.imageData->firstRow),
            static_cast<double>(complexData.imageData->firstCol));
    const types::RowCol<double> spSCP(complexData.imageData->scpPixel);
    const types::RowCol<double> spOffset(spSCP.row - spOrigOffset.row,
                                         spSCP.col - spOrigOffset.col);

    // Project output plane pixels to slant plane pixels.
    spPixels.resize(opPixels.size());
    for (size_t ii = 0; ii < opPixels.size(); ++ii)
    {
        // Convert output plane pixel to ECEF.
        const scene::Vector3 ecef = ecefTransform.rowColToECEF(opPixels[ii]);

        // Convert ECEF to slant plane distance from SCP.
        double timeCOA = 0.0;
        const types::RowCol<double> spXY =
                projectionModel->sceneToImage(ecef, &timeCOA);

        // Convert to slant plane pixel.
        spPixels[ii] = (spXY / spSampleSpacing + spOffset);
    }
}
}
}

std::vector<std::byte> six::sicd::readFromNITF(const fs::path& pathname, const std::vector<fs::path>* pSchemaPaths,
    std::unique_ptr<ComplexData>& pComplexData)
{
    six::sicd::NITFReadComplexXMLControl reader;
    reader.setLogger();
    reader.load(pathname, pSchemaPaths);
    
    // For SICD, there's only one image (container->size() == 1)
    if (reader.getContainer()->size() != 1)
    {
        throw std::invalid_argument(pathname.string() + " is not a SICD; it contains more than one image.");
    }

    pComplexData = reader.getComplexData();
    return reader.interleaved();
}

static void writeAsNITF(const fs::path& pathname, const std::vector<std::string>& schemaPaths_, const six::sicd::ComplexData& data, const six::zfloat* image_)
{
    six::getXMLControlFactory().addCreator<six::sicd::ComplexXMLControl>();

    six::NITFWriteControl writer(data.unique_clone());
    writer.setLogger(logging::setupLogger("out"));

    const std::span<const six::zfloat> image(image_, getExtent(data).area());
    const auto schemaPaths = sys::convertPaths(schemaPaths_);
    writer.save_image(image, pathname, schemaPaths);
}
void six::sicd::writeAsNITF(const fs::path& pathname, const std::vector<std::string>& schemaPaths, const ComplexData& data, std::span<const six::zfloat> image)
{
    if (image.size() != getExtent(data).area())
    {
        throw std::invalid_argument("size() mis-match");
    }
    ::writeAsNITF(pathname, schemaPaths, data, image.data());
}
void six::sicd::writeAsNITF(const fs::path& pathname, const std::vector<fs::path>& schemaPaths, const ComplexData& data, std::span<const six::zfloat> image)
{
    const auto schemaPaths_ = sys::convertPaths(schemaPaths);
    writeAsNITF(pathname, schemaPaths_, data, image);
}
void six::sicd::writeAsNITF(const fs::path& pathname, const std::vector<fs::path>& schemaPaths, const ComplexImage& image)
{
    writeAsNITF(pathname, schemaPaths, image.data, image.image);
}

std::vector<six::zfloat> six::sicd::testing::make_complex_image(const types::RowCol<size_t>& dims)
{
    std::vector<six::zfloat> image;
    image.reserve(dims.area());
    for (size_t r = 0; r < dims.row; r++)
    {
        for (size_t c = 0; c < dims.col; c++)
        {
            image.emplace_back(gsl::narrow_cast<float>(r), gsl::narrow_cast<float>(c) * -1.0f);
        }
    }
    return image;
}

std::vector<std::byte> six::sicd::testing::toBytes(const ComplexImageResult& result)
{
    const auto& image = result.widebandData;
    const auto bytes = sys::as_bytes(image);

    std::vector<std::byte> retval;
    const auto& data = *(result.pComplexData);
    if (data.getPixelType() == six::PixelType::AMP8I_PHS8I)
    {
        std::vector<AMP8I_PHS8I_t> results;
        std::ignore = data.convertPixels(bytes, results);

        const auto result_bytes = std::as_bytes(sys::make_span(results));
        retval.assign(result_bytes.begin(), result_bytes.end());
    }
    else
    {
        retval.insert(retval.begin(), bytes.begin(), bytes.end());
    }

    return retval;
}
