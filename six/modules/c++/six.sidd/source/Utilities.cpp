/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "six/sidd/Utilities.h"

#include <stdexcept>
#include <set>
#include <std/string>

#include <str/Encoding.h>
#include <sys/Path.h>

#include "six/Utilities.h"
#include "six/sidd/DerivedXMLControl.h"
#include "six/sidd/DerivedDataBuilder.h"
#include "six/sidd/DataParser.h"

namespace
{
double getCenterTime(const six::sidd::DerivedData& derived)
{
    if (derived.measurement->projection->isMeasurable())
    {
        const six::sidd::MeasurableProjection* const projection =
            dynamic_cast<const six::sidd::MeasurableProjection*>(
                        derived.measurement->projection.get());

        return projection->timeCOAPoly(0, 0);
    }
    else
    {
        // we estimate...
        return derived.exploitationFeatures->collections[0]
                             ->information.collectionDuration /
                2;
    }
}

namespace
{
void getErrors(const six::sidd::DerivedData& data, scene::Errors& errors)
{
    if (!data.measurement->projection->isMeasurable())
    {
        throw except::Exception(Ctxt("Need measurable projection"));
    }

    const six::sidd::MeasurableProjection* const projection =
        dynamic_cast<const six::sidd::MeasurableProjection*>(
                    data.measurement->projection.get());

    six::getErrors(data.errorStatistics.get(),
                   types::RgAz<double>(projection->sampleSpacing.row,
                                       projection->sampleSpacing.col),
                   errors);
}
}
}

namespace six
{
namespace sidd
{
scene::SideOfTrack Utilities::getSideOfTrack(const DerivedData* derived)
{
    const double centerTime = getCenterTime(*derived);

    // compute arpPos and arpVel
    const six::Vector3 arpPos = derived->measurement->arpPoly(centerTime);
    const six::Vector3 arpVel =
            derived->measurement->arpPoly.derivative()(centerTime);
    const six::Vector3 refPt =
            derived->measurement->projection->referencePoint.ecef;

    return scene::SceneGeometry(arpVel, arpPos, refPt).getSideOfTrack();
}

static six::Vector3 latLonToECEF(const six::sidd::PolynomialProjection& projection,
    double atX, double atY)
{
    scene::LatLonAlt lla;
    lla.setLat(projection.rowColToLat(atX, atY));
    lla.setLon(projection.rowColToLon(atX, atY));
   return scene::Utilities::latLonToECEF(lla);
}

std::unique_ptr<scene::SceneGeometry> Utilities::getSceneGeometry(
        const DerivedData* derived)
{
    const double centerTime = getCenterTime(*derived);

    // compute arpPos and arpVel
    const six::Vector3 arpPos = derived->measurement->arpPoly(centerTime);
    const six::Vector3 arpVel =
            derived->measurement->arpPoly.derivative()(centerTime);
    const six::Vector3 refPt = derived->measurement->projection->referencePoint.ecef;

    six::Vector3 rowVec{};
    six::Vector3 colVec{};

    if (derived->measurement->projection->projectionType ==
        six::ProjectionType::POLYNOMIAL)
    {
        const six::sidd::PolynomialProjection* projection =
            dynamic_cast<const six::sidd::PolynomialProjection*>(
                        derived->measurement->projection.get());

        const auto cR = projection->referencePoint.rowCol.row;
        const auto cC = projection->referencePoint.rowCol.col;

        const six::Vector3 centerEcef = latLonToECEF(*projection, cR, cC);
        const six::Vector3 downEcef = latLonToECEF(*projection, cR + 1, cC);
        const six::Vector3 rightEcef = latLonToECEF(*projection, cR, cC + 1);

        rowVec = downEcef - centerEcef;
        rowVec.normalize();
        colVec = rightEcef - centerEcef;
        colVec.normalize();
    }
    else if (derived->measurement->projection->projectionType ==
             six::ProjectionType::PLANE)
    {
        const six::sidd::PlaneProjection* projection =
            dynamic_cast<const six::sidd::PlaneProjection*>(
                        derived->measurement->projection.get());

        rowVec = projection->productPlane.rowUnitVector;
        colVec = projection->productPlane.colUnitVector;
    }
    else if (derived->measurement->projection->projectionType ==
             six::ProjectionType::GEOGRAPHIC)
    {
        // In this case there are no image plane row/col vectors, so we want
        // to use a different constructor
        std::unique_ptr<scene::SceneGeometry> geom(
                new scene::SceneGeometry(arpVel, arpPos, refPt));
        return geom;
    }
    else
    {
        throw except::Exception(
                Ctxt("Cylindrical projection not yet supported"));
    }

    std::unique_ptr<scene::SceneGeometry> geom(
            new scene::SceneGeometry(arpVel, arpPos, refPt, rowVec, colVec));
    return geom;
}

std::unique_ptr<scene::GridECEFTransform> Utilities::getGridECEFTransform(
        const DerivedData* derived)
{
    if (!derived->measurement->projection->isMeasurable())
    {
        throw except::Exception(Ctxt(
                "Projection type is not measurable: " +
                derived->measurement->projection->projectionType.toString()));
    }

    const six::sidd::MeasurableProjection* p =
        dynamic_cast<const six::sidd::MeasurableProjection*>(
                    derived->measurement->projection.get());

    std::unique_ptr<scene::GridECEFTransform> transform;

    switch ((int)p->projectionType)
    {
    case six::ProjectionType::PLANE:
    {
        const six::sidd::PlaneProjection* const planeP =
            dynamic_cast<const six::sidd::PlaneProjection*>(p);

        transform.reset(new scene::PlanarGridECEFTransform(
                p->sampleSpacing,
                p->referencePoint.rowCol,
                planeP->productPlane.rowUnitVector,
                planeP->productPlane.colUnitVector,
                p->referencePoint.ecef));
        break;
    }

    case six::ProjectionType::GEOGRAPHIC:
        /*
                // Not complete or tested yet
                return new scene::GeographicGridECEFTransform(p->sampleSpacing,
                                                         p->referencePoint.rowCol,
                                                         scene::Utilities::ecefToLatLon(p->referencePoint.ecef));
        */

    case six::ProjectionType::CYLINDRICAL:
        /*
                // Not complete or tested yet
            {
                six::Vector3 stripmapDir =
                        ((six::sidd::CylindricalProjection*)p)->stripmapDirection;
                six::Vector3 ecef = p->referencePoint.ecef;
                scene::ECEFToLLATransform ecefToLLA;
                scene::LatLonAlt lla = ecefToLLA.transform(ecef);

                scene::WGS84EllipsoidModel model;
                double lon = lla.getLonRadians();
                double lat = atan2(ecef[2], pow(1 - model.calculateFlattening(),
           2) * sqrt(pow(ecef[0], 2) + pow(ecef[1], 2)));

                six::Vector3 e, n, u;
                e[0] = -sin(lon);
                e[1] = cos(lon);
                e[2] = 0;

                n[0] = -sin(lat) * cos(lon);
                n[1] = -sin(lat) * sin(lon);
                n[2] = cos(lat);

                u[0] = cos(lat) * cos(lon);
                u[1] = cos(lat) * sin(lon);
                u[2] = sin(lat);

                double a = atan2(e.dot(stripmapDir), n.dot(stripmapDir));
                six::Vector3 colVec = cos(a) * n + sin(a) * e;
                six::Vector3 rowVec = math::linear::cross(colVec, u);

                return new scene::CylindricalGridECEFTransform(p->sampleSpacing,
                                                          p->referencePoint.rowCol,
                                                          rowVec,
                                                          colVec,
                                                          u,
                                                          p->referencePoint.ecef,
                                                          ((six::sidd::CylindricalProjection*)p)->curvatureRadius);
            }
        */

    default:
        throw except::Exception(Ctxt("Invalid projection type: " +
                                     p->projectionType.toString()));
    }

    return transform;
}

std::unique_ptr<scene::GridGeometry> Utilities::getGridGeometry(
        const DerivedData* derived)
{
    if (!derived->measurement->projection->isMeasurable())
    {
        throw except::Exception(Ctxt(
                "Projection type is not measurable: " +
                derived->measurement->projection->projectionType.toString()));
    }

    const six::sidd::MeasurableProjection* p =
        dynamic_cast<const six::sidd::MeasurableProjection*>(
                    derived->measurement->projection.get());

    std::unique_ptr<scene::GridGeometry> geom;

    // Only currently have an implementation for PGD
    switch ((int)p->projectionType)
    {
    case six::ProjectionType::PLANE:
    {
        const six::sidd::PlaneProjection* const planeP =
            dynamic_cast<const six::sidd::PlaneProjection*>(p);

        geom.reset(new scene::PlanarGridGeometry(
                planeP->productPlane.rowUnitVector,
                planeP->productPlane.colUnitVector,
                p->referencePoint.ecef,
                derived->measurement->arpPoly,
                p->timeCOAPoly));
        break;
    }

    default:
        throw except::Exception(Ctxt("Invalid/unsupported projection type: " +
                                     p->projectionType.toString()));
    }

    return geom;
}

void Utilities::setProductValues(Poly2D timeCOAPoly,
                                 PolyXYZ arpPoly,
                                 ReferencePoint ref,
                                 const Vector3* row,
                                 const Vector3* col,
                                 types::RgAz<double> res,
                                 Product* product)
{
    const double scpTime = timeCOAPoly(0, 0);

    const Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    const Vector3 arpVel = arpVelPoly(scpTime);

    setProductValues(arpVel, arpPos, ref.ecef, row, col, res, product);
}

void Utilities::setProductValues(Vector3 arpVel,
                                 Vector3 arpPos,
                                 Vector3 refPos,
                                 const Vector3* row,
                                 const Vector3* col,
                                 types::RgAz<double> res,
                                 Product* product)
{
    const scene::SceneGeometry sceneGeom(arpVel, arpPos, refPos, *row, *col);

    // do some setup of derived data from geometry
    if (product->north == Init::undefined<double>())
    {
        product->north = sceneGeom.getNorthAngle();
    }

    // if (product->resolution
    //    == Init::undefined<RowColDouble>())
    {
        product->resolution = sceneGeom.getGroundResolution(res);
    }
}

void Utilities::setCollectionValues(Poly2D timeCOAPoly,
                                    PolyXYZ arpPoly,
                                    ReferencePoint ref,
                                    const Vector3* row,
                                    const Vector3* col,
                                    Collection* collection)
{
    const double scpTime = timeCOAPoly(0, 0);

    const Vector3 arpPos = arpPoly(scpTime);
    PolyXYZ arpVelPoly = arpPoly.derivative();
    const Vector3 arpVel = arpVelPoly(scpTime);

    setCollectionValues(arpVel, arpPos, ref.ecef, row, col, collection);
}

void Utilities::setCollectionValues(Vector3 arpVel,
                                    Vector3 arpPos,
                                    Vector3 refPos,
                                    const Vector3* row,
                                    const Vector3* col,
                                    Collection* collection)
{
    const scene::SceneGeometry sceneGeom(arpVel, arpPos, refPos, *row, *col);

    if (collection->geometry.get() == nullptr)
    {
        collection->geometry.reset(new Geometry());
    }
    if (collection->phenomenology.get() == nullptr)
    {
        collection->phenomenology.reset(new Phenomenology());
    }

    if (collection->geometry->slope == Init::undefined<double>())
    {
        collection->geometry->slope = sceneGeom.getETPSlopeAngle();
    }
    if (collection->geometry->squint == Init::undefined<double>())
    {
        collection->geometry->squint = sceneGeom.getSquintAngle();
    }
    if (collection->geometry->graze == Init::undefined<double>())
    {
        collection->geometry->graze = sceneGeom.getETPGrazingAngle();
    }
    if (collection->geometry->tilt == Init::undefined<double>())
    {
        collection->geometry->tilt = sceneGeom.getETPTiltAngle();
    }
    if (collection->geometry->azimuth == Init::undefined<double>())
    {
        collection->geometry->azimuth = sceneGeom.getAzimuthAngle();
    }
    if (collection->phenomenology->multiPath == Init::undefined<double>())
    {
        collection->phenomenology->multiPath = sceneGeom.getMultiPathAngle();
    }
    if (collection->phenomenology->groundTrack == Init::undefined<double>())
    {
        collection->phenomenology->groundTrack =
                sceneGeom.getOPGroundTrackAngle();
    }

    if (collection->phenomenology->shadow == Init::undefined<AngleMagnitude>())
    {
        collection->phenomenology->shadow = sceneGeom.getShadow();
    }

    if (collection->phenomenology->layover == Init::undefined<AngleMagnitude>())
    {
        collection->phenomenology->layover = sceneGeom.getLayover();
    }
}

six::PolarizationSequenceType _convertDualPolarization(six::DualPolarizationType pol,
                                               bool useFirst)
{
    switch (pol)
    {
    case six::DualPolarizationType::OTHER:
        return six::PolarizationSequenceType::OTHER;
    case six::DualPolarizationType::V_V:
        return six::PolarizationSequenceType::V;
    case six::DualPolarizationType::V_H:
        return useFirst ? six::PolarizationSequenceType::V : six::PolarizationSequenceType::H;
    case six::DualPolarizationType::V_RHC:
        return useFirst ? six::PolarizationSequenceType::V : six::PolarizationSequenceType::RHC;
    case six::DualPolarizationType::V_LHC:
        return useFirst ? six::PolarizationSequenceType::V : six::PolarizationSequenceType::LHC;
    case six::DualPolarizationType::H_V:
        return useFirst ? six::PolarizationSequenceType::H : six::PolarizationSequenceType::V;
    case six::DualPolarizationType::H_H:
        return six::PolarizationSequenceType::H;
    case six::DualPolarizationType::H_RHC:
        return useFirst ? six::PolarizationSequenceType::H : six::PolarizationSequenceType::RHC;
    case six::DualPolarizationType::H_LHC:
        return useFirst ? six::PolarizationSequenceType::H : six::PolarizationSequenceType::LHC;
    case six::DualPolarizationType::RHC_RHC:
        return six::PolarizationSequenceType::RHC;
    case six::DualPolarizationType::RHC_LHC:
        return useFirst ? six::PolarizationSequenceType::RHC
                        : six::PolarizationSequenceType::LHC;
    case six::DualPolarizationType::RHC_V:
        return useFirst ? six::PolarizationSequenceType::RHC : six::PolarizationSequenceType::V;
    case six::DualPolarizationType::RHC_H:
        return useFirst ? six::PolarizationSequenceType::RHC : six::PolarizationSequenceType::H;
    case six::DualPolarizationType::LHC_RHC:
        return useFirst ? six::PolarizationSequenceType::LHC
                        : six::PolarizationSequenceType::RHC;
    case six::DualPolarizationType::LHC_LHC:
        return six::PolarizationSequenceType::LHC;
    case six::DualPolarizationType::LHC_V:
        return useFirst ? six::PolarizationSequenceType::LHC : six::PolarizationSequenceType::V;
    case six::DualPolarizationType::LHC_H:
        return useFirst ? six::PolarizationSequenceType::LHC : six::PolarizationSequenceType::H;
    case six::DualPolarizationType::UNKNOWN:
        throw except::Exception(Ctxt("DualPolarizationType::UNKNOWN has no "
                                     "corresponding PolarizationType"));
    default:
        return six::PolarizationSequenceType::NOT_SET;
    }
}

std::pair<six::PolarizationSequenceType, six::PolarizationSequenceType>
    Utilities::convertDualPolarization(six::DualPolarizationType pol)
{
    std::pair<six::PolarizationSequenceType, six::PolarizationSequenceType> pols;
    pols.first = _convertDualPolarization(pol, true);
    pols.second = _convertDualPolarization(pol, false);
    return pols;
}

std::unique_ptr<scene::ProjectionModel> Utilities::getProjectionModel(
        const DerivedData* data)
{
    const int lookDir = getSideOfTrack(data);
    scene::Errors errors;
    ::getErrors(*data, errors);

    std::unique_ptr<scene::SceneGeometry> geom(getSceneGeometry(data));

    const six::ProjectionType gridType =
            data->measurement->projection->projectionType;

    std::unique_ptr<scene::ProjectionModel> projModel;
    switch (gridType)
    {
    case six::ProjectionType::PLANE:
    {
        const six::sidd::PlaneProjection* const plane =
            dynamic_cast<six::sidd::PlaneProjection*>(
                        data->measurement->projection.get());

        projModel.reset(new scene::PlaneProjectionModel(
                geom->getSlantPlaneZ(),
                plane->productPlane.rowUnitVector,
                plane->productPlane.colUnitVector,
                plane->referencePoint.ecef,
                data->measurement->arpPoly,
                plane->timeCOAPoly,
                lookDir,
                errors));
        break;
    }
    case six::ProjectionType::GEOGRAPHIC:
    {
        const six::sidd::MeasurableProjection* const geo =
            dynamic_cast<six::sidd::MeasurableProjection*>(
                        data->measurement->projection.get());

        projModel.reset(
                new scene::GeodeticProjectionModel(geom->getSlantPlaneZ(),
                                                   geo->referencePoint.ecef,
                                                   data->measurement->arpPoly,
                                                   geo->timeCOAPoly,
                                                   lookDir,
                                                   errors));
        break;
    }
    case six::ProjectionType::POLYNOMIAL:
    case six::ProjectionType::CYLINDRICAL:
    case six::ProjectionType::NOT_SET:
        throw except::Exception(
                Ctxt("Grid type not supported: " + gridType.toString()));
    default:
        throw except::Exception(
                Ctxt("Invalid grid type: " + gridType.toString()));
    }

    return projModel;
}

std::unique_ptr<DerivedData> Utilities::parseData(::io::InputStream& xmlStream,
        const std::vector<std::string>& schemaPaths, logging::Logger& log)
{
    XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator<DerivedXMLControl>();

    auto data(six::parseData(xmlRegistry, xmlStream, schemaPaths, log));
    return std::unique_ptr<DerivedData>(static_cast<DerivedData*>(data.release()));
}

std::unique_ptr<DerivedData> Utilities::parseData(::io::InputStream& xmlStream,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger& log)
{
    DataParser dataParser(log, pSchemaPaths);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.fromXML(xmlStream);
}

std::unique_ptr<DerivedData> Utilities::parseDataFromFile(const std::string& pathname,
        const std::vector<std::string>& schemaPaths, logging::Logger& log)
{
    io::FileInputStream inStream(pathname);
    return parseData(inStream, schemaPaths, log);
}
std::unique_ptr<DerivedData> Utilities::parseDataFromFile(const std::filesystem::path& pathname,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLogger)
{
    DataParser dataParser(pSchemaPaths, pLogger);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.fromXML(pathname);
}

std::unique_ptr<DerivedData> Utilities::parseDataFromString(const std::string& xmlStr_,
        const std::vector<std::string>& schemaPaths_, logging::Logger& log)
{
    const auto xmlStr = str::u8FromNative(xmlStr_);
    const auto schemaPaths = sys::convertPaths(schemaPaths_);

    auto result = parseDataFromString(xmlStr, &schemaPaths, &log);
    return std::unique_ptr<DerivedData>(result.release());
}
std::unique_ptr<DerivedData> Utilities::parseDataFromString(const std::u8string& xmlStr,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLogger)
{
    DataParser dataParser(pSchemaPaths, pLogger);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.fromXML(xmlStr);
}

std::u8string Utilities::toXMLString(const DerivedData& data,
                                   const std::vector<std::string>& schemaPaths_, logging::Logger* logger)
{
    const auto schemaPaths = sys::convertPaths(schemaPaths_);
    return toXMLString(data, &schemaPaths, logger);
}
std::string Utilities::toXMLString_(const DerivedData& data,
    const std::vector<std::string>& schemaPaths,
    logging::Logger* logger)
{
    const auto result = toXMLString(data, schemaPaths, logger);
    return str::to_native(result);
}
std::u8string Utilities::toXMLString(const DerivedData& data,
    const std::vector<std::filesystem::path>* pSchemaPaths, logging::Logger* pLogger)
{
    DataParser dataParser(pSchemaPaths, pLogger);
    dataParser.preserveCharacterData(false); // existing behavior
    return dataParser.toXML(data);
}

static void createPredefinedFilter(six::sidd::Filter& filter)
{
    filter.filterName = "Some predefined Filter";
    filter.filterKernel.reset(new six::sidd::Filter::Kernel());
    filter.filterKernel->predefined.reset(new six::sidd::Filter::Predefined());
    filter.filterKernel->predefined->databaseName = six::sidd::FilterDatabaseName::LAGRANGE;
    filter.operation = six::sidd::FilterOperation::CONVOLUTION;
}
inline std::unique_ptr<six::sidd::Filter> createPredefinedFilter()
{
    auto filter = std::make_unique<six::sidd::Filter>();
    createPredefinedFilter(*filter);
    return filter;
}

static void createCustomFilter(six::sidd::Filter& filter)
{
    filter.filterName = "Some custom Filter";
    filter.filterKernel.reset(new six::sidd::Filter::Kernel());
    filter.filterKernel->custom.reset(new six::sidd::Filter::Kernel::Custom());
    auto& custom = *(filter.filterKernel->custom);
    custom.size.row = 2;
    custom.size.col = 3;
    custom.filterCoef.resize(static_cast<size_t>(custom.size.area()));
    for (size_t ii = 0, end = custom.filterCoef.size(); ii < end; ++ii)
    {
        custom.filterCoef[ii] = static_cast<double>(ii) * 1.5;
    }
    filter.operation = six::sidd::FilterOperation::CORRELATION;
}
inline std::unique_ptr<six::sidd::Filter> createCustomFilter()
{
    auto filter = std::make_unique<six::sidd::Filter>();
    createCustomFilter(*filter);
    return filter;
}

//
// from test_create_sidd_from_mem.cpp
//
static void initProcessorInformation(six::sidd::ProcessorInformation& processorInformation)
{
    processorInformation.application = "ProcessorName";
    processorInformation.profile = "Profile";
    processorInformation.site = "Ypsilanti, MI";
    processorInformation.processingDateTime = six::DateTime();
}

static void initDED(mem::ScopedCopyablePtr<six::sidd::DigitalElevationData>& ded)
{
    ded.reset(new six::sidd::DigitalElevationData());

    ded->geographicCoordinates.longitudeDensity = 1;
    ded->geographicCoordinates.latitudeDensity = 2;
    ded->geographicCoordinates.referenceOrigin.setLat(42);
    ded->geographicCoordinates.referenceOrigin.setLon(-83);

    ded->geopositioning.coordinateSystemType =
        six::sidd::CoordinateSystemType::UTM;
    ded->geopositioning.falseOrigin = 0;
    ded->geopositioning.utmGridZoneNumber = 15;

    ded->positionalAccuracy.numRegions = 3;
    ded->positionalAccuracy.absoluteAccuracyHorizontal = 4;
    ded->positionalAccuracy.absoluteAccuracyVertical = 5;
    ded->positionalAccuracy.pointToPointAccuracyHorizontal = 6;
    ded->positionalAccuracy.pointToPointAccuracyVertical = 7;

    ded->nullValue = -32768;
}

static int32_t getDESVersion(six::sidd300::ISMVersion ismVersion)
{
    if (ismVersion == six::sidd300::ISMVersion::v13)
    {
        return 13;
    }
    if (ismVersion == six::sidd300::ISMVersion::v201609)
    {
        return 201609;
    }
    throw std::invalid_argument("Unknown 'ISMVersion' value.");
}

static void initProductCreation(six::sidd::ProductCreation& productCreation, Version siddVersion, six::sidd300::ISMVersion ismVersion)
{
    productCreation.productName = "ProductName";
    productCreation.productClass = "Unclassified";
    productCreation.productType = "The product's type";
    six::Parameter parameter("sample");
    productCreation.productCreationExtensions.push_back(parameter);

    productCreation.classification.securityExtensions.push_back(parameter);
    if (siddVersion != Version::v3_0_0)
    {
        productCreation.classification.desVersion = 234; // existing code
    }
    else
    {
        productCreation.classification.desVersion = getDESVersion(ismVersion);        
    }
    productCreation.classification.createDate = six::DateTime();
    productCreation.classification.classification = "U";

    if (siddVersion == Version::v1_0_0)
    {
        productCreation.classification.compliesWith.push_back("ICD-710");
    }
    else
    {
        productCreation.classification.compliesWith.push_back("USGov");
    }
    productCreation.classification.ownerProducer.push_back("ABW");
    productCreation.classification.ownerProducer.push_back("AIA");
    productCreation.classification.sciControls.push_back("HCS");
    productCreation.classification.sciControls.push_back("SI");
    productCreation.classification.sarIdentifier.push_back("EU BC");
    productCreation.classification.disseminationControls.push_back("FOUO");
    productCreation.classification.disseminationControls.push_back("IMC");
    productCreation.classification.fgiSourceOpen.push_back("AIA");
    productCreation.classification.fgiSourceOpen.push_back("ALB");
    productCreation.classification.fgiSourceProtected.push_back("AIA");
    productCreation.classification.fgiSourceProtected.push_back("AND");
    productCreation.classification.releasableTo.push_back("ABW");
    productCreation.classification.releasableTo.push_back("AFG");
    productCreation.classification.nonICMarkings.push_back("SBU");
    productCreation.classification.nonICMarkings.push_back("DS");
    productCreation.classification.classifiedBy = "PVT Snuffy";
    productCreation.classification.compilationReason = "Testing purposes";
    productCreation.classification.derivativelyClassifiedBy = "Anna J. Stepp";
    productCreation.classification.classificationReason = "More testing";
    productCreation.classification.nonUSControls.push_back("BOHEMIA");
    productCreation.classification.nonUSControls.push_back("BALK");
    productCreation.classification.derivedFrom = "Other documents";
    productCreation.classification.declassDate.reset(new six::DateTime());
    productCreation.classification.declassEvent = "N/A";
    productCreation.classification.declassException = "25X1";
    productCreation.classification.exemptedSourceType = "X8";
    productCreation.classification.exemptedSourceDate.reset(new six::DateTime());
    productCreation.classification.exemptFrom = "IC_710_MANDATORY_FDR";
    productCreation.classification.joint = six::BooleanType::IS_TRUE;
    productCreation.classification.atomicEnergyMarkings.push_back("DCNI");
    productCreation.classification.displayOnlyTo.push_back("AIA");
    productCreation.classification.noticeType = "LES";
    productCreation.classification.noticeReason = "Notice reason here";
    productCreation.classification.noticeDate.reset(new six::DateTime());
    productCreation.classification.unregisteredNoticeType = "Foo";
    productCreation.classification.externalNotice = six::BooleanType::IS_FALSE;
    initProcessorInformation(productCreation.processorInformation);
}

static void initDisplay(six::sidd::Display& display, const std::string& lutType)
{
    // pixelType set in iniData() function
    if (lutType == "Mono")
    {
        display.remapInformation.reset(new six::sidd::MonochromeDisplayRemap("Some mono type", new LUT(256, 2)));
    }
    else
    {
        display.remapInformation.reset(new six::sidd::ColorDisplayRemap(new LUT(256, 3)));
        for (size_t ii = 0; ii < 50; ++ii)
        {
            *(*display.remapInformation->remapLUT)[ii] = 125;
        }
    }
    // magnificationMethod and decimationMethod set in populateData() function

    display.histogramOverrides.reset(new six::sidd::DRAHistogramOverrides());
    display.histogramOverrides->clipMin = 1;
    display.histogramOverrides->clipMax = 500;

    display.monitorCompensationApplied.reset(new six::sidd::MonitorCompensationApplied());
    display.monitorCompensationApplied->gamma = 5.9;
    display.monitorCompensationApplied->xMin = 0.87;

    display.numBands = 3;

    // NonInteractiveProcessing
    display.nonInteractiveProcessing.resize(1);
    display.nonInteractiveProcessing[0].reset(new six::sidd::NonInteractiveProcessing());
    auto& prodGenOptions = display.nonInteractiveProcessing[0]->productGenerationOptions;
    prodGenOptions.bandEqualization.reset(new six::sidd::BandEqualization());
    prodGenOptions.bandEqualization->algorithm = six::sidd::BandEqualizationAlgorithm::LUT_1D;
    prodGenOptions.bandEqualization->bandLUTs.resize(1);
    prodGenOptions.bandEqualization->bandLUTs[0].reset(new six::sidd::LookupTable());
    prodGenOptions.bandEqualization->bandLUTs[0]->lutName = "LUT Name";
    prodGenOptions.bandEqualization->bandLUTs[0]->predefined.reset(new six::sidd::LookupTable::Predefined());
    prodGenOptions.bandEqualization->bandLUTs[0]->predefined->remapFamily = 5;
    prodGenOptions.bandEqualization->bandLUTs[0]->predefined->remapMember = 3;

    prodGenOptions.modularTransferFunctionRestoration.reset(new six::sidd::Filter());
    createPredefinedFilter(*prodGenOptions.modularTransferFunctionRestoration);

    prodGenOptions.dataRemapping.reset(new six::sidd::LookupTable());
    prodGenOptions.dataRemapping->custom.reset(new six::sidd::LookupTable::Custom(256, 3));
    for (size_t ii = 0; ii < 3; ++ii)
    {
        auto& table = prodGenOptions.dataRemapping->custom->lutValues[ii].table;
        std::fill_n(table.begin(), table.size(), static_cast<uint8_t>(0));
    }

    prodGenOptions.asymmetricPixelCorrection.reset(createCustomFilter());

    display.nonInteractiveProcessing[0]->rrds.downsamplingMethod =  six::sidd::DownsamplingMethod::LAGRANGE;
    display.nonInteractiveProcessing[0]->rrds.antiAlias.reset(createCustomFilter());
    display.nonInteractiveProcessing[0]->rrds.interpolation.reset(createPredefinedFilter());

    // InteractiveProcessing
    display.interactiveProcessing.resize(1);
    display.interactiveProcessing[0].reset(new six::sidd::InteractiveProcessing());
    auto& geoTransform = display.interactiveProcessing[0]->geometricTransform;
    createPredefinedFilter(geoTransform.scaling.antiAlias);
    createCustomFilter(geoTransform.scaling.interpolation);
    geoTransform.orientation.shadowDirection = six::sidd::ShadowDirection::ARBITRARY;

    display.interactiveProcessing[0]->sharpnessEnhancement.modularTransferFunctionCompensation.reset(createCustomFilter());

    display.interactiveProcessing[0]->colorSpaceTransform.reset(new six::sidd::ColorSpaceTransform());
    auto& cmm = display.interactiveProcessing[0]->colorSpaceTransform->colorManagementModule;
    cmm.renderingIntent = six::sidd::RenderingIntent::ABSOLUTE_INTENT;
    cmm.sourceProfile = "Some source profile";
    cmm.displayProfile = "Some display profile";
    cmm.iccProfile = "Some ICC profile";

    auto& dra = display.interactiveProcessing[0]->dynamicRangeAdjustment;
    dra.algorithmType = six::sidd::DRAType::AUTO;
    dra.bandStatsSource = 1;
    dra.draParameters.reset(new six::sidd::DynamicRangeAdjustment::DRAParameters());
    dra.draParameters->pMin = 0.2;
    dra.draParameters->pMax = 0.8;
    dra.draParameters->eMinModifier = 0.1;
    dra.draParameters->eMaxModifier = 0.9;

    display.interactiveProcessing[0]->tonalTransferCurve.reset(new six::sidd::LookupTable());
    display.interactiveProcessing[0]->tonalTransferCurve->lutName = "TTC Name";
    display.interactiveProcessing[0]->tonalTransferCurve->predefined.reset(new six::sidd::LookupTable::Predefined());
    display.interactiveProcessing[0]->tonalTransferCurve->predefined->databaseName = "TTC DB";

    six::Parameter param;
    param.setName("name");
    param.setValue("val");
    display.displayExtensions.push_back(param);
}

static void initGeographicAndTarget(six::sidd::GeographicAndTarget& geographicAndTarget)
{
    six::Parameter param;
    param.setName("GeoName");
    param.setValue("GeoValue");

    mem::ScopedCopyablePtr<six::sidd::GeographicCoverage> geoCoverage(
        new six::sidd::GeographicCoverage(RegionType::GEOGRAPHIC_INFO));
    geoCoverage->georegionIdentifiers.push_back(param);

    mem::ScopedCopyablePtr<six::sidd::GeographicInformation> geoInfo(new six::sidd::GeographicInformation());
    geoInfo->countryCodes.push_back("US");
    geoInfo->countryCodes.push_back("GB");
    geoInfo->securityInformation = "SInformation";
    geoInfo->geographicInformationExtensions.push_back(param);

    geoCoverage->geographicInformation = geoInfo;
    geographicAndTarget.geographicCoverage = geoCoverage;
    geographicAndTarget.geographicCoverage->subRegion.push_back(geoCoverage);

    mem::ScopedCopyablePtr<six::sidd::TargetInformation> targetInfo(
        new six::sidd::TargetInformation());
    targetInfo->identifiers.push_back(param);
    targetInfo->footprint.reset(new six::LatLonCorners());
    targetInfo->targetInformationExtensions.push_back(param);
    geographicAndTarget.targetInformation.push_back(targetInfo);

    for (size_t ii = 0; ii < six::LatLonCorners::NUM_CORNERS; ++ii)
    {
        const auto ii_ = static_cast<double>(ii);
        auto footprint = geographicAndTarget.targetInformation[0]->footprint.get();
        footprint->getCorner(ii).setLat(ii_ + 1);
        footprint->getCorner(ii).setLon(ii_ * 3);
        footprint = &(geographicAndTarget.geographicCoverage->footprint);
        footprint->getCorner(ii).setLat(ii_ + 1);
        footprint->getCorner(ii).setLon(ii_ * 3);
    }
}

static void initGeoData(six::GeoDataBase& geoData)
{
    six::Parameter param;
    param.setName("GeoName");
    param.setValue("GeoValue");

    for (size_t ii = 0; ii < six::LatLonCorners::NUM_CORNERS; ++ii)
    {
        const auto ii_ = static_cast<double>(ii);
        geoData.imageCorners.getCorner(ii).setLat(ii_ + 1);
        geoData.imageCorners.getCorner(ii).setLon(ii_ * 3);
    }

    geoData.validData.push_back(six::LatLon(23, 34));
    geoData.validData.push_back(six::LatLon(23, 35));
    geoData.validData.push_back(six::LatLon(23, 36));

    mem::ScopedCopyablePtr<six::GeoInfo> newGeoInfo(new six::GeoInfo());
    newGeoInfo->name = "GeoInfo";
    newGeoInfo->desc.push_back(param);
    newGeoInfo->geometryLatLon.push_back(six::LatLon(36.5, 99.87));
    geoData.geoInfos.push_back(newGeoInfo);
}

static void initExploitationFeatures(six::sidd::ExploitationFeatures& exFeatures, Version siddVersion)
{
    // The first collection is corresponds to the parent image
    six::sidd::Collection& collection = *exFeatures.collections[0];

    collection.identifier = "ExCollection";

    //--------------------------------------------------------
    // Creating this stuff from a SICD source, normally these
    // values would mirror what was in the SICD XML
    // Here they are dummy values
    //--------------------------------------------------------
    collection.information.resolution.rg = 0;
    collection.information.resolution.az = 0;
    collection.information.collectionDuration = 0;

    // This demo sets the collection time to now (not true)
    collection.information.collectionDateTime = six::DateTime();
    collection.information.radarMode = RadarModeType::SPOTLIGHT;
    collection.information.radarModeID = "RMID";
    collection.information.sensorName = "Sensor Name";
    collection.information.localDateTime = six::DateTime();

    collection.information.inputROI.reset(new six::sidd::InputROI());
    collection.information.inputROI->size.row = 5;
    collection.information.inputROI->size.col = 10;
    collection.information.inputROI->upperLeft.row = 9;
    collection.information.inputROI->upperLeft.col = 3;

    mem::ScopedCopyablePtr<six::sidd::TxRcvPolarization> polarization(
        new six::sidd::TxRcvPolarization());
    polarization->txPolarization = six::PolarizationSequenceType::V;
    polarization->rcvPolarization = six::PolarizationSequenceType::OTHER;
    polarization->rcvPolarizationOffset = 1.37;
    if (siddVersion == Version::v1_0_0)
    {
        polarization->processed = six::BooleanType::IS_TRUE;
    }
    collection.information.polarization.push_back(polarization);

    collection.geometry.reset(new six::sidd::Geometry());
    collection.geometry->azimuth = 1.2;
    collection.geometry->slope = 3.4;
    collection.geometry->squint = 5.6;
    collection.geometry->graze = 7.8;
    collection.geometry->tilt = 9.1;
    six::Parameter param;
    param.setName("Extension");
    param.setValue("Parameter");
    collection.geometry->extensions.push_back(param);

    collection.phenomenology.reset(new six::sidd::Phenomenology());
    if (siddVersion != Version::v3_0_0)
    {
        // [-180, 180) before SIDD 3.0
        collection.phenomenology->shadow = six::AngleMagnitude(-1.5, 3.7);
        collection.phenomenology->layover = six::AngleMagnitude(-10.13, 50.9);
    }
    else
    {
        // [0, 360) in SIDD 3.0
        collection.phenomenology->shadow = six::AngleMagnitude(1.5, 3.7);
        collection.phenomenology->layover = six::AngleMagnitude(10.13, 50.9);
    }
    collection.phenomenology->multiPath = 3.79;
    collection.phenomenology->groundTrack = 8.11;
    collection.phenomenology->extensions.push_back(param);

    exFeatures.product.resize(1);
    exFeatures.product[0].resolution.row = 0;
    exFeatures.product[0].resolution.col = 0;
    exFeatures.product[0].north = 58.332;
    exFeatures.product[0].extensions.push_back(param);

    if (siddVersion == Version::v2_0_0)
    {
        exFeatures.product[0].ellipticity = 12.0;
        exFeatures.product[0].polarization.resize(1);
        exFeatures.product[0].polarization[0].txPolarizationProc =
            PolarizationSequenceType::SEQUENCE;
        exFeatures.product[0].polarization[0].rcvPolarizationProc =
            PolarizationSequenceType::RHC;
    }
}

static void initDownstreamReprocessing(six::sidd::DownstreamReprocessing& reprocess)
{
    reprocess.geometricChip.reset(new six::sidd::GeometricChip());
    reprocess.geometricChip->chipSize = RowColInt(1, 2);
    reprocess.geometricChip->originalUpperLeftCoordinate = RowColDouble(1.4, 2.9);
    reprocess.geometricChip->originalUpperRightCoordinate = RowColDouble(1.4, 2.9);
    reprocess.geometricChip->originalLowerLeftCoordinate = RowColDouble(1.4, 2.9);
    reprocess.geometricChip->originalLowerRightCoordinate = RowColDouble(1.4, 2.9);

    reprocess.processingEvents.push_back(mem::ScopedCopyablePtr<six::sidd::ProcessingEvent>(new six::sidd::ProcessingEvent()));
    reprocess.processingEvents[0]->applicationName = "Processing Event";
    reprocess.processingEvents[0]->appliedDateTime = six::DateTime();

    six::Parameter eventParameter;
    eventParameter.setName("Name");
    eventParameter.setValue("Value");
    reprocess.processingEvents[0]->descriptor.push_back(eventParameter);
}

static void initErrorStatistics(six::ErrorStatistics& err)
{
    err.compositeSCP.reset(new six::CompositeSCP());
    err.compositeSCP->xErr = 0.12;
    err.compositeSCP->yErr = 0.73;
    err.compositeSCP->xyErr = 9.84;

    err.components.reset(new six::Components());
    err.components->posVelError.reset(new six::PosVelError());
    err.components->posVelError->frame = six::FrameType("ECF");
    err.components->posVelError->p1 = 1.1;
    err.components->posVelError->p2 = 1.1;
    err.components->posVelError->p3 = 1.1;
    err.components->posVelError->v1 = 1.1;
    err.components->posVelError->v2 = 1.1;
    err.components->posVelError->v3 = 1.1;

    err.components->posVelError->corrCoefs.reset(new six::CorrCoefs());
    err.components->posVelError->corrCoefs->p1p2 = 6.2;
    err.components->posVelError->corrCoefs->p1p3 = 6.2;
    err.components->posVelError->corrCoefs->p1v1 = 6.2;
    err.components->posVelError->corrCoefs->p1v2 = 6.2;
    err.components->posVelError->corrCoefs->p1v3 = 6.2;
    err.components->posVelError->corrCoefs->p2p3 = 6.2;
    err.components->posVelError->corrCoefs->p2v1 = 6.2;
    err.components->posVelError->corrCoefs->p2v2 = 6.2;
    err.components->posVelError->corrCoefs->p2v3 = 6.2;
    err.components->posVelError->corrCoefs->p3v1 = 6.2;
    err.components->posVelError->corrCoefs->p3v2 = 6.2;
    err.components->posVelError->corrCoefs->p3v3 = 6.2;
    err.components->posVelError->corrCoefs->v1v2 = 6.2;
    err.components->posVelError->corrCoefs->v1v3 = 6.2;
    err.components->posVelError->corrCoefs->v2v3 = 6.2;
    err.components->posVelError->positionDecorr = six::DecorrType(48.17, 113.965);

    err.components->radarSensor.reset(new six::RadarSensor());
    err.components->radarSensor->rangeBias = 43.5;
    err.components->radarSensor->clockFreqSF = 1111.1;
    err.components->radarSensor->transmitFreqSF = 85;
    err.components->radarSensor->rangeBiasDecorr = six::DecorrType(123, .03);

    err.components->tropoError.reset(new six::TropoError());
    err.components->tropoError->tropoRangeVertical = .00289;
    err.components->tropoError->tropoRangeSlant = 777;
    err.components->tropoError->tropoRangeDecorr = six::DecorrType(0, 98.7);

    err.components->ionoError.reset(new six::IonoError());
    err.components->ionoError->ionoRangeVertical = 1.2;
    err.components->ionoError->ionoRangeRateVertical = 77632;
    err.components->ionoError->ionoRgRgRateCC = .072;
    err.components->ionoError->ionoRangeVertDecorr = six::DecorrType(48.16, 113.964);

    six::Parameter param;
    param.setName("ErrorStatisticsParameterName");
    param.setValue("Error StatisticsParameterValue");

    err.additionalParameters.push_back(param);
}

static void initRadiometric(six::Radiometric& radiometric)
{
    radiometric.noiseLevel.noiseType = "ABSOLUTE";
    radiometric.noiseLevel.noisePoly = six::Poly2D(0, 0);

    radiometric.rcsSFPoly = six::Poly2D(0, 0);
    radiometric.betaZeroSFPoly = six::Poly2D(1, 3);
    radiometric.sigmaZeroSFPoly = six::Poly2D(0, 0);
    radiometric.sigmaZeroSFIncidenceMap = six::AppliedType::IS_TRUE;
    radiometric.gammaZeroSFPoly = six::Poly2D(0, 0);
    radiometric.gammaZeroSFIncidenceMap = six::AppliedType::IS_FALSE;
}

static void initAnnotations(six::sidd::Annotations& annotations)
{
    mem::ScopedCopyablePtr<six::sidd::Annotation> annotation(new six::sidd::Annotation("Annotation identifier"));

    annotation->spatialReferenceSystem.reset(new six::sidd::SFAReferenceSystem());
    annotation->spatialReferenceSystem->axisNames.push_back("Axis 1");
    annotation->spatialReferenceSystem->coordinateSystem.reset(new six::sidd::SFAGeocentricCoordinateSystem());
    auto system = static_cast<six::sidd::SFAGeocentricCoordinateSystem*>(annotation->spatialReferenceSystem->coordinateSystem.get());
    system->csName = "CS Name";
    system->datum.spheroid.name = "Spheroid";
    system->datum.spheroid.semiMajorAxis = 69.03;
    system->datum.spheroid.inverseFlattening = 71.473;
    system->primeMeridian.name = "Prime Meridian";
    system->primeMeridian.longitude = 11.11;
    system->linearUnit = "Unit";

    annotation->objects.resize(1);
    annotation->objects[0].reset(new six::sidd::SFAPoint(2.4, 3.5, 4.55, 5.757));
    annotations.push_back(annotation);
}

static void initProductProcessing(six::sidd::ProductProcessing& processing)
{
    mem::ScopedCopyablePtr<six::sidd::ProcessingModule> module(new six::sidd::ProcessingModule());
    mem::ScopedCopyablePtr<six::sidd::ProcessingModule> nestedModule(new six::sidd::ProcessingModule());
    six::Parameter moduleParameter;
    moduleParameter.setName("Name");
    moduleParameter.setValue("Value");

    module->moduleName = moduleParameter;
    nestedModule->moduleName = moduleParameter;
    nestedModule->moduleParameters.push_back(moduleParameter);

    module->processingModules.push_back(nestedModule);
    processing.processingModules.push_back(module);
}

static void populateData(six::sidd::DerivedData& siddData, const std::string& lutType = "Mono")
{
    const auto siddVersion = siddData.getSIDDVersion();

    constexpr bool smallImage = true;
    const auto elementSize = static_cast<size_t>(lutType == "Mono" ? 2 : 3);

    if ((siddVersion == Version::v2_0_0) || (siddVersion == Version::v3_0_0))
    {
        // This will naturally get constructed in the course of 1.0.0
        // Separate field in 2.0.0
        siddData.setDisplayLUT(std::make_unique<six::AmplitudeTable>(elementSize));

        for (size_t ii = 0; ii < siddData.getDisplayLUT()->table.size(); ++ii)
        {
            siddData.getDisplayLUT()->table[ii] = ii % 128;
        }
    }
    // These things are essential to forming the file
    if (smallImage)
    {
        siddData.setNumRows(2);
        siddData.setNumCols(2);
    }

    initDisplay(*siddData.display, lutType);
    initGeographicAndTarget(*siddData.geographicAndTarget);
    initGeoData(*siddData.geoData);

    //siddData.setImageCorners(makeUpCornersFromDMS());

    // Can certainly be init'ed in a function
    initProductCreation(*siddData.productCreation, siddVersion, siddData.getISMVersion());

    // Or directly if preferred
    siddData.display->decimationMethod = DecimationMethod::BRIGHTEST_PIXEL;
    siddData.display->magnificationMethod = MagnificationMethod::NEAREST_NEIGHBOR;

    //---------------------------------------------------------------
    // We can only do this because we know it's PGD in this example
    // If you don't know which it is, you don't need to use a
    // dynamic_cast<> since there is an enum in projection for this:
    // siddData.measurement->projection->projectionType
    // In this case:
    //    == six::PLANE
    //---------------------------------------------------------------
    auto planeProjection = static_cast<six::sidd::PlaneProjection*>(siddData.measurement->projection.get());

    //--------------------------------------------------
    // This is creating a constant-term polynomial 2D
    // 2D Polynomials are laid out as a matrix of terms
    // e.g.,
    // 2*x^n + 3*y^1*x^1 + 7 =
    //
    //     | y^0  y^1 ... y^n
    //-------------------------------
    // x^0 |  7    0   0   0
    // x^1 |  0    3   0   0
    // ... |  0    0   0   0
    // x^n |  2    0   0   0
    //--------------------------------------------------
    planeProjection->timeCOAPoly = six::Poly2D(0, 0);
    planeProjection->timeCOAPoly[0][0] = 1;

    //--------------------------------------------------
    // This is creating a constant-term polynomial of vectors
    // In this case, an index into the polynomial yields a
    // six::Vector3
    //--------------------------------------------------
    siddData.measurement->arpPoly = six::PolyXYZ(0);

    // The constant term is a vector.  Each component is 0
    siddData.measurement->arpPoly[0] = six::Vector3(0.0);

    siddData.measurement->validData.push_back(RowColInt(3, 2));
    siddData.measurement->validData.push_back(RowColInt(3, 1));
    siddData.measurement->validData.push_back(RowColInt(3, 0));

    //----------------------------------------------------
    // The basis vectors are dummied, each component is 0
    // This works because Vector3 supports assignment from
    // a scalar, assuming that each component should be
    // set to that value.  This is not really math, just
    // C++ syntactic sugar.  Vector3 supports most math
    // operations that one would expect
    //----------------------------------------------------
    planeProjection->productPlane.rowUnitVector = six::Vector3(0.0);
    planeProjection->productPlane.colUnitVector = six::Vector3(0.0);

    initExploitationFeatures(*siddData.exploitationFeatures, siddVersion);
    initDED(siddData.digitalElevationData);
    initProductProcessing(*siddData.productProcessing);
    initDownstreamReprocessing(*siddData.downstreamReprocessing);
    initErrorStatistics(*siddData.errorStatistics);
    initRadiometric(*siddData.radiometric);
    initAnnotations(siddData.annotations);
}

static void update_for_SIDD_300(DerivedData& data) // n.b., much of this was added before SIDD 3.0
{
    auto& display = *(data.display);
    data.display->pixelType = six::PixelType::RGB24I; // TODO: this was added before SIDD 3.0.0
    data.display->numBands = 3;

    data.display->nonInteractiveProcessing.resize(1); // TODO: this was added before SIDD 3.0.0
    data.display->nonInteractiveProcessing[0].reset(new NonInteractiveProcessing());
    data.display->nonInteractiveProcessing[0]->rrds.downsamplingMethod = DownsamplingMethod::MAX_PIXEL;

    data.display->interactiveProcessing.resize(1); // TODO: this was added before SIDD 3.0.0
    data.display->interactiveProcessing[0].reset(new InteractiveProcessing());

    data.display->interactiveProcessing[0]->sharpnessEnhancement.modularTransferFunctionCompensation.reset(createCustomFilter());

    auto& geoTransform = data.display->interactiveProcessing[0]->geometricTransform;
    createPredefinedFilter(geoTransform.scaling.antiAlias);
    createCustomFilter(geoTransform.scaling.interpolation);
    geoTransform.orientation.shadowDirection = ShadowDirection::ARBITRARY;

    display.interactiveProcessing[0]->colorSpaceTransform.reset(new ColorSpaceTransform());
    auto& cmm = display.interactiveProcessing[0]->colorSpaceTransform->colorManagementModule;
    cmm.renderingIntent = six::sidd::RenderingIntent::ABSOLUTE_INTENT;
    cmm.sourceProfile = "Some source profile";
    cmm.displayProfile = "Some display profile";
    cmm.iccProfile = "Some ICC profile";

    auto& dra = display.interactiveProcessing[0]->dynamicRangeAdjustment;
    dra.algorithmType = six::sidd::DRAType::AUTO;
    dra.bandStatsSource = 1;
    dra.draParameters.reset(new DynamicRangeAdjustment::DRAParameters());
    dra.draParameters->pMin = 0.2;
    dra.draParameters->pMax = 0.8;
    dra.draParameters->eMinModifier = 0.1;
    dra.draParameters->eMaxModifier = 0.9;

    display.interactiveProcessing[0]->tonalTransferCurve.reset(new six::sidd::LookupTable());
    display.interactiveProcessing[0]->tonalTransferCurve->lutName = "TTC Name";
    display.interactiveProcessing[0]->tonalTransferCurve->predefined.reset(new LookupTable::Predefined());
    display.interactiveProcessing[0]->tonalTransferCurve->predefined->databaseName = "TTC DB";


    data.measurement->pixelFootprint.row = data.measurement->pixelFootprint.col = 0; // TODO: this was added before SIDD 3.0.0
    data.measurement->validData.emplace_back(1, 2); // TODO: this was added before SIDD 3.0.0
    data.measurement->validData.emplace_back(3, 4);
    data.measurement->validData.emplace_back(5, 6);

    data.geoData.reset(new six::GeoDataBase()); // TODO: this was added before SIDD 3.0.0
    data.geoData->imageCorners.lowerLeft.clearLatLon();
    data.geoData->imageCorners.upperLeft.clearLatLon();
    data.geoData->imageCorners.lowerRight.clearLatLon();
    data.geoData->imageCorners.upperRight.clearLatLon();

    data.exploitationFeatures->product[0].ellipticity = 0.0; // TODO: this was added before SIDD 3.0.0
    ProcTxRcvPolarization polarization{ PolarizationSequenceType::UNKNOWN, PolarizationSequenceType::UNKNOWN };
    data.exploitationFeatures->product[0].polarization.push_back(polarization); // TODO: this was added before SIDD 3.0.0

    populateData(data);
}

static std::unique_ptr<DerivedData> createFakeDerivedData_(const Version* pSiddVersion, const six::sidd300::ISMVersion* pISMVersion = nullptr)
{
    std::unique_ptr<DerivedData> data;
    if (pSiddVersion != nullptr) // preserve behavior of existing code
    {
        //-----------------------------------------------------------
        // Make the object.  You could do this directly, but this way
        // is less error prone, and more flexible
        //-----------------------------------------------------------
        six::sidd::DerivedDataBuilder siddBuilder;
        //-----------------------------------------------------------
        // You can cascade these operators, or call them one after
        // the other.  Here is how you add them individually
        //-----------------------------------------------------------
        //siddBuilder.addDisplay(pixelType);
        siddBuilder.addGeographicAndTarget(RegionType::GEOGRAPHIC_INFO);
        siddBuilder.addGeoData();

        // Here is how you can cascade them
        siddBuilder.addMeasurement(ProjectionType::PLANE).addExploitationFeatures(1);

        siddBuilder.addProductProcessing();
        siddBuilder.addDownstreamReprocessing();
        siddBuilder.addErrorStatistics();
        siddBuilder.addRadiometric();

        //---------------------------------------------------------
        // Take ownership of the SIDD data, the builder still can
        // manipulate the same pointer after this happens if you
        // want it to, but it won't try to release it when it goes
        // out of scope if you steal() it.
        //---------------------------------------------------------
        data.reset(siddBuilder.steal());
    }
    else
    {
        data = std::make_unique<DerivedData>();
    }

    if (pSiddVersion != nullptr)
    {
        if (pISMVersion != nullptr)
        {
            data->setSIDDVersion(*pSiddVersion, *pISMVersion);
        }
        else
        {
            data->setSIDDVersion(*pSiddVersion);
        }       
    }

    data->productCreation->classification.classification = "U";
    data->display.reset(new Display());
    data->geographicAndTarget.reset(new GeographicAndTarget());
    data->geographicAndTarget->geographicCoverage.reset(
            new GeographicCoverage(RegionType::SUB_REGION));
    data->geographicAndTarget->geographicCoverage->geographicInformation.reset(
            new GeographicInformation());
    LatLonCorners& imageCorners =
            data->geographicAndTarget->geographicCoverage->footprint;
    imageCorners.getCorner(0).setLat(3.001596271329710E01);
    imageCorners.getCorner(0).setLon(-9.331563100917214E01);

    imageCorners.getCorner(1).setLat(3.051902395021247E01);
    imageCorners.getCorner(1).setLon(-9.341349915372815E01);

    imageCorners.getCorner(2).setLat(3.054375544528100E01);
    imageCorners.getCorner(2).setLon(-9.324399299230399E01);

    imageCorners.getCorner(3).setLat(3.004057030343078E01);
    imageCorners.getCorner(3).setLon(-9.314696314152951E01);

    data->measurement.reset(new Measurement(ProjectionType::PLANE));

    auto projection = dynamic_cast<PlaneProjection*>(data->measurement->projection.get());
    projection->timeCOAPoly = Poly2D(1, 1);
    projection->timeCOAPoly[0][0] = 0;
    projection->timeCOAPoly[1][0] = 0;
    projection->timeCOAPoly[0][1] = 0;
    projection->timeCOAPoly[1][1] = 0;
    projection->productPlane.rowUnitVector[0] = 0;
    projection->productPlane.rowUnitVector[1] = 0;
    projection->productPlane.rowUnitVector[2] = 0;

    projection->productPlane.colUnitVector[0] = 0;
    projection->productPlane.colUnitVector[1] = 0;
    projection->productPlane.colUnitVector[2] = 0;

    data->measurement->arpPoly = PolyXYZ(1);
    data->measurement->arpPoly[0][0] = 0;
    data->measurement->arpPoly[0][1] = 0;
    data->measurement->arpPoly[0][2] = 0;

    data->exploitationFeatures.reset(new ExploitationFeatures());
    data->exploitationFeatures->collections.push_back(mem::ScopedCopyablePtr<Collection>());
    data->exploitationFeatures->collections[0].reset(new Collection());
    auto& information = data->exploitationFeatures->collections[0]->information;
    information.radarMode = RadarModeType::SPOTLIGHT;
    information.collectionDuration = 0;
    information.resolution.rg = 0;
    information.resolution.az = 0;

    data->exploitationFeatures->product.resize(1);
    data->exploitationFeatures->product[0].resolution.row = 0;
    data->exploitationFeatures->product[0].resolution.col = 0;

    if (pSiddVersion != nullptr) // TODO: better check for version; this avoid changing any existing test code
    {
        update_for_SIDD_300(*data);
    }

    return data;
}
std::unique_ptr<DerivedData> Utilities::createFakeDerivedData(Version siddVersion)
{
    if (siddVersion == Version::v3_0_0)
    {
        throw std::invalid_argument("Must use ISMVersion overload.");
    }
    if (siddVersion == Version::v2_0_0)
    {
        return createFakeDerivedData_(&siddVersion);
    }
    throw std::invalid_argument("SIDD version = '" + to_string(siddVersion) + "' is not supported.");
}
std::unique_ptr<DerivedData> Utilities::createFakeDerivedData(Version siddVersion, six::sidd300::ISMVersion ismVersion)
{
    if (siddVersion != Version::v3_0_0)
    {
        return createFakeDerivedData(siddVersion);
    }
    return createFakeDerivedData_(&siddVersion, &ismVersion);
}
std::unique_ptr<DerivedData> Utilities::createFakeDerivedData()
{
    return createFakeDerivedData_(nullptr);
}

}
}
